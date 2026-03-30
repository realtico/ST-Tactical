#include "game/dashboard.h"
#include "video/terminal.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

// Cores (padrão terminal de 1 a 15, em tese, a paleta é customizável, mas usaremos 
// 15 = braco, 0 = preto, 10 = verde, 12 = vermelho, 14 = amarelo, 8 = cinza escuro)
#define C_WHITE  15
#define C_BLACK  0
#define C_GREEN  10
#define C_RED    9    // 9 é Bright Red no terminal
#define C_YELLOW 11   // 11 é Bright Yellow
#define C_DGRAY  8

#define C_CYAN   14   // 14 é Bright Cyan

// UI Anchors
#define NAV_PANEL_X 1
#define NAV_PANEL_Y 1
#define CENTER_PANEL_X 18
#define CENTER_PANEL_Y 1
#define CENTER_PANEL_W 33
#define CENTER_PANEL_H 18
#define NAV_PANEL_W 24
#define NAV_PANEL_H 18
#define LOG_PANEL_X 1
#define LOG_PANEL_Y 20
#define LOG_PANEL_W 59
#define LOG_PANEL_H 4
#define RES_PANEL_X 61
#define RES_PANEL_Y 1

#define B_V_DBL  0x2551
#define B_H_DBL  0x2550
#define B_X_DBL  0x256C
#define B_HD_DBL 0x2566
#define B_HU_DBL 0x2569
#define B_VR_DBL 0x2560
#define B_VL_DBL 0x2563
#define B_X_VD_HS 0x256B
#define B_X_HD_VS 0x256A


// Remapeando para Box Drawing Unicode de verdade agora que usamos 32-bit codepoints
// Esses caracteres já estão cobertos pela JetBrainsMono nos bytes 0x2500+
#define B_V   0x2502 
#define B_H   0x2500 
#define B_TL  0x250C 
#define B_TR  0x2510 
#define B_BL  0x2514 
#define B_BR  0x2518 
#define B_VR  0x251C 
#define B_VL  0x2524 
#define B_HD  0x252C 
#define B_HU  0x2534 
#define B_X   0x253C 
#define BLOCK_FULL 0x2588 
#define BLOCK_LITE 0x2591
#define MIDDLE_DOT 0x00B7

// Função auxiliar para desenhar box

// Auxiliar para texto
static void DrawTextColor(int x, int y, const char* text, uint8_t fg, uint8_t bg, uint8_t attr) {
    for (int i = 0; text[i] != '\0'; i++) {
        Terminal_SetCell(x + i, y, (unsigned char)text[i], fg, bg, attr);
    }
}

// Desenha 3 middle dots (U+00B7) — usado para quadrantes não-escaneados no mapa
static void DrawMiddleDots(int x, int y, uint8_t fg, uint8_t bg) {
    for (int i = 0; i < 3; i++)
        Terminal_SetCell(x + i, y, MIDDLE_DOT, fg, bg, 0);
}

// Barras de Progresso
static void DrawProgressBar(int x, int y, int current, int max, int width, uint8_t fg_full, uint8_t fg_empty, uint8_t bg) {
    if (max <= 0) return;
    int filled = (current * width) / max;
    if (filled > width) filled = width;
    if (filled < 0) filled = 0;
    
    for (int i = 0; i < width; i++) {
        int c = (i < filled) ? BLOCK_FULL : BLOCK_LITE;
        uint8_t fg = (i < filled) ? fg_full : fg_empty;
        Terminal_SetCell(x + i, y, c, fg, bg, 0);
    }
}

void Dashboard_Init(void) {
    // Para futura iniciação caso precise carregar paletas ou texturas de UI.
}

