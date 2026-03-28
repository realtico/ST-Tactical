import re

with open("src/game/dashboard.c", "r") as f:
    dash = f.read()

# Fix logs
dash = dash.replace("int logY = 20;\n    int display_count = 4;", "int logY = 20;\n    int display_count = 5;")
dash = dash.replace("Terminal_SetCell(0, 24, B_V, C_DGRAY, C_BLACK, 0);", "")
dash = dash.replace('DrawTextColor(2, 24, "COMMAND? > ", C_CYAN, C_BLACK, 0);', 'DrawTextColor(2, 19, "COMMAND? > ", C_CYAN, C_BLACK, 0);')
dash = dash.replace('DrawTextColor(13, 24, input_buffer, C_WHITE, C_BLACK, 0);', 'DrawTextColor(13, 19, input_buffer, C_WHITE, C_BLACK, 0);')
dash = dash.replace('Terminal_SetCell(13 + len, 24,', 'Terminal_SetCell(13 + len, 19,')

# Find exactly where DrawGalaxyMap ends
start_idx = dash.find("void Dashboard_DrawGalaxyMap")
if start_idx != -1:
    end_idx = dash.find("}\n", dash.rfind("Terminal_SetCell(sX+51, bY, B_BR")) + 2
    dash = dash[:start_idx] + """
void Dashboard_DrawGalaxyMap(GalaxyState* galaxy, const Enterprise* ent) {
    int sX = 16; 
    int sY = 2;
    
    // Clear center panel background (X=16 to 51, Y=1 to 18)
    for(int y=1; y<=18; y++) {
        for(int x=16; x<=51; x++) {
            Terminal_SetCell(x, y, ' ', C_BLACK, C_BLACK, 0);
        }
    }
    
    DrawTextColor(20, 1, "      GALACTIC CHART      ", C_WHITE, C_BLACK, 0);

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
""" + dash[end_idx:]

with open("src/game/dashboard.c", "w") as f:
    f.write(dash)

