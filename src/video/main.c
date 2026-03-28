#include <raylib.h>
#include "video/terminal.h"
#include "game/dashboard.h"
#include "game/commands.h"
#include "game/enterprise.h"
#include <stdio.h>
#include <string.h>

// Estado global do jogo
static Enterprise ent;
static GalaxyState galaxy;

// Renderiza o dashboard conforme o modo de scan ativo
static void DrawDashboard(ScanMode mode, const char* input_buffer, int cursor_blink) {
    if (mode == SCAN_MODE_MAP) {
        Dashboard_DrawGalaxyMap(&galaxy, &ent);
    } else {
        Dashboard_DrawFrame();
        Dashboard_DrawEnterpriseStats(&ent);
        if (mode == SCAN_MODE_SRS)
            Dashboard_DrawSRS(&galaxy, &ent);
        else if (mode == SCAN_MODE_LRS)
            Dashboard_DrawLRS(&galaxy, &ent);
    }
    Dashboard_DrawPrompt(input_buffer, cursor_blink);
    Dashboard_DrawLogs();
}

int main(void) {
    InitWindow(1280, 800, "ST-Tactical");
    SetTargetFPS(60);

    if (!Terminal_Init(80, 25, "assets/fonts/JetBrainsMono.ttf", 36)) {
        printf("Falha ao inicializar o Terminal Virtual!\n");
        return 1;
    }

    // Inicializar galáxia e Enterprise
    Galaxy_Generate(&galaxy, 1701);
    Enterprise_Init(&ent);

    // Marcar quadrante inicial como explorado (SRS scan)
    Galaxy_UpdateSensors(&galaxy, ent.quadX, ent.quadY, ent.stardate, false);

    Commands_Init();
    Dashboard_AddLog("STARDATE %.1f - ENTERPRISE READY.", ent.stardate);
    Dashboard_AddLog("Type HELP for available commands.");

    char input_buf[33] = {0};  // Limite de 32 caracteres + null
    int input_len = 0;
    int cursor_timer = 0;
    int prevQuadX = ent.quadX;
    int prevQuadY = ent.quadY;

    while (!WindowShouldClose() && !Commands_ShouldQuit()) {
        // --- Input: Movimento ---
        if (IsKeyPressed(KEY_RIGHT) && ent.sectX < 95) ent.sectX++;
        if (IsKeyPressed(KEY_LEFT)  && ent.sectX > 0)  ent.sectX--;
        if (IsKeyPressed(KEY_DOWN)  && ent.sectY < 95) ent.sectY++;
        if (IsKeyPressed(KEY_UP)    && ent.sectY > 0)  ent.sectY--;

        ent.quadX = ent.sectX / 8;
        ent.quadY = ent.sectY / 8;

        // Ao mudar de quadrante, atualizar sensores
        if (ent.quadX != prevQuadX || ent.quadY != prevQuadY) {
            Galaxy_UpdateSensors(&galaxy, ent.quadX, ent.quadY, ent.stardate, false);
            prevQuadX = ent.quadX;
            prevQuadY = ent.quadY;
        }

        // --- Input: Prompt de comando ---
        int ch = GetCharPressed();
        while (ch > 0) {
            if (ch >= 32 && ch < 127 && input_len < 32) {
                input_buf[input_len++] = (char)ch;
                input_buf[input_len] = '\0';
            }
            ch = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE) && input_len > 0) {
            input_buf[--input_len] = '\0';
        }
        if (IsKeyPressed(KEY_ENTER)) {
            if (input_len > 0) {
                Dashboard_AddLog("> %s", input_buf);
                Commands_Execute(input_buf);
                input_len = 0;
                input_buf[0] = '\0';
            }
        }

        // Modo atual vem do interpretador de comandos
        ScanMode currentMode = Commands_GetScanMode();

        // Ao ativar LRS, fazer scan de longo alcance
        if (currentMode == SCAN_MODE_LRS) {
            Galaxy_UpdateSensors(&galaxy, ent.quadX, ent.quadY, ent.stardate, true);
        }

        cursor_timer++;

        // --- Render ---
        Terminal_Clear(0);
        DrawDashboard(currentMode, input_buf, (cursor_timer / 30) % 2);
        Terminal_SwapBuffers();

        BeginDrawing();
            ClearBackground(BLACK);
            Terminal_Render();
        EndDrawing();
    }

    Terminal_Close();
    CloseWindow();
    return 0;
}