void Dashboard_DrawFrame(void) {
    // Colunas do Template do PO:
    // Esq: 0 a 15 (W=16)
    // Centro: 16 a 52 (W=37)
    // Dir: 53 a 79 (W=27)
    
    // Top Row
    Terminal_SetCell(0, 0, B_TL, C_DGRAY, C_BLACK, 0);
    for(int x=1; x<=14; x++) Terminal_SetCell(x, 0, B_H, C_DGRAY, C_BLACK, 0);
    Terminal_SetCell(15, 0, B_HD, C_DGRAY, C_BLACK, 0); // ┬
    for(int x=16; x<=51; x++) Terminal_SetCell(x, 0, B_H, C_DGRAY, C_BLACK, 0);
    Terminal_SetCell(52, 0, B_HD, C_DGRAY, C_BLACK, 0); // ┬
    for(int x=53; x<=78; x++) Terminal_SetCell(x, 0, B_H, C_DGRAY, C_BLACK, 0);
    Terminal_SetCell(79, 0, B_TR, C_DGRAY, C_BLACK, 0); // ┐
    
    // Header Titles
    DrawTextColor(1, 1, "  NAVIGATION  ", C_WHITE, C_BLACK, 0);
    DrawTextColor(53, 1, "         RESOURCES        ", C_WHITE, C_BLACK, 0);
    Terminal_SetCell(0, 1, B_V, C_DGRAY, C_BLACK, 0);
    Terminal_SetCell(15, 1, B_V, C_DGRAY, C_BLACK, 0);
    Terminal_SetCell(52, 1, B_V, C_DGRAY, C_BLACK, 0);
    Terminal_SetCell(79, 1, B_V, C_DGRAY, C_BLACK, 0);

    // Separators
    Terminal_SetCell(0, 2, B_VR, C_DGRAY, C_BLACK, 0);
    for(int x=1; x<=14; x++) Terminal_SetCell(x, 2, B_H, C_DGRAY, C_BLACK, 0);
    Terminal_SetCell(15, 2, B_VL, C_DGRAY, C_BLACK, 0);
    
    Terminal_SetCell(52, 2, B_VR, C_DGRAY, C_BLACK, 0);
    for(int x=53; x<=78; x++) Terminal_SetCell(x, 2, B_H, C_DGRAY, C_BLACK, 0);
    Terminal_SetCell(79, 2, B_VL, C_DGRAY, C_BLACK, 0);

    // Fill vertical borders for the rest of UI
    for (int y = 3; y < 19; y++) {
        Terminal_SetCell(0, y, B_V, C_DGRAY, C_BLACK, 0);
        Terminal_SetCell(15, y, B_V, C_DGRAY, C_BLACK, 0);
        Terminal_SetCell(52, y, B_V, C_DGRAY, C_BLACK, 0);
        Terminal_SetCell(79, y, B_V, C_DGRAY, C_BLACK, 0);
    }
    
    // Bottom border command prompt separator
    Terminal_SetCell(0, 19, B_BL, C_DGRAY, C_BLACK, 0);
    for(int x=1; x<=14; x++) Terminal_SetCell(x, 19, B_H, C_DGRAY, C_BLACK, 0);
    Terminal_SetCell(15, 19, B_HU, C_DGRAY, C_BLACK, 0);
    for(int x=16; x<=51; x++) Terminal_SetCell(x, 19, B_H, C_DGRAY, C_BLACK, 0);
    
    // Resources separator / Logs
    Terminal_SetCell(52, 12, B_VR, C_DGRAY, C_BLACK, 0);
    for(int x=53; x<=78; x++) Terminal_SetCell(x, 12, B_H, C_DGRAY, C_BLACK, 0);
    Terminal_SetCell(79, 12, B_VL, C_DGRAY, C_BLACK, 0);
    
    DrawTextColor(53, 13, "       ENEMY STATUS       ", C_WHITE, C_BLACK, 0);

    Terminal_SetCell(52, 19, B_HU, C_DGRAY, C_BLACK, 0);
    for(int x=53; x<=78; x++) Terminal_SetCell(x, 19, B_H, C_DGRAY, C_BLACK, 0);
    Terminal_SetCell(79, 19, B_BR, C_DGRAY, C_BLACK, 0); 
    
    // Nav Separator 
    Terminal_SetCell(0, 12, B_VR, C_DGRAY, C_BLACK, 0);
    for(int x=1; x<=14; x++) Terminal_SetCell(x, 12, B_H, C_DGRAY, C_BLACK, 0);
    Terminal_SetCell(15, 12, B_VL, C_DGRAY, C_BLACK, 0);
    DrawTextColor(1, 13, " DAMAGE REPT  ", C_WHITE, C_BLACK, 0);
    
    // Command Prompt Area
    
}

