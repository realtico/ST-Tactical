#include "game/galaxy.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Bestiário (simulando a leitura de um arquivo conf JSON estilo Angband - Zero Malloc state)
// class_id, name, symbol, base_hp, base_shields, ai_policy, can_cloak, spawn_weight
const ShipClassDef KlingonBestiary[] = {
    { 0, "B'rel",    "r",  500,   300,  AI_SCOUT,   true,  100 },
    { 1, "K't'inga", "K",  1200,  800,  AI_CRUISER, false, 50  },
    { 2, "Vor'cha",  "V",  2500, 1500,  AI_HEAVY,   false, 20  }
};
const int NumKlingonClasses = 3;

// O próprio pool de inimigos para toda a galáxia

static void Galaxy_SRand(GalaxyState* state, uint32_t seed) {
    state->seed_state = (seed == 0) ? 1 : seed;
}

static uint32_t Galaxy_Rand(GalaxyState* state) {
    uint32_t x = state->seed_state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    return state->seed_state = x;
}

// Generate an enemy inside a specific quadrant and cell
static bool Galaxy_SpawnEnemy(GalaxyState* state, uint8_t qx, uint8_t qy, uint8_t sx, uint8_t sy, uint8_t class_id) {
    if (state->enemies_count >= MAX_ENEMIES) return false;
    if (class_id >= NumKlingonClasses) class_id = 0; // fallback safe
    
    // Alocar no pool estático
    uint16_t id = state->enemies_count++;
    KlingonGroup* e = &(state->enemy_pool[id]);
    
    const ShipClassDef* def = &KlingonBestiary[class_id];
    
    e->id = id;
    e->class_id = class_id;
    e->qx = qx;
    e->qy = qy;
    e->sx = sx;
    e->sy = sy;
    e->active = true;
    
    // Pequena variação (Angband-like): +- 5% no HP baseado na classe principal
    int hp_var = (int)def->base_hp / 20; 
    int random_hp_mod = (Galaxy_Rand(state) % (hp_var * 2 + 1)) - hp_var;
    
    e->current_hp = def->base_hp + random_hp_mod;
    e->current_shields = def->base_shields;
    
    // Atualizar no mapa
    state->galaxy_map[qy][qx].sectors[sy][sx].entity_type = ENTITY_ENEMY;
    state->galaxy_map[qy][qx].sectors[sy][sx].entity_id = id;
    state->galaxy_map[qy][qx].num_enemies++;
    
    return true;
}

void Galaxy_Generate(GalaxyState* state, uint32_t seed) {
    Galaxy_SRand(state, seed);
    
    // Reset galaxy state
        state->enemies_count = 0;

    for (int y = 0; y < 12; y++) {
        for (int x = 0; x < 12; x++) {
            state->last_scan_date[y][x] = -1.0;
            state->last_known_klingons[y][x] = 0;
        }
    }

    memset(state->galaxy_map, 0, sizeof(state->galaxy_map));
    
    // Populate the galaxy quadrand by quadrant
    for (int qy = 0; qy < QUADRANT_GRID_SIZE; qy++) {
        for (int qx = 0; qx < QUADRANT_GRID_SIZE; qx++) {
            Quadrant* q = &state->galaxy_map[qy][qx];
            
            // 1. Stars: 1 to 9
            q->num_stars = (Galaxy_Rand(state) % 9) + 1;
            for (int i = 0; i < q->num_stars; i++) {
                int sx, sy;
                int attempts = 0;
                do {
                    sx = Galaxy_Rand(state) % SECTOR_GRID_SIZE;
                    sy = Galaxy_Rand(state) % SECTOR_GRID_SIZE;
                    attempts++;
                } while (q->sectors[sy][sx].entity_type != ENTITY_EMPTY && attempts < 100);
                
                if (attempts >= 100) continue;
                
                q->sectors[sy][sx].entity_type = ENTITY_STAR;
            }
            
            // 2. Starbases: Maior propabilidade no Sudoeste (qx baixo, qy alto)
            // Sudoeste: x < 6, y >= 6
            int base_chance = (qx < 6 && qy >= 6) ? 15 : 2; // 15% vs 2% de ter base
            if ((Galaxy_Rand(state) % 100) < (uint32_t)base_chance) {
                int sx, sy;
                int attempts = 0;
                do {
                    sx = Galaxy_Rand(state) % SECTOR_GRID_SIZE;
                    sy = Galaxy_Rand(state) % SECTOR_GRID_SIZE;
                    attempts++;
                } while (q->sectors[sy][sx].entity_type != ENTITY_EMPTY && attempts < 100);
                
                if (attempts >= 100) continue;
                
                q->sectors[sy][sx].entity_type = ENTITY_BASE;
                q->has_base = true;
            }
            
            // 3. Nível de Perigo Geográfico (Gamma) e Inimigos
            // 0,11 (Sudoeste) -> 0.   11,0 (Nordeste) -> 1.0.
            float gamma = (qx + (11.0f - qy)) / 22.0f;
            
            // Probabilidade de ter grupo de inimigos escala com Gamma: min 10%, max 85%
            int enemy_prob = 10 + (int)(gamma * 75);
            
            if ((Galaxy_Rand(state) % 100) < (uint32_t)enemy_prob) {
                // Quantos inimigos? 1 a 3+ dependendo de quão profundo é o perigo
                int e_count = (Galaxy_Rand(state) % 3) + 1;
                if (gamma > 0.8f) e_count++; // Cerco/Formação Extra!
                
                for (int i = 0; i < e_count; i++) {
                    int sx, sy;
                    int attempts = 0;
                    do {
                        sx = Galaxy_Rand(state) % SECTOR_GRID_SIZE;
                        sy = Galaxy_Rand(state) % SECTOR_GRID_SIZE;
                        attempts++;
                    } while (q->sectors[sy][sx].entity_type != ENTITY_EMPTY && attempts < 100);
                    if (attempts >= 100) continue;

                    
                    // Lógica proporcional de Bestiário (peso dinâmico via cast/gamma)
                    int w_brel   = (int)(KlingonBestiary[0].spawn_weight * (1.2f - gamma)); // cai no Nordeste
                    int w_ktinga = KlingonBestiary[1].spawn_weight;                         // Linear no mapa
                    int w_vorcha = (int)(KlingonBestiary[2].spawn_weight + (100 * gamma));  // Sobe drasticamente no Nordeste
                    
                    int total_weight = w_brel + w_ktinga + w_vorcha;
                    int r = Galaxy_Rand(state) % total_weight;
                    
                    uint8_t cls = 0;
                    if (r < w_brel) cls = 0; // B'rel
                    else if (r < w_brel + w_ktinga) cls = 1; // K't'inga
                    else cls = 2; // Vor'cha
                    
                    Galaxy_SpawnEnemy(state, qx, qy, sx, sy, cls);
                }
            }
        }
    }
}

