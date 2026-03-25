#include "game/dashboard.h"
#include "video/terminal.h"
#include <stdio.h>
#include <string.h>

// Cores (padrão terminal de 1 a 15, em tese, a paleta é customizável, mas usaremos 
// 15 = braco, 0 = preto, 10 = verde, 12 = vermelho, 14 = amarelo, 8 = cinza escuro)
#define C_WHITE  15
#define C_BLACK  0
#define C_GREEN  10
#define C_RED    9    // 9 é Bright Red no terminal
#define C_YELLOW 11   // 11 é Bright Yellow
#define C_DGRAY  8
#define C_CYAN   14   // 14 é Bright Cyan

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

// Função auxiliar para desenhar box
static void DrawBox(int x, int y, int w, int h, uint8_t fg, uint8_t bg) {
    Terminal_SetCell(x, y, B_TL, fg, bg, 0);
    Terminal_SetCell(x + w - 1, y, B_TR, fg, bg, 0);
    Terminal_SetCell(x, y + h - 1, B_BL, fg, bg, 0);
    Terminal_SetCell(x + w - 1, y + h - 1, B_BR, fg, bg, 0);
    
    for (int i = 1; i < w - 1; i++) {
        Terminal_SetCell(x + i, y, B_H, fg, bg, 0);
        Terminal_SetCell(x + i, y + h - 1, B_H, fg, bg, 0);
    }
    for (int i = 1; i < h - 1; i++) {
        Terminal_SetCell(x, y + i, B_V, fg, bg, 0);
        Terminal_SetCell(x + w - 1, y + i, B_V, fg, bg, 0);
    }
}

