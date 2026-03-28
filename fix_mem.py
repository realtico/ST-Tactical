import re

with open("include/game/galaxy.h", "r") as f:
    text = f.read()

struct_insert = """    uint8_t enemies_count;

    // Memória de Sensores
    double last_scan_date[12][12];
    uint8_t last_known_klingons[12][12];
"""

text = text.replace("uint8_t enemies_count;", struct_insert)

if "Galaxy_UpdateSensors" not in text:
    text = text.replace("void Galaxy_DumpToLog(GalaxyState* state, const char* filepath);", "void Galaxy_DumpToLog(GalaxyState* state, const char* filepath);\nvoid Galaxy_UpdateSensors(GalaxyState* state, int qX, int qY, double stardate, bool isLRS);")

with open("include/game/galaxy.h", "w") as f:
    f.write(text)

with open("src/game/galaxy.c", "r") as f:
    text = f.read()

generate_insert = """    state->enemies_count = 0;

    for (int y = 0; y < 12; y++) {
        for (int x = 0; x < 12; x++) {
            state->last_scan_date[y][x] = -1.0;
            state->last_known_klingons[y][x] = 0;
        }
    }
"""

text = text.replace("state->enemies_count = 0;", generate_insert)

func_impl = """
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
"""

if "Galaxy_UpdateSensors" not in text:
    text += func_impl

with open("src/game/galaxy.c", "w") as f:
    f.write(text)

