import re

with open("include/game/galaxy.h", "r") as f:
    text = f.read()

struct_def = """// Estado completo da Galáxia instanciável
typedef struct {
    uint32_t seed_state;
    uint16_t enemies_count;
    KlingonGroup enemy_pool[MAX_ENEMIES];
    Quadrant galaxy_map[QUADRANT_GRID_SIZE][QUADRANT_GRID_SIZE];
} GalaxyState;

/* Inicializa a Galáxia a partir de uma seed */
void Galaxy_Generate(GalaxyState* state, uint32_t seed);

/* Recupera acesso de leitura para a Galáxia e o Pool de inimigos para renderização, etc */
Quadrant* Galaxy_GetQuadrant(GalaxyState* state, int qx, int qy);
KlingonGroup* Galaxy_GetEnemy(GalaxyState* state, uint16_t id);

/* Salva um log ASCII em arquivo */
void Galaxy_DumpToLog(GalaxyState* state, const char* filepath);"""

# Replace old functions
old_funcs_start = "/* Inicializa a Galáxia a partir de uma seed */"
text = text[:text.find(old_funcs_start)] + struct_def + "\n\n#endif // GALAXY_H\n"

with open("include/game/galaxy.h", "w") as f:
    f.write(text)