void Dashboard_DrawEnterpriseStats(const Enterprise* ent) {
    char buf[64];
    
    // Naigation Panels
    DrawTextColor(2, 3, "STARDATE:", C_WHITE, C_BLACK, 0);
    snprintf(buf, sizeof(buf), "%.1f", ent->stardate);
    DrawTextColor(3, 4, buf, C_CYAN, C_BLACK, 0);
    
    DrawTextColor(2, 6, "QUADRANT:", C_WHITE, C_BLACK, 0);
    snprintf(buf, sizeof(buf), "%02d, %02d", ent->quadX, ent->quadY);
    DrawTextColor(3, 7, buf, C_CYAN, C_BLACK, 0);
    
    DrawTextColor(2, 9, "SECTOR:", C_WHITE, C_BLACK, 0);
    snprintf(buf, sizeof(buf), "%02d, %02d", ent->sectX, ent->sectY);
    DrawTextColor(3, 10, buf, C_CYAN, C_BLACK, 0);
    
    // Damage Rept
    snprintf(buf, sizeof(buf), "WARP:   [%s]", (ent->sysWarp > 0.5f) ? "OK" : "!! ");
    DrawTextColor(2, 14, buf, (ent->sysWarp > 0.5f) ? C_GREEN : C_RED, C_BLACK, 0);
    
    snprintf(buf, sizeof(buf), "PHASER: [%s]", (ent->sysPhaser > 0.5f) ? "OK" : "!! ");
    DrawTextColor(2, 15, buf, (ent->sysPhaser > 0.5f) ? C_GREEN : C_RED, C_BLACK, 0);
    
    snprintf(buf, sizeof(buf), "SNSR:   [%s]", (ent->sysSensor > 0.5f) ? "OK" : "!! ");
    DrawTextColor(2, 16, buf, (ent->sysSensor > 0.5f) ? C_GREEN : C_RED, C_BLACK, 0);
    
    snprintf(buf, sizeof(buf), "LRS:    [%s]", (ent->sysLRS > 0.5f) ? "OK" : "!! ");
    DrawTextColor(2, 17, buf, (ent->sysLRS > 0.5f) ? C_GREEN : C_RED, C_BLACK, 0);

    snprintf(buf, sizeof(buf), "COMP:   [%s]", (ent->sysComputer > 0.5f) ? "OK" : "!! ");
    DrawTextColor(2, 18, buf, (ent->sysComputer > 0.5f) ? C_GREEN : C_RED, C_BLACK, 0);

    // Resources Left Panel
    uint8_t condColor = C_GREEN;
    if (ent->condition == COND_RED) condColor = C_RED;
    if (ent->condition == COND_YELLOW) condColor = C_YELLOW;
    
    const char* condStr = (ent->condition == COND_RED) ? "RED   " : (ent->condition == COND_YELLOW) ? "YELLOW" : "GREEN ";
    snprintf(buf, sizeof(buf), "CONDITION: %s", condStr);
    DrawTextColor(54, 3, buf, condColor, C_BLACK, 0);
    
    DrawTextColor(54, 4, "ENERGY:   ", C_WHITE, C_BLACK, 0);
    DrawProgressBar(65, 4, ent->energy, ent->energyMax, 10, C_YELLOW, C_DGRAY, C_BLACK);
    snprintf(buf, sizeof(buf), "(%d/%d)", ent->energy, ent->energyMax);
    DrawTextColor(56, 5, buf, C_WHITE, C_BLACK, 0);

    DrawTextColor(54, 7, "SHIELDS:  ", C_WHITE, C_BLACK, 0);
    DrawProgressBar(65, 7, ent->shields, ent->shieldsMax, 10, C_CYAN, C_DGRAY, C_BLACK);
    snprintf(buf, sizeof(buf), "(%d%%)", ent->shieldsMax > 0 ? (int)(((float)ent->shields/(float)ent->shieldsMax)*100) : 0);
    DrawTextColor(56, 8, buf, C_WHITE, C_BLACK, 0);
    
    DrawTextColor(54, 10, "TORPEDOES:", C_WHITE, C_BLACK, 0);
    DrawProgressBar(65, 10, ent->torpedoes, ent->torpedoesMax, 10, C_RED, C_DGRAY, C_BLACK);
    snprintf(buf, sizeof(buf), "(%d/%d)", ent->torpedoes, ent->torpedoesMax);
    DrawTextColor(56, 11, buf, C_WHITE, C_BLACK, 0);
}

