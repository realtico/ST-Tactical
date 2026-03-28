void Dashboard_DrawGalaxyMap(GalaxyState* galaxy, const Enterprise* ent) {
    if (!galaxy) return;

    // Centered map. X=15 leaves exactly 15 offset with width ~50 (actually 49).
    int sX = 14; 
    int sY = 2;

    DrawTextColor(sX + 18, 0, "GALACTIC CHART", C_WHITE, C_BLACK, 0);

    // X headers
    for (int gx = 0; gx < 12; gx++) {
        char buf[8];
        snprintf(buf, sizeof(buf), "%02d", gx);
        // Each block of 3 has 12 chars. "00" takes 2 chars.
        // gx=0 -> sX + 1(border) + 0*12 + 0*4 = sX + 1. 
        // We want it centered over the 3 chars, so add 1.
        int block = gx / 3;
        int sub = gx % 3;
        int px = sX + 1 + block * 12 + sub * 4 + 1;
        DrawTextColor(px, 1, buf, C_WHITE, C_BLACK, 0);
    }

    int totalY = sY + 16;
    for (int y = sY; y <= totalY; y++) {
        int cy = y - sY;
        if (cy % 4 == 0) {
            // It's a border line
            bool isTop = (cy == 0);
            bool isBot = (cy == 16);
            Terminal_SetCell(sX, y, isTop ? B_TL : (isBot ? B_BL : B_VR), C_DGRAY, C_BLACK, 0);
            for (int b = 0; b < 4; b++) {
                for (int i = 0; i < 11; i++) Terminal_SetCell(sX + 1 + b*12 + i, y, B_H, C_DGRAY, C_BLACK, 0);
                if (b < 3) {
                    Terminal_SetCell(sX + 12 + b*12, y, isTop ? B_HD : (isBot ? B_HU : B_X), C_DGRAY, C_BLACK, 0);
                }
            }
            Terminal_SetCell(sX + 48, y, isTop ? B_TR : (isBot ? B_BR : B_VL), C_DGRAY, C_BLACK, 0);
        } else {
            // It's a data line
            int gy = (cy / 4) * 3 + (cy % 4) - 1;
            char rBuf[5];
            snprintf(rBuf, sizeof(rBuf), "%02d", gy);
            DrawTextColor(sX - 3, y, rBuf, C_WHITE, C_BLACK, 0);

            for (int b = 0; b < 4; b++) {
                Terminal_SetCell(sX + b*12, y, B_V, C_DGRAY, C_BLACK, 0);
                for (int sub = 0; sub < 3; sub++) {
                    int gx = b * 3 + sub;
                    int px = sX + 1 + b*12 + sub*4;
                    
                    Quadrant* q = Galaxy_GetQuadrant(galaxy, gx, gy);
                    bool scanned = galaxy->last_scan_date[gy][gx] > 0.0;
                    char qData[5];
                    
                    if (q) {
                        snprintf(qData, sizeof(qData), "%d%d%d", q->num_enemies, q->has_base ? 1 : 0, q->num_stars);
                    } else {
                        snprintf(qData, sizeof(qData), "???");
                    }
                    
                    if ((ent->quadX - 1) == gx && (ent->quadY - 1) == gy) {
                        if (!scanned) {
                            DrawTextColor(px, y, "...", C_BLACK, C_CYAN, 0);
                        } else {
                            DrawTextColor(px, y, qData, C_BLACK, C_CYAN, 0);
                        }
                    } else {
                        if (!scanned) {
                            DrawTextColor(px, y, "...", C_DGRAY, C_BLACK, 0);
                        } else {
                            uint8_t fg = C_GREEN;
                            if (q && q->num_enemies > 0) fg = C_RED;
                            DrawTextColor(px, y, qData, fg, C_BLACK, 0);
                        }
                    }
                }
            }
            Terminal_SetCell(sX + 48, y, B_V, C_DGRAY, C_BLACK, 0);
        }
    }
}