Quadrant* Galaxy_GetQuadrant(GalaxyState* state, int qx, int qy) {
    if (qx < 0 || qy < 0 || qx >= QUADRANT_GRID_SIZE || qy >= QUADRANT_GRID_SIZE)
        return NULL;
    return &state->galaxy_map[qy][qx];
}

KlingonGroup* Galaxy_GetEnemy(GalaxyState* state, uint16_t id) {
    if (id >= state->enemies_count) return NULL;
    return &(state->enemy_pool[id]);
}

void Galaxy_DumpToLog(GalaxyState* state, const char* filepath) {
    FILE* f = fopen(filepath ? filepath : "galaxy_debug.txt", "w");
    if (!f) return;
    
    // ... rest of dump ...
    fprintf(f, "ST-TACTICAL GALAXY DUMP\n");
    fprintf(f, "=======================\n\n");
    
    fprintf(f, "GALAXY MAP (96x96):\n");
    for (int qy = 0; qy < QUADRANT_GRID_SIZE; qy++) {
        for (int sy = 0; sy < SECTOR_GRID_SIZE; sy++) {
            for (int qx = 0; qx < QUADRANT_GRID_SIZE; qx++) {
                for (int sx = 0; sx < SECTOR_GRID_SIZE; sx++) {
                    Sector* s = &state->galaxy_map[qy][qx].sectors[sy][sx];
                    char c = '.';
                    if (s->entity_type == ENTITY_STAR) c = '*';
                    else if (s->entity_type == ENTITY_BASE) c = 'B';
                    else if (s->entity_type == ENTITY_ENEMY) {
                        KlingonGroup *e = &state->enemy_pool[s->entity_id];
                        c = KlingonBestiary[e->class_id].symbol[0];
                    }
                    fprintf(f, "%c", c);
                }
                fprintf(f, " "); // Spacer entre quadrantes
            }
            fprintf(f, "\n");
        }
        fprintf(f, "\n"); // Spacer entre linhas de quadrantes
    }
    
    fprintf(f, "ENEMY ROSTER (TOTAL: %d):\n", state->enemies_count);
    fprintf(f, "===========================================================\n");
    fprintf(f, "ID   CLASS      Q(X,Y)  S(X,Y)   HP     SHIELDS  BEHAVIOR  \n");
    for (int i = 0; i < state->enemies_count; i++) {
        KlingonGroup* e = &state->enemy_pool[i];
        const ShipClassDef* def = &KlingonBestiary[e->class_id];
        
        const char* ai_str = "UNK";
        if (def->ai_policy == AI_SCOUT) ai_str = "SCOUT";
        else if (def->ai_policy == AI_CRUISER) ai_str = "CRUISER";
        else if (def->ai_policy == AI_HEAVY) ai_str = "SIEGE";
        
        fprintf(f, "%-4d %-10s (%2d,%2d) (%2d,%2d)  %-6d %-8d %-8s\n", 
                e->id, def->name, e->qx, e->qy, e->sx, e->sy, 
                e->current_hp, e->current_shields, ai_str);
    }
    
    fclose(f);
}

void Galaxy_UpdateSensors(GalaxyState* state, int qX, int qY, double stardate, bool isLRS) {
    if (isLRS) {
        for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
                int cx = qX + dx;
                int cy = qY + dy;
                if (cx >= 0 && cx < 12 && cy >= 0 && cy < 12) {
                    state->last_scan_date[cy][cx] = stardate;
                    Quadrant* q = Galaxy_GetQuadrant(state, cx, cy);
                    state->last_known_klingons[cy][cx] = q ? q->num_enemies : 0;
                }
            }
        }
    } else {
        if (qX >= 0 && qX < 12 && qY >= 0 && qY < 12) {
            state->last_scan_date[qY][qX] = stardate;
            Quadrant* q = Galaxy_GetQuadrant(state, qX, qY);
            state->last_known_klingons[qY][qX] = q ? q->num_enemies : 0;
        }
    }
}