void Dashboard_DrawLRS(GalaxyState* galaxy, const Enterprise* ent) {
    int sX = 28;
    int sY = 4;
    DrawTextColor(16, 1, "       LONG RANGE SCAN  (LRS)       ", C_WHITE, C_BLACK, 0);
        
        
        // Header
        char colHeader[40] = "";
        for (int x = 0; x < 3; x++) {
            int checkQx = ent->quadX - 1 + x;
            if (checkQx >= 0 && checkQx < 12) {
                char buf[5]; snprintf(buf, sizeof(buf), "%02d  ", checkQx);
                strcat(colHeader, buf);
            } else {
                strcat(colHeader, "    ");
            }
        }
        DrawTextColor(sX+2, 3, colHeader, C_WHITE, C_BLACK, 0);
        
        // LRS Grid Top
        
        Terminal_SetCell(sX, sY, B_TL, C_DGRAY, C_BLACK, 0);
        for(int x=0; x<3;x++){
            Terminal_SetCell(sX+1 + x*4, sY, B_H, C_DGRAY, C_BLACK, 0);
            Terminal_SetCell(sX+2 + x*4, sY, B_H, C_DGRAY, C_BLACK, 0);
            Terminal_SetCell(sX+3 + x*4, sY, B_H, C_DGRAY, C_BLACK, 0);
            if (x < 2) Terminal_SetCell(sX+4 + x*4, sY, B_HD, C_DGRAY, C_BLACK, 0);
        }
        Terminal_SetCell(sX+12, sY, B_TR, C_DGRAY, C_BLACK, 0);
        
        // 3x3 Grid
        for (int y = 0; y < 3; y++) {
            int checkQy = ent->quadY - 1 + y;
            if (checkQy >= 0 && checkQy < 12) {
                char rBuf[5];
                snprintf(rBuf, sizeof(rBuf), "%02d", checkQy);
                DrawTextColor(sX-3, sY+1 + y*2, rBuf, C_WHITE, C_BLACK, 0);
            }
            
            for (int x = 0; x < 3; x++) {
                Terminal_SetCell(sX + x*4, sY+1 + y*2, B_V, C_DGRAY, C_BLACK, 0);
                
                // Get Galaxy Data Relative to Enterprise
                int checkQx = ent->quadX - 1 + x;
                int checkQy = ent->quadY - 1 + y;
                
                Quadrant* q = Galaxy_GetQuadrant(galaxy, checkQx, checkQy);
                if (q != NULL) {
                    char data[8];
                    snprintf(data, sizeof(data), "%d%d%d", q->num_enemies, q->has_base ? 1 : 0, q->num_stars);
                    DrawTextColor(sX+1 + x*4, sY+1 + y*2, data, C_GREEN, C_BLACK, 0);
                } else {
                    // Out of bounds: completely empty string, no numbers.
                    DrawTextColor(sX+1 + x*4, sY+1 + y*2, "   ", C_DGRAY, C_BLACK, 0);
                }
            }
            Terminal_SetCell(sX+12, sY+1 + y*2, B_V, C_DGRAY, C_BLACK, 0);
            
            // Mid Row
            if (y < 2) {
                Terminal_SetCell(sX, sY+2 + y*2, B_VR, C_DGRAY, C_BLACK, 0);
                for(int x=0; x<3;x++){
                    Terminal_SetCell(sX+1 + x*4, sY+2 + y*2, B_H, C_DGRAY, C_BLACK, 0);
                    Terminal_SetCell(sX+2 + x*4, sY+2 + y*2, B_H, C_DGRAY, C_BLACK, 0);
                    Terminal_SetCell(sX+3 + x*4, sY+2 + y*2, B_H, C_DGRAY, C_BLACK, 0);
                    if (x < 2) Terminal_SetCell(sX+4 + x*4, sY+2 + y*2, B_X, C_DGRAY, C_BLACK, 0);
                }
                Terminal_SetCell(sX+12, sY+2 + y*2, B_VL, C_DGRAY, C_BLACK, 0);
            }
        }
        
        // Bottom Row
        int bY = sY + 6;
        Terminal_SetCell(sX, bY, B_BL, C_DGRAY, C_BLACK, 0);
        for(int x=0; x<3;x++){
            Terminal_SetCell(sX+1 + x*4, bY, B_H, C_DGRAY, C_BLACK, 0);
            Terminal_SetCell(sX+2 + x*4, bY, B_H, C_DGRAY, C_BLACK, 0);
            Terminal_SetCell(sX+3 + x*4, bY, B_H, C_DGRAY, C_BLACK, 0);
            if (x < 2) Terminal_SetCell(sX+4 + x*4, bY, B_HU, C_DGRAY, C_BLACK, 0);
        }
        Terminal_SetCell(sX+12, bY, B_BR, C_DGRAY, C_BLACK, 0);
}

