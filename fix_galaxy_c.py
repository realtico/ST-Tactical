import re

with open("src/game/galaxy.c", "r") as f:
    text = f.read()

# 1. Remove globals
text = re.sub(r"// Database Estático.*?\nconst ShipClassDef KlingonBestiary", "// Database Estático\nconst ShipClassDef KlingonBestiary", text, flags=re.DOTALL)
text = re.sub(r"static KlingonGroup enemy_pool\[MAX_ENEMIES\];\s*static uint16_t enemies_count = 0;\s*// O mapa da galáxia.*?;.*?static uint32_t seed_state = 1;\n", "", text, flags=re.DOTALL)
text = re.sub(r"// Custom PRNG.*?static uint32_t seed_state = 1;", "", text, flags=re.DOTALL)

# 2. Fix SRand and Rand to take state
text = text.replace("static void Galaxy_SRand(uint32_t seed) {", "static void Galaxy_SRand(GalaxyState* state, uint32_t seed) {")
text = text.replace("seed_state = (seed == 0) ? 1 : seed;", "state->seed_state = (seed == 0) ? 1 : seed;")
text = text.replace("static uint32_t Galaxy_Rand(void) {", "static uint32_t Galaxy_Rand(GalaxyState* state) {")
text = text.replace("uint32_t x = seed_state;", "uint32_t x = state->seed_state;")
text = text.replace("return seed_state = x;", "return state->seed_state = x;")

# 3. Add state parameter to internal functions and update implementations
text = text.replace("static bool Galaxy_SpawnEnemy(uint8_t qx, uint8_t qy, uint8_t sx, uint8_t sy, uint8_t class_id) {", "static bool Galaxy_SpawnEnemy(GalaxyState* state, uint8_t qx, uint8_t qy, uint8_t sx, uint8_t sy, uint8_t class_id) {")
text = text.replace("if (enemies_count >= MAX_ENEMIES)", "if (state->enemies_count >= MAX_ENEMIES)")
text = text.replace("uint16_t id = enemies_count++;", "uint16_t id = state->enemies_count++;")
text = text.replace("KlingonGroup* e = &enemy_pool[id];", "KlingonGroup* e = &(state->enemy_pool[id]);")
text = text.replace("int random_hp_mod = (Galaxy_Rand() % (hp_var * 2 + 1)) - hp_var;", "int random_hp_mod = (Galaxy_Rand(state) % (hp_var * 2 + 1)) - hp_var;")
text = text.replace("galaxy_map", "state->galaxy_map")

# 4. Update Generate
text = text.replace("void Galaxy_Generate(uint32_t seed) {", "void Galaxy_Generate(GalaxyState* state, uint32_t seed) {")
text = text.replace("enemies_count = 0;", "state->enemies_count = 0;")
text = text.replace("Galaxy_SRand(seed);", "Galaxy_SRand(state, seed);")
text = text.replace("Galaxy_Rand()", "Galaxy_Rand(state)")
text = text.replace("Galaxy_SpawnEnemy(qx, qy, sx, sy, cls);", "Galaxy_SpawnEnemy(state, qx, qy, sx, sy, cls);")

# 5. Update getters
text = text.replace("Quadrant* Galaxy_GetQuadrant(int qx, int qy) {", "Quadrant* Galaxy_GetQuadrant(GalaxyState* state, int qx, int qy) {")
text = text.replace("KlingonGroup* Galaxy_GetEnemy(uint16_t id) {", "KlingonGroup* Galaxy_GetEnemy(GalaxyState* state, uint16_t id) {")
text = text.replace("if (id >= enemies_count)", "if (id >= state->enemies_count)")
text = text.replace("return &enemy_pool[id];", "return &(state->enemy_pool[id]);")

# 6. Update Dump
text = text.replace("void Galaxy_DumpToLog(void) {", "void Galaxy_DumpToLog(GalaxyState* state, const char* filepath) {")
text = text.replace('FILE* f = fopen("galaxy_debug.txt", "w");', 'FILE* f = fopen(filepath ? filepath : "galaxy_debug.txt", "w");')
# Dump uses galaxy_map (which is replaced earlier) and enemies_count AND enemy_pool
text = text.replace("enemies_count", "state->enemies_count")
text = text.replace("enemy_pool", "state->enemy_pool")

# 7. Add attempt guard for empty check
text = re.sub(r"for \(int i = 0; i < e_count; i\+\+\) \{\n                    int sx, sy;\n                    do \{\n                        sx = Galaxy_Rand\(state\) % SECTOR_GRID_SIZE;\n                        sy = Galaxy_Rand\(state\) % SECTOR_GRID_SIZE;\n                    \} while \(q->sectors\[sy\]\[sx\]\.entity_type != ENTITY_EMPTY\);", r"for (int i = 0; i < e_count; i++) {\n                    int sx, sy;\n                    int attempts = 0;\n                    do {\n                        sx = Galaxy_Rand(state) % SECTOR_GRID_SIZE;\n                        sy = Galaxy_Rand(state) % SECTOR_GRID_SIZE;\n                        attempts++;\n                    } while (q->sectors[sy][sx].entity_type != ENTITY_EMPTY && attempts < 100);\n                    if (attempts >= 100) continue;\n", text)


with open("src/game/galaxy.c", "w") as f:
    f.write(text)

