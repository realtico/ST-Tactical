#ifndef GALAXY_H
#define GALAXY_H

#include <stdint.h>
#include <stdbool.h>

#define QUADRANT_GRID_SIZE 12
#define SECTOR_GRID_SIZE 8
#define MAX_ENEMIES 250

// Tipos de Entidade no Setor
typedef enum {
    ENTITY_EMPTY = 0,
    ENTITY_STAR,
    ENTITY_BASE,
    ENTITY_ENEMY
} EntityType;

// IA e Comportamento Especial (Angband-like flags)
typedef enum {
    AI_SCOUT,     // Foco: Agilidade. Fuga para Zona Neutra < 20% de HP
    AI_CRUISER,   // Foco: Combate. Patrulha Agressiva
    AI_HEAVY      // Foco: Tanque. Movimento limitado, Cerco de bases
} AIBehavior;

// Definição do Tipo/Classe de Nave (O "DNA" / Mock do JSON Angband-style)
typedef struct {
    uint8_t      class_id;
    const char*  name;
    const char*  symbol;     // Representação ASCII no terminal
    uint16_t     base_hp;
    uint16_t     base_shields;
    AIBehavior   ai_policy;
    bool         can_cloak;
    int          spawn_weight; // Peso base proporcional de aparição global
} ShipClassDef;

// Estrutura de Inimigo no Pool
typedef struct {
    uint16_t id;              // ID único (índice no pool)
    uint8_t  class_id;        // Classe (Refere-se ao ShipClassDef)
    uint8_t  qx;              // Coordenada do Quadrante X (0-11)
    uint8_t  qy;              // Coordenada do Quadrante Y (0-11)
    uint8_t  sx;              // Coordenada do Setor X (0-7)
    uint8_t  sy;              // Coordenada do Setor Y (0-7)
    uint16_t current_hp;      // HP atual
    uint16_t current_shields; // Escudos atuais
    bool     active;          // true se estiver vivo/ativo
} KlingonGroup;

// Estrutura do Setor (8x8 por Quadrante)
typedef struct {
    EntityType entity_type; // Vazio, Estrela, Base, Inimigo
    uint16_t entity_id;     // Se for Inimigo, aponta para o índice em KlingonGroup pool. Caso contrário, ignorado ou usado para outros IDs.
} Sector;

// Estrutura do Quadrante (12x12 na Galáxia)
typedef struct {
    Sector sectors[SECTOR_GRID_SIZE][SECTOR_GRID_SIZE];
    uint8_t num_stars;
    bool has_base;
    uint8_t num_enemies;
} Quadrant;

// Pool estático global para a engine
// Em purismo de C, o header só expõe (extern) se necessário. Mas pela nota do Arquiteto, "Crie um pool estático KlingonGroup enemies[MAX_ENEMIES]".
// Faremos isso no source e daremos acesso via funções se preciso.

// Estado completo da Galáxia instanciável
typedef struct {
    uint32_t seed_state;
    uint16_t enemies_count;

    // Memória de Sensores
    double last_scan_date[12][12];
    uint8_t last_known_klingons[12][12];

    KlingonGroup enemy_pool[MAX_ENEMIES];
    Quadrant galaxy_map[QUADRANT_GRID_SIZE][QUADRANT_GRID_SIZE];
} GalaxyState;

/* Inicializa a Galáxia a partir de uma seed */
void Galaxy_Generate(GalaxyState* state, uint32_t seed);

/* Recupera acesso de leitura para a Galáxia e o Pool de inimigos para renderização, etc */
Quadrant* Galaxy_GetQuadrant(GalaxyState* state, int qx, int qy);
KlingonGroup* Galaxy_GetEnemy(GalaxyState* state, uint16_t id);

/* Salva um log ASCII em arquivo */
void Galaxy_DumpToLog(GalaxyState* state, const char* filepath);
void Galaxy_UpdateSensors(GalaxyState* state, int qX, int qY, double stardate, bool isLRS);

#endif // GALAXY_H