void Dashboard_DrawSRS(GalaxyState* galaxy, const Enterprise* ent) {
    int sX = 18;
    int sY = 2;
    // SRS Mode
    
        
        
        static int camX = -1;
        static int camY = -1;
        int entGX = ent->sectX;
        int entGY = ent->sectY;
        
        if (camX == -1 || entGX < camX || entGX >= camX + 8 || entGY < camY || entGY >= camY + 8) {
            camX = entGX - 3;
            camY = entGY - 3;
            if (camX < 0) camX = 0;
            if (camY < 0) camY = 0;
            if (camX > 96 - 8) camX = 96 - 8;
            if (camY > 96 - 8) camY = 96 - 8;
        }

        // Draw col header based on camX
        char colHeader[40] = "";
        for(int x=0; x<8; x++) {
            char buf[5]; snprintf(buf, sizeof(buf), " %02d ", camX+x);
            strcat(colHeader, buf);
        }
        DrawTextColor(sX+1, 1, colHeader, C_WHITE, C_BLACK, 0);

        Terminal_SetCell(sX, sY, B_TL, C_DGRAY, C_BLACK, 0);
        for(int i=0; i<8; i++) {
            Terminal_SetCell(sX+1 + i*4, sY, B_H, C_DGRAY, C_BLACK, 0);
            Terminal_SetCell(sX+2 + i*4, sY, B_H, C_DGRAY, C_BLACK, 0);
            Terminal_SetCell(sX+3 + i*4, sY, B_H, C_DGRAY, C_BLACK, 0);
            if (i < 7) {
                int hd = ((camX + i) % 8 == 7) ? B_HD_DBL : B_HD;
                Terminal_SetCell(sX+4 + i*4, sY, hd, C_DGRAY, C_BLACK, 0);
            }
        }
        Terminal_SetCell(sX+32, sY, B_TR, C_DGRAY, C_BLACK, 0);
        
        for (int y = 0; y < 8; y++) {
            char rBuf[5];
            snprintf(rBuf, sizeof(rBuf), "%02d", camY+y);
            DrawTextColor(sX-2, sY+1 + y*2, rBuf, C_WHITE, C_BLACK, 0);
            
            for (int x = 0; x < 8; x++) {
                int v_line = (x > 0 && (camX + x) % 8 == 0) ? B_V_DBL : B_V;
                Terminal_SetCell(sX + x*4, sY+1 + y*2, v_line, C_DGRAY, C_BLACK, 0);
                
                int gx = camX + x;
                int gy = camY + y;
                int qx = gx / 8;
                int qy = gy / 8;
                int sx = gx % 8;
                int sy = gy % 8;
                
                int charStart = sX + 1 + x*4;
                
                if (gx == entGX && gy == entGY) {
                    DrawTextColor(charStart, sY+1 + y*2, "<E>", C_CYAN, C_BLACK, 0);
                } else {
                    Quadrant* q = Galaxy_GetQuadrant(galaxy, qx, qy);
                    if (q) {
                        Sector* s = &q->sectors[sy][sx];
                        if (s->entity_type == ENTITY_STAR) {
                            DrawTextColor(charStart, sY+1 + y*2, " * ", C_YELLOW, C_BLACK, 0);
                        } else if (s->entity_type == ENTITY_BASE) {
                            DrawTextColor(charStart, sY+1 + y*2, ">!<", C_CYAN, C_BLACK, 0);
                        } else if (s->entity_type == ENTITY_ENEMY) {
                            KlingonGroup* e = Galaxy_GetEnemy(galaxy, s->entity_id);
                            if (e && e->active) {
                                if (e->class_id == 0) DrawTextColor(charStart, sY+1 + y*2, "+++", C_RED, C_BLACK, 0);
                                else if (e->class_id == 1) DrawTextColor(charStart, sY+1 + y*2, "+K+", C_RED, C_BLACK, 0);
                                else DrawTextColor(charStart, sY+1 + y*2, "+V+", C_RED, C_BLACK, 0);
                            } else {
                                DrawTextColor(charStart, sY+1 + y*2, "   ", C_WHITE, C_BLACK, 0);
                            }
                        } else {
                            DrawTextColor(charStart, sY+1 + y*2, "   ", C_WHITE, C_BLACK, 0);
                        }
                    } else {
                        DrawTextColor(charStart, sY+1 + y*2, "   ", C_WHITE, C_BLACK, 0);
                    }
                }
            }
            Terminal_SetCell(sX+32, sY+1 + y*2, B_V, C_DGRAY, C_BLACK, 0);
            
            if (y < 7) {
                int h_dbl = ((camY + y) % 8 == 7);
                Terminal_SetCell(sX, sY+2 + y*2, h_dbl ? B_VR_DBL : B_VR, C_DGRAY, C_BLACK, 0);
                for(int i=0; i<8; i++){
                    int h_char = h_dbl ? B_H_DBL : B_H;
                    Terminal_SetCell(sX+1 + i*4, sY+2 + y*2, h_char, C_DGRAY, C_BLACK, 0);
                    Terminal_SetCell(sX+2 + i*4, sY+2 + y*2, h_char, C_DGRAY, C_BLACK, 0);
                    Terminal_SetCell(sX+3 + i*4, sY+2 + y*2, h_char, C_DGRAY, C_BLACK, 0);
                    if (i < 7) {
                        int v_dbl = ((camX + i) % 8 == 7);
                        int cross = B_X;
                        if (h_dbl && v_dbl) cross = B_X_DBL;
                        else if (h_dbl) cross = B_X_HD_VS;
                        else if (v_dbl) cross = B_X_VD_HS;
                        Terminal_SetCell(sX+4 + i*4, sY+2 + y*2, cross, C_DGRAY, C_BLACK, 0);
                    }
                }
                Terminal_SetCell(sX+32, sY+2 + y*2, h_dbl ? B_VL_DBL : B_VL, C_DGRAY, C_BLACK, 0);
            }
        }
        
        int bY = sY + 16;  // y takes 2 lines, so 8*2 = 16
        Terminal_SetCell(sX, bY, B_BL, C_DGRAY, C_BLACK, 0);
        for(int i=0; i<8; i++){
            Terminal_SetCell(sX+1 + i*4, bY, B_H, C_DGRAY, C_BLACK, 0);
            Terminal_SetCell(sX+2 + i*4, bY, B_H, C_DGRAY, C_BLACK, 0);
            Terminal_SetCell(sX+3 + i*4, bY, B_H, C_DGRAY, C_BLACK, 0);
            if (i < 7) {
                int hu = ((camX + i) % 8 == 7) ? B_HU_DBL : B_HU;
                Terminal_SetCell(sX+4 + i*4, bY, hu, C_DGRAY, C_BLACK, 0);
            }
        }
        Terminal_SetCell(sX+32, bY, B_BR, C_DGRAY, C_BLACK, 0);
}

