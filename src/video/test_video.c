#include <raylib.h>
#include "video/terminal.h"
#include "game/dashboard.h"
#include "game/enterprise.h"
#include <stdio.h>
#include <string.h>

// Tela 80x25 char (com JetBrains Mono de std 16px, dá 80*9, 25*16 aprox)
// Vamos escalar em 2x pra ver melhor, então janela 1280x720, virtual 80x25.
#define SCALE 2

void DrawMockupDashboard(ScanMode mode) {
    Enterprise ent;
    memset(&ent, 0, sizeof(Enterprise));
    ent.stardate = 3421.5;
    ent.quadX = 6;
    ent.quadY = 6;
    ent.sectX = 4;
    ent.sectY = 2;
    
    ent.energyMax = 5000;
    ent.energy = 3500;
    ent.shieldsMax = 100; // Porcentagem
    ent.shields = 21;
    ent.torpedoesMax = 10;
    ent.torpedoes = 10;
    
    strcpy(ent.condition, "RED");
    
    ent.sysWarp = 1.0f;
    ent.sysPhaser = 1.0f;
    ent.sysSensor = 1.0f;
    ent.sysLRS = 1.0f;
    ent.sysComputer = 1.0f;

    Dashboard_DrawTemplate();
    Dashboard_DrawEnterpriseStats(&ent);
    Dashboard_DrawScan(mode, &ent);
}

void FillStressTest() {
    int rows = 25; // default fallback, a usar GetScreen height params on resize depois, simulado
    int cols = 80;

    // Respeitar resolução atual (recriada na test)
    char c = '!';
    for (int y = 5; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            uint8_t fg = (x % 14) + 1; // Cores 1 a 14
            uint8_t bg = (y % 14) + 1; // Cores 1 a 14
            
            if (fg == bg) fg = (fg + 1) % 15; // evita texto invisível

            Terminal_SetCell(x, y, c, fg, bg, 0);

            c++;
            if (c > '~') c = '!'; // roll ascii
        }
    }

    // First rows = text tests
    const char *msg1 = "ZENIT-VIDEO HARDWARE TEST";
    for(int i=0; msg1[i] != '\0'; i++) {
        Terminal_SetCell(2+i, 1, msg1[i], 15, 0, 0);
    }

    const char *msg2 = "Negrito (BOLD) Teste ...";
    for(int i=0; msg2[i] != '\0'; i++) {
        Terminal_SetCell(2+i, 2, msg2[i], 10, 0, ATTR_BOLD);
    }
    
    // Italico
    const char *msg3 = "Italico (ITALIC) Teste ...";
    for(int i=0; msg3[i] != '\0'; i++) {
        Terminal_SetCell(2+i, 3, msg3[i], 11, 0, ATTR_ITALIC);
    }

    // Blink
    const char *msg4 = "Piscante (BLINK) Teste ...";
    for(int i=0; msg4[i] != '\0'; i++) {
        Terminal_SetCell(2+i, 4, msg4[i], 12, 0, ATTR_BLINK);
    }
}

int main(void) {
    int windowWidth = 1280;
    int windowHeight = 800;   

    InitWindow(windowWidth, windowHeight, "Terminal Hardware - Zenit-Video");
    SetTargetFPS(60);

    // Fonte em alta resolução (Supersampling interno) para garantir o "crisp" moderno
    if (!Terminal_Init(80, 25, "assets/fonts/JetBrainsMono.ttf", 36)) {
        printf("Falha ao inicializar o Terminal Virtual!\n");
        return 1;
    }

    int cursorX = 40;
    int cursorY = 12;

    int currentCols = 80;
    int currentRows = 25;

    // Gerar uma galáxia de teste!
    Galaxy_Generate(1701);

    while (!WindowShouldClose()) {

        // Tratamento do Controle do Cursor
        if (IsKeyPressed(KEY_RIGHT) && cursorX < currentCols - 1) cursorX++;
        if (IsKeyPressed(KEY_LEFT) && cursorX > 0) cursorX--;
        if (IsKeyPressed(KEY_DOWN) && cursorY < currentRows - 1) cursorY++;
        if (IsKeyPressed(KEY_UP) && cursorY > 0) cursorY--;

        // Hotkeys de Resolução
        if (IsKeyPressed(KEY_ONE)) {
            currentCols = 80; currentRows = 25;
            Terminal_Resize(currentCols, currentRows);
        }
        if (IsKeyPressed(KEY_TWO)) {
            currentCols = 100; currentRows = 32;
            Terminal_Resize(currentCols, currentRows);
        }
        
        static ScanMode currentMockupMode = SCAN_MODE_LRS;
        if (IsKeyPressed(KEY_SPACE)) {
            currentMockupMode = (currentMockupMode == SCAN_MODE_LRS) ? SCAN_MODE_SRS : SCAN_MODE_LRS;
        }
        
        Terminal_Clear(0);
        DrawMockupDashboard(currentMockupMode);
        
        VRAMCell cell = Terminal_GetCell(cursorX, cursorY);
        Terminal_SetCell(cursorX, cursorY, '_', cell.bg, cell.fg, ATTR_BLINK | ATTR_BOLD);
        
        Terminal_SwapBuffers();

        BeginDrawing();
            ClearBackground(BLACK);
            Terminal_Render();
            DrawFPS(10, 10);
        EndDrawing();
    }

    Terminal_Close();
    CloseWindow();
    return 0;
}
