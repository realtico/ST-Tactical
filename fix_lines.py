import re

with open("src/game/dashboard.c", "r") as f:
    text = f.read()

new_map = """void Dashboard_DrawGalaxyMap(GalaxyState* galaxy, const Enterprise* ent) {
    int sX = 15; 
    int sY = 2;
    DrawTextColor(33, 0, "GALACTIC CHART", C_WHITE, C_BLACK, 0);

    // Headers
    for(int x = 0; x < 12; x++) {
        char buf[10];
        snprintf(buf, sizeof(buf), " %02d ", x);
        DrawTextColor(sX + 1 + x*4, sY, buf, C_WHITE, C_BLACK, 0);
    }
    
    // Top border
    Terminal_SetCell(sX, sY+1, B_TL, C_DGRAY, C_BLACK, 0);
    for(int i=1; i<48; i++) {
        Terminal_SetCell(sX+i, sY+1, B_H, C_DGRAY, C_BLACK, 0);
    }
    Terminal_SetCell(sX+48, sY+1, B_TR, C_DGRAY, C_BLACK, 0);

    // Overlay Top Junctions
    for (int x=2; x<11; x+=3) {
        Terminal_SetCell(sX + 4 + x*4, sY+1, B_HD, C_DGRAY, C_BLACK, 0);
    }

    for (int y = 0; y < 12; y++) {
        int rY = sY + 2 + y + (y/3);
        char rBuf[5];
        snprintf(rBuf, sizeof(rBuf), "%02d", y);
        DrawTextColor(sX-3, rY, rBuf, C_WHITE, C_BLACK, 0);
        
        Terminal_SetCell(sX, rY, B_V, C_DGRAY, C_BLACK, 0);
        for (int x = 0; x < 12; x++) {
            
            double age = ent->stardate - galaxy->last_scan_date[y][x];
            Quadrant* q = Galaxy_GetQuadrant(galaxy, x, y);
            
            char data[5] = "...";
            uint8_t color = C_DGRAY;
            
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
                    color = C_BROWN;
                }
            }
            
            if (x == ent->quadX && y == ent->quadY) {
                // Highlight current quad
                Terminal_SetCell(sX+1 + x*4, rY, data[0], C_BLACK, color, 0);
                Terminal_SetCell(sX+2 + x*4, rY, data[1], C_BLACK, color, 0);
                Terminal_SetCell(sX+3 + x*4, rY, data[2], C_BLACK, color, 0);
            } else {
                DrawTextColor(sX+1 + x*4, rY, data, color, C_BLACK, 0);
            }
            
            if (x % 3 == 2 && x != 11) {
                Terminal_SetCell(sX+4 + x*4, rY, B_V, C_DGRAY, C_BLACK, 0);
            }
        }
        Terminal_SetCell(sX+48, rY, B_V, C_DGRAY, C_BLACK, 0);
        
        // Mid separators
        if (y % 3 == 2 && y != 11) {
            int sepY = rY + 1;
            Terminal_SetCell(sX, sepY, B_VR, C_DGRAY, C_BLACK, 0);
            for(int i=1; i<48; i++) {
                Terminal_SetCell(sX+i, sepY, B_H, C_DGRAY, C_BLACK, 0);
            }
            Terminal_SetCell(sX+48, sepY, B_VL, C_DGRAY, C_BLACK, 0);
            
            for (int x=2; x<11; x+=3) {
                Terminal_SetCell(sX + 4 + x*4, sepY, B_X, C_DGRAY, C_BLACK, 0);
            }
        }
    }
    
    // Bottom border
    int bY = sY + 17;
    Terminal_SetCell(sX, bY, B_BL, C_DGRAY, C_BLACK, 0);
    for(int i=1; i<48; i++) {
        Terminal_SetCell(sX+i, bY, B_H, C_DGRAY, C_BLACK, 0);
    }
    Terminal_SetCell(sX+48, bY, B_BR, C_DGRAY, C_BLACK, 0);
    
    for (int x=2; x<11; x+=3) {
        Terminal_SetCell(sX + 4 + x*4, bY, B_HU, C_DGRAY, C_BLACK, 0);
    }
}
"""

match = re.search(r"void Dashboard_DrawGalaxyMap\(.*?\n}", text, flags=re.MULTILINE|re.DOTALL)
if match:
    text = text[:match.start()] + new_map + text[match.end():]
    with open("src/game/dashboard.c", "w") as f:
        f.write(text)
    print("Substituição aplicada.")
else:
    print("Função não encontrada!")