static int log_count = 0;
static char logs[6][80];

void Dashboard_AddLog(const char* format, ...) {
    va_list args;
    va_start(args, format);
    for (int i = 0; i < 5; i++) {
        strncpy(logs[i], logs[i + 1], sizeof(logs[i]) - 1);
    }
    vsnprintf(logs[5], sizeof(logs[5]), format, args);
    va_end(args);
    log_count++;
}

void Dashboard_DrawPrompt(const char* input_buffer, int cursor_blink) {
    DrawTextColor(1, 24, "COMMAND? >", C_WHITE, C_BLACK, 0);
    DrawTextColor(12, 24, input_buffer, C_CYAN, C_BLACK, 0);
    
    // Cursor piscante
    if (cursor_blink) {
        int len = strlen(input_buffer);
        Terminal_SetCell(12 + len, 24, '_', C_CYAN, C_BLACK, 0);
    }
}

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
                    char qData[8];
                    
                    if (q) {
                        snprintf(qData, sizeof(qData), "%d%d%d", q->num_enemies, q->has_base ? 1 : 0, q->num_stars);
                    } else {
                        snprintf(qData, sizeof(qData), "???");
                    }
                    
                    bool isHere = (ent->quadX == gx && ent->quadY == gy);
                    
                    if (isHere) {
                        if (!scanned) {
                            DrawMiddleDots(px, y, C_BLACK, C_CYAN);
                        } else {
                            DrawTextColor(px, y, qData, C_BLACK, C_CYAN, 0);
                        }
                    } else {
                        if (!scanned) {
                            DrawMiddleDots(px, y, C_DGRAY, C_BLACK);
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

void Dashboard_DrawLogs(void) {
    // 6 logs armazenados, exibir os 4 mais recentes (indices 2..5) em y=20..23
    for (int i = 0; i < 4; i++) {
        uint8_t c = (i == 3) ? C_WHITE : C_DGRAY;
        DrawTextColor(1, 20 + i, logs[2 + i], c, C_BLACK, 0);
    }
}
