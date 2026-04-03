#include <raylib.h>
#include "video/terminal.h"
#include "game/dashboard.h"
#include "game/commands.h"
#include "game/enterprise.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

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
    Galaxy_Generate(&galaxy, (uint32_t)time(NULL));
    Enterprise_Init(&ent);

    // Marcar quadrante inicial como explorado (SRS scan)
    Galaxy_UpdateSensors(&galaxy, (int)(ent.x / 8), (int)(ent.y / 8), ent.stardate, false);

    Commands_Init();
    Dashboard_AddLog("STARDATE %.1f - ENTERPRISE READY.", ent.stardate);
    Dashboard_AddLog("Type HELP for available commands.");

    char input_buf[33] = {0};  // Limite de 32 caracteres + null
    int input_len = 0;
    int cursor_timer = 0;
    int prevQuadX = (int)(ent.x / 8);
    int prevQuadY = (int)(ent.y / 8);

    while (!WindowShouldClose() && !Commands_ShouldQuit()) {
        // --- Input: Movimento ---
        if (IsKeyPressed(KEY_RIGHT) && ent.x < 95.0f) ent.x++;
        if (IsKeyPressed(KEY_LEFT)  && ent.x > 0.0f)  ent.x--;
        if (IsKeyPressed(KEY_DOWN)  && ent.y < 95.0f) ent.y++;
        if (IsKeyPressed(KEY_UP)    && ent.y > 0.0f)  ent.y--;

        int currentQuadX = (int)(ent.x / 8);
        int currentQuadY = (int)(ent.y / 8);

        // Ao mudar de quadrante, atualizar sensores
        if (currentQuadX != prevQuadX || currentQuadY != prevQuadY) {
            Galaxy_UpdateSensors(&galaxy, currentQuadX, currentQuadY, ent.stardate, false);
            prevQuadX = currentQuadX;
            prevQuadY = currentQuadY;
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
                Commands_Execute(input_buf, &ent, &galaxy);
                input_len = 0;
                input_buf[0] = '\0';

                // Recalcular quadrante se houve warp/impulse
                currentQuadX = (int)(ent.x / 8);
                currentQuadY = (int)(ent.y / 8);
                if (currentQuadX != prevQuadX || currentQuadY != prevQuadY) {
                    Galaxy_UpdateSensors(&galaxy, currentQuadX, currentQuadY, ent.stardate, false);
                    prevQuadX = currentQuadX;
                    prevQuadY = currentQuadY;
                }
            }
        }

        // Modo atual vem do interpretador de comandos
        ScanMode currentMode = Commands_GetScanMode();

        // Ao ativar LRS, fazer scan de longo alcance
        if (currentMode == SCAN_MODE_LRS) {
            Galaxy_UpdateSensors(&galaxy, currentQuadX, currentQuadY, ent.stardate, true);
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
