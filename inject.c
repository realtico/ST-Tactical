int log_count = 0;
char logs[5][100];

void Dashboard_AddLog(const char* format, ...) {
    va_list args;
    __builtin_va_start(args, format);
    for (int i = 0; i < 4; i++) {
        strncpy(logs[i], logs[i + 1], sizeof(logs[i]) - 1);
    }
    vsnprintf(logs[4], sizeof(logs[4]), format, args);
    __builtin_va_end(args);
    log_count++;
}

void Dashboard_DrawPrompt(const char* input_buffer, int cursor_blink) {
    DrawTextColor(1, 20, "COMMAND? >", C_WHITE, C_BLACK, 0);
    DrawTextColor(12, 20, input_buffer, C_CYAN, C_BLACK, 0);
    
    // Draw cursor
    if (cursor_blink) {
        int len = strlen(input_buffer);
        Terminal_SetCell(12 + len, 20, '_', C_CYAN, C_BLACK, 0);
    }
}

void Dashboard_DrawGalaxyMap(GalaxyState* galaxy, const Enterprise* ent) {
    int sX = 16;
    int sY = 2;
    int eX = 51;
    int eY = 18;

    // Limpar painel central
    for (int y = sY; y <= eY; y++) {
        for (int x = sX; x <= eX; x++) {
            Terminal_SetCell(x, y, ' ', C_BLACK, C_BLACK, 0);
        }
    }

    if (!galaxy) return;

    DrawTextColor(21, 2, "      GALAXY MAP      ", C_WHITE, C_BLACK, 0);

    int mapX = 18;
    int mapY = 4;

    // Header colunas
    for (int x = 0; x < 8; x++) {
        char b[5]; snprintf(b, sizeof(b), "%d", x + 1);
        DrawTextColor(mapX + 2 + x * 4, mapY - 1, b, C_WHITE, C_BLACK, 0);
    }

    for (int y = 0; y < 8; y++) {
        // Linha divisória horizontal
        for (int x = 0; x < 8; x++) {
            DrawTextColor(mapX + x * 4, mapY + y * 2, "+---", C_DGRAY, C_BLACK, 0);
        }
        DrawTextColor(mapX + 32, mapY + y * 2, "+", C_DGRAY, C_BLACK, 0);

        // Header linhas
        char rBuf[5]; snprintf(rBuf, sizeof(rBuf), "%d", y + 1);
        DrawTextColor(mapX - 2, mapY + 1 + y * 2, rBuf, C_WHITE, C_BLACK, 0);
        
        for (int x = 0; x < 8; x++) {
            // Linha divisória vertical
            Terminal_SetCell(mapX + x * 4, mapY + 1 + y * 2, '|', C_DGRAY, C_BLACK, 0);
            
            Quadrant* q = Galaxy_GetQuadrant(galaxy, x, y);
            if (q) {
                // E = Inimigos, B = Base, S = Estrelas
                // Ex: "318" se Q=3, B=1, S=8
                char qData[5];
                snprintf(qData, sizeof(qData), "%d%d%d", q->num_enemies, q->has_base ? 1 : 0, q->num_stars);

                // Highlight superquadrante onde estamos
                uint8_t fg = C_GREEN;
                if ((ent->quadX - 1) == x && (ent->quadY - 1) == y) {
                    fg = C_CYAN;
                    Terminal_SetCell(mapX + 1 + x * 4, mapY + 1 + y * 2, '<', C_CYAN, C_BLACK, 0);
                    DrawTextColor(mapX + 2 + x * 4, mapY + 1 + y * 2, "E", C_WHITE, C_BLACK, 0);
                    Terminal_SetCell(mapX + 3 + x * 4, mapY + 1 + y * 2, '>', C_CYAN, C_BLACK, 0);
                } else if (q->num_enemies == 0 && q->has_base == 0 && q->num_stars == 0) {
                    DrawTextColor(mapX + 1 + x * 4, mapY + 1 + y * 2, "   ", C_DGRAY, C_BLACK, 0); // Vazio
                } else {
                    if (q->num_enemies > 0) fg = C_RED;
                    DrawTextColor(mapX + 1 + x * 4, mapY + 1 + y * 2, qData, fg, C_BLACK, 0);
                }
            } else {
                DrawTextColor(mapX + 1 + x * 4, mapY + 1 + y * 2, "???", C_DGRAY, C_BLACK, 0);
            }
        }
        Terminal_SetCell(mapX + 32, mapY + 1 + y * 2, '|', C_DGRAY, C_BLACK, 0);
    }
    // Bottom border map
    for (int x = 0; x < 8; x++) {
        DrawTextColor(mapX + x * 4, mapY + 16, "+---", C_DGRAY, C_BLACK, 0);
    }
    DrawTextColor(mapX + 32, mapY + 16, "+", C_DGRAY, C_BLACK, 0);
}

void Dashboard_DrawLogs(void) {
    // 5 logs no total, vamos desenhar os últimos 4 (indices 1,2,3,4)
    for (int i = 0; i < 4; i++) {
        // o mais recente é o índice 4 (i = 3 da tela, então y = 24)
        uint8_t c = (i == 3) ? C_WHITE : C_DGRAY;
        DrawTextColor(1, 21 + i, logs[1 + i], c, C_BLACK, 0);
    }
}