// Auxiliar para texto
static void DrawTextColor(int x, int y, const char* text, uint8_t fg, uint8_t bg, uint8_t attr) {
    for (int i = 0; text[i] != '\0'; i++) {
        Terminal_SetCell(x + i, y, text[i], fg, bg, attr);
    }
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

void Dashboard_DrawTemplate(void) {
    // Colunas do Template do PO:
    // Esq: 0 a 15 (W=16)
    // Centro: 16 a 52 (W=37)
    // Dir: 53 a 79 (W=27)
    
    // Top Row
    Terminal_SetCell(0, 0, B_TL, C_WHITE, C_BLACK, 0);
    for(int x=1; x<=14; x++) Terminal_SetCell(x, 0, B_H, C_WHITE, C_BLACK, 0);
    Terminal_SetCell(15, 0, B_HD, C_WHITE, C_BLACK, 0); // ┬
    for(int x=16; x<=51; x++) Terminal_SetCell(x, 0, B_H, C_WHITE, C_BLACK, 0);
    Terminal_SetCell(52, 0, B_HD, C_WHITE, C_BLACK, 0); // ┬
    for(int x=53; x<=78; x++) Terminal_SetCell(x, 0, B_H, C_WHITE, C_BLACK, 0);
    Terminal_SetCell(79, 0, B_TR, C_WHITE, C_BLACK, 0); // ┐
    
    // Header Titles
    DrawTextColor(1, 1, "  NAVIGATION  ", C_WHITE, C_BLACK, 0);
    DrawTextColor(53, 1, "         RESOURCES        ", C_WHITE, C_BLACK, 0);
    Terminal_SetCell(0, 1, B_V, C_WHITE, C_BLACK, 0);
    Terminal_SetCell(15, 1, B_V, C_WHITE, C_BLACK, 0);
    Terminal_SetCell(52, 1, B_V, C_WHITE, C_BLACK, 0);
    Terminal_SetCell(79, 1, B_V, C_WHITE, C_BLACK, 0);

    // Separators
    Terminal_SetCell(0, 2, B_VR, C_WHITE, C_BLACK, 0);
    for(int x=1; x<=14; x++) Terminal_SetCell(x, 2, B_H, C_WHITE, C_BLACK, 0);
    Terminal_SetCell(15, 2, B_VL, C_WHITE, C_BLACK, 0);
    
    Terminal_SetCell(52, 2, B_VR, C_WHITE, C_BLACK, 0);
    for(int x=53; x<=78; x++) Terminal_SetCell(x, 2, B_H, C_WHITE, C_BLACK, 0);
    Terminal_SetCell(79, 2, B_VL, C_WHITE, C_BLACK, 0);

    // Fill vertical borders for the rest of UI
    for (int y = 3; y < 19; y++) {
        Terminal_SetCell(0, y, B_V, C_WHITE, C_BLACK, 0);
        Terminal_SetCell(15, y, B_V, C_WHITE, C_BLACK, 0);
        Terminal_SetCell(52, y, B_V, C_WHITE, C_BLACK, 0);
        Terminal_SetCell(79, y, B_V, C_WHITE, C_BLACK, 0);
    }
    
    // Bottom border command prompt separator
    Terminal_SetCell(0, 19, B_BL, C_WHITE, C_BLACK, 0);
    for(int x=1; x<=14; x++) Terminal_SetCell(x, 19, B_H, C_WHITE, C_BLACK, 0);
    Terminal_SetCell(15, 19, B_HU, C_WHITE, C_BLACK, 0);
    for(int x=16; x<=51; x++) Terminal_SetCell(x, 19, B_H, C_WHITE, C_BLACK, 0);
    
    // Resources separator / Logs
    Terminal_SetCell(52, 12, B_VR, C_WHITE, C_BLACK, 0);
    for(int x=53; x<=78; x++) Terminal_SetCell(x, 12, B_H, C_WHITE, C_BLACK, 0);
    Terminal_SetCell(79, 12, B_VL, C_WHITE, C_BLACK, 0);
    
    DrawTextColor(53, 13, "       ENEMY STATUS       ", C_WHITE, C_BLACK, 0);

    Terminal_SetCell(52, 19, B_HU, C_WHITE, C_BLACK, 0);
    for(int x=53; x<=78; x++) Terminal_SetCell(x, 19, B_H, C_WHITE, C_BLACK, 0);
    Terminal_SetCell(79, 19, B_BR, C_WHITE, C_BLACK, 0); 
    
    // Nav Separator 
    Terminal_SetCell(0, 12, B_VR, C_WHITE, C_BLACK, 0);
    for(int x=1; x<=14; x++) Terminal_SetCell(x, 12, B_H, C_WHITE, C_BLACK, 0);
    Terminal_SetCell(15, 12, B_VL, C_WHITE, C_BLACK, 0);
    DrawTextColor(1, 13, " DAMAGE REPT  ", C_WHITE, C_BLACK, 0);
    
    // Command Prompt Area
    DrawTextColor(0, 20, "COMMAND? > _", C_WHITE, C_BLACK, 0);
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
    if (strcmp(ent->condition, "RED") == 0) condColor = C_RED;
    if (strcmp(ent->condition, "YELLOW") == 0) condColor = C_YELLOW;
    
    snprintf(buf, sizeof(buf), "CONDITION: %s", ent->condition);
    DrawTextColor(54, 3, buf, condColor, C_BLACK, 0);
    
    DrawTextColor(54, 4, "ENERGY:   ", C_WHITE, C_BLACK, 0);
    DrawProgressBar(65, 4, ent->energy, ent->energyMax, 10, C_YELLOW, C_DGRAY, C_BLACK);
    snprintf(buf, sizeof(buf), "(%d/%d)", ent->energy, ent->energyMax);
    DrawTextColor(56, 5, buf, C_WHITE, C_BLACK, 0);

    DrawTextColor(54, 7, "SHIELDS:  ", C_WHITE, C_BLACK, 0);
    DrawProgressBar(65, 7, ent->shields, ent->shieldsMax, 10, C_CYAN, C_DGRAY, C_BLACK);
    snprintf(buf, sizeof(buf), "(%d%%)", (int)(((float)ent->shields/(float)ent->shieldsMax)*100));
    DrawTextColor(56, 8, buf, C_WHITE, C_BLACK, 0);
    
    DrawTextColor(54, 10, "TORPEDOES:", C_WHITE, C_BLACK, 0);
    DrawProgressBar(65, 10, ent->torpedoes, ent->torpedoesMax, 10, C_RED, C_DGRAY, C_BLACK);
    snprintf(buf, sizeof(buf), "(%d/%d)", ent->torpedoes, ent->torpedoesMax);
    DrawTextColor(56, 11, buf, C_WHITE, C_BLACK, 0);
}

void Dashboard_DrawScan(ScanMode mode, const Enterprise* ent) {
    if (mode == SCAN_MODE_LRS) {
        DrawTextColor(16, 1, "       LONG RANGE SCAN  (LRS)       ", C_WHITE, C_BLACK, 0);
        
        int sX = 27;
        // Header
        DrawTextColor(sX+2, 3, "00  01  02", C_WHITE, C_BLACK, 0);
        
        // LRS Grid Top
        int sY = 4;
        Terminal_SetCell(sX, sY, B_TL, C_WHITE, C_BLACK, 0);
        for(int x=0; x<3;x++){
            Terminal_SetCell(sX+1 + x*4, sY, B_H, C_WHITE, C_BLACK, 0);
            Terminal_SetCell(sX+2 + x*4, sY, B_H, C_WHITE, C_BLACK, 0);
            Terminal_SetCell(sX+3 + x*4, sY, B_H, C_WHITE, C_BLACK, 0);
            if (x < 2) Terminal_SetCell(sX+4 + x*4, sY, B_HD, C_WHITE, C_BLACK, 0);
        }
        Terminal_SetCell(sX+12, sY, B_TR, C_WHITE, C_BLACK, 0);
        
        // 3x3 Grid
        for (int y = 0; y < 3; y++) {
            char rBuf[5];
            snprintf(rBuf, sizeof(rBuf), "%02d", y);
            DrawTextColor(sX-3, sY+1 + y*2, rBuf, C_WHITE, C_BLACK, 0);
            
            for (int x = 0; x < 3; x++) {
                Terminal_SetCell(sX + x*4, sY+1 + y*2, B_V, C_WHITE, C_BLACK, 0);
                
                // Get Galaxy Data Relative to Enterprise
                int checkQx = ent->quadX - 1 + x;
                int checkQy = ent->quadY - 1 + y;
                
                Quadrant* q = Galaxy_GetQuadrant(checkQx, checkQy);
                if (q != NULL) {
                    char data[4];
                    snprintf(data, sizeof(data), "%d%d%d", q->num_enemies, q->has_base ? 1 : 0, q->num_stars);
                    DrawTextColor(sX+1 + x*4, sY+1 + y*2, data, C_GREEN, C_BLACK, 0);
                } else {
                    DrawTextColor(sX+1 + x*4, sY+1 + y*2, "***", C_DGRAY, C_BLACK, 0); // Out of bounds
                }
            }
            Terminal_SetCell(sX+12, sY+1 + y*2, B_V, C_WHITE, C_BLACK, 0);
            
            // Mid Row
            if (y < 2) {
                Terminal_SetCell(sX, sY+2 + y*2, B_VR, C_WHITE, C_BLACK, 0);
                for(int x=0; x<3;x++){
                    Terminal_SetCell(sX+1 + x*4, sY+2 + y*2, B_H, C_WHITE, C_BLACK, 0);
                    Terminal_SetCell(sX+2 + x*4, sY+2 + y*2, B_H, C_WHITE, C_BLACK, 0);
                    Terminal_SetCell(sX+3 + x*4, sY+2 + y*2, B_H, C_WHITE, C_BLACK, 0);
                    if (x < 2) Terminal_SetCell(sX+4 + x*4, sY+2 + y*2, B_X, C_WHITE, C_BLACK, 0);
                }
                Terminal_SetCell(sX+12, sY+2 + y*2, B_VL, C_WHITE, C_BLACK, 0);
            }
        }
        
        // Bottom Row
        int bY = sY + 6;
        Terminal_SetCell(sX, bY, B_BL, C_WHITE, C_BLACK, 0);
        for(int x=0; x<3;x++){
            Terminal_SetCell(sX+1 + x*4, bY, B_H, C_WHITE, C_BLACK, 0);
            Terminal_SetCell(sX+2 + x*4, bY, B_H, C_WHITE, C_BLACK, 0);
            Terminal_SetCell(sX+3 + x*4, bY, B_H, C_WHITE, C_BLACK, 0);
            if (x < 2) Terminal_SetCell(sX+4 + x*4, bY, B_HU, C_WHITE, C_BLACK, 0);
        }
        Terminal_SetCell(sX+12, bY, B_BR, C_WHITE, C_BLACK, 0);
        
    } else {
        // SRS Mode
        int sX = 19;
        int sY = 2;
        
        DrawTextColor(sX+2, 1, "00  01  02  03  04  05  06  07", C_WHITE, C_BLACK, 0);
        
        // Top
        Terminal_SetCell(sX, sY, B_TL, C_DGRAY, C_BLACK, 0);
        for(int x=0; x<8;x++){
            Terminal_SetCell(sX+1 + x*4, sY, B_H, C_DGRAY, C_BLACK, 0);
            Terminal_SetCell(sX+2 + x*4, sY, B_H, C_DGRAY, C_BLACK, 0);
            Terminal_SetCell(sX+3 + x*4, sY, B_H, C_DGRAY, C_BLACK, 0);
            if (x < 7) Terminal_SetCell(sX+4 + x*4, sY, B_HD, C_DGRAY, C_BLACK, 0);
        }
        Terminal_SetCell(sX+32, sY, B_TR, C_DGRAY, C_BLACK, 0);
        
        // Quadrant
        Quadrant* q = Galaxy_GetQuadrant(ent->quadX, ent->quadY);
        
        for (int y = 0; y < 8; y++) {
            char rBuf[5];
            snprintf(rBuf, sizeof(rBuf), "%02d", y);
            DrawTextColor(sX-3, sY+1 + y*2, rBuf, C_WHITE, C_BLACK, 0);
            
            for (int x = 0; x < 8; x++) {
                Terminal_SetCell(sX + x*4, sY+1 + y*2, B_V, C_DGRAY, C_BLACK, 0);
                
                // Draw Entity
                int charStart = sX + 1 + x*4;
                if (x == ent->sectX && y == ent->sectY) {
                    DrawTextColor(charStart, sY+1 + y*2, "<E>", C_CYAN, C_BLACK, 0);
                } else if (q != NULL) {
                    Sector* s = &q->sectors[y][x];
                    if (s->entity_type == ENTITY_STAR) {
                        DrawTextColor(charStart, sY+1 + y*2, " * ", C_YELLOW, C_BLACK, 0);
                    } else if (s->entity_type == ENTITY_BASE) {
                        DrawTextColor(charStart, sY+1 + y*2, ">!<", C_CYAN, C_BLACK, 0);
                    } else if (s->entity_type == ENTITY_ENEMY) {
                        KlingonGroup* e = Galaxy_GetEnemy(s->entity_id);
                        if (e->class_id == 0) DrawTextColor(charStart, sY+1 + y*2, "+++", C_RED, C_BLACK, 0); // B'rel
                        else if (e->class_id == 1) DrawTextColor(charStart, sY+1 + y*2, "+K+", C_RED, C_BLACK, 0); // K'Tinga
                        else DrawTextColor(charStart, sY+1 + y*2, "+V+", C_RED, C_BLACK, 0); // Vor'Cha
                    } else {
                        DrawTextColor(charStart, sY+1 + y*2, "   ", C_WHITE, C_BLACK, 0); // Empty
                    }
                }
            }
            Terminal_SetCell(sX+32, sY+1 + y*2, B_V, C_DGRAY, C_BLACK, 0);
            
            // Mid Row
            if (y < 7) {
                Terminal_SetCell(sX, sY+2 + y*2, B_VR, C_DGRAY, C_BLACK, 0);
                for(int x=0; x<8;x++){
                    Terminal_SetCell(sX+1 + x*4, sY+2 + y*2, B_H, C_DGRAY, C_BLACK, 0);
                    Terminal_SetCell(sX+2 + x*4, sY+2 + y*2, B_H, C_DGRAY, C_BLACK, 0);
                    Terminal_SetCell(sX+3 + x*4, sY+2 + y*2, B_H, C_DGRAY, C_BLACK, 0);
                    if (x < 7) Terminal_SetCell(sX+4 + x*4, sY+2 + y*2, B_X, C_DGRAY, C_BLACK, 0);
                }
                Terminal_SetCell(sX+32, sY+2 + y*2, B_VL, C_DGRAY, C_BLACK, 0);
            }
        }
        
        // Bottom Row completely standalone for SRS
        Terminal_SetCell(sX, sY+16, B_BL, C_DGRAY, C_BLACK, 0);
        for(int x=0; x<8;x++){
            Terminal_SetCell(sX+1 + x*4, sY+16, B_H, C_DGRAY, C_BLACK, 0);
            Terminal_SetCell(sX+2 + x*4, sY+16, B_H, C_DGRAY, C_BLACK, 0);
            Terminal_SetCell(sX+3 + x*4, sY+16, B_H, C_DGRAY, C_BLACK, 0);
            if (x < 7) {
                Terminal_SetCell(sX+4 + x*4, sY+16, B_HU, C_DGRAY, C_BLACK, 0);
            }
        }
        Terminal_SetCell(sX+32, sY+16, B_BR, C_DGRAY, C_BLACK, 0);
    }
}
