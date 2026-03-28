import re

with open("src/video/test_video.c", "r") as f:
    test_video = f.read()

# Fix QUT confirmation
test_video = test_video.replace('shouldQuit = true;\n}', 'Dashboard_AddLog("Deseja desligar? (Digite Y ou N)");\n}')
test_video = test_video.replace('static bool shouldQuit = false;', 'static bool shouldQuit = false;\nstatic bool waitingQuit = false;')

map_qut_logic = """
void Cmd_QUT(int argc, char** argv) {
    Dashboard_AddLog("Deseja desligar? (Digite Y ou N)");
    waitingQuit = true;
}
"""
test_video = re.sub(r'void Cmd_QUT\(int argc, char\*\* argv\) \{.*?\}', map_qut_logic, test_video, flags=re.DOTALL)

process_cmd = """
void Shell_ProcessCommand(const char* input) {
    if (waitingQuit) {
        if (input[0] == 'Y' || input[0] == 'y') {
            shouldQuit = true;
        } else {
            waitingQuit = false;
            Dashboard_AddLog("Desligamento abortado.");
        }
        return;
    }
"""

test_video = test_video.replace("void Shell_ProcessCommand(const char* input) {", process_cmd)

# Set Exit Key properly
test_video = test_video.replace("SetExitKey(0); // Disable ESC to exit", "SetExitKey(0); // Disable ESC")

with open("src/video/test_video.c", "w") as f:
    f.write(test_video)


with open("src/game/dashboard.c", "r") as f:
    dash = f.read()

new_map = """
void Dashboard_DrawGalaxyMap(GalaxyState* galaxy, const Enterprise* ent) {
    int sX = 16; 
    int sY = 2;
    
    // Clear center panel background (X=16 to 51, Y=1 to 18)
    for(int y=1; y<=18; y++) {
        for(int x=16; x<=51; x++) {
            Terminal_SetCell(x, y, ' ', C_BLACK, C_BLACK, 0);
        }
    }
    
    DrawTextColor(20, 1, " GALACTIC CHART ", C_WHITE, C_BLACK, 0);

    // Headers
    char colHeader[40] = "";
    for(int x = 0; x < 12; x++) {
        char buf[5];
        snprintf(buf, sizeof(buf), "%02d ", x);
        strcat(colHeader, buf);
    }
    DrawTextColor(sX + 1, sY, colHeader, C_WHITE, C_BLACK, 0);
    
    for (int y = 0; y < 12; y++) {
        for (int x = 0; x < 12; x++) {
            double age = ent->stardate - galaxy->last_scan_date[y][x];
            Quadrant* q = Galaxy_GetQuadrant(galaxy, x, y);
            
            char data[4] = " . ";
            uint8_t color = C_DGRAY;
            uint8_t bg = C_BLACK;
            
            if (galaxy->last_scan_date[y][x] >= 0) {
                if (q && q->has_base) {
                    snprintf(data, sizeof(data), "%d%d%d", galaxy->last_known_klingons[y][x], 1, q->num_stars);
                    color = C_CYAN;
                } else if (age < 5.0) {
                    snprintf(data, sizeof(data), "%d%d%d", galaxy->last_known_klingons[y][x], 0, q ? q->num_stars : 0);
                    color = C_GREEN;
                } else if (age <= 15.0) {
                    snprintf(data, sizeof(data), "%d%d%d", galaxy->last_known_klingons[y][x], 0, q ? q->num_stars : 0);
                    color = C_YELLOW;
                } else {
                    snprintf(data, sizeof(data), "%d%d%d", galaxy->last_known_klingons[y][x], 0, q ? q->num_stars : 0);
                    color = C_RED; // Brown is missing, use RED or DGRAY
                }
            }
            
            if (x == ent->quadX && y == ent->quadY) {
                bg = C_DGRAY;
                if(data[1] == '.') strcpy(data, " E ");
            }
            
            Terminal_SetCell(sX + 1 + x*3, sY + 2 + y, data[0], color, bg, 0);
            Terminal_SetCell(sX + 2 + x*3, sY + 2 + y, data[1], color, bg, 0);
            Terminal_SetCell(sX + 3 + x*3, sY + 2 + y, data[2], color, bg, 0);
        }
    }
}
"""

dash = re.sub(r'void Dashboard_DrawGalaxyMap\(GalaxyState\* galaxy, const Enterprise\* ent\) \{.*?\}\n', new_map, dash, flags=re.DOTALL)

with open("src/game/dashboard.c", "w") as f:
    f.write(dash)

