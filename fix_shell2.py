import re

with open("src/video/test_video.c", "r") as f:
    content = f.read()

# 1. Insert shell engine before main
shell_engine = """
// -----------------------------------------------------------------------------
// COMANDOS DO SISTEMA (SHELL ENGINE)
// -----------------------------------------------------------------------------

static ScanMode currentMockupMode = SCAN_MODE_LRS;
static bool showFPS = false;
static bool shouldQuit = false;

typedef void (*CmdHandler)(int argc, char** argv);

typedef struct {
    const char* name;
    CmdHandler handler;
} CommandEntry;

void Cmd_SRS(int argc, char** argv) {
    currentMockupMode = SCAN_MODE_SRS;
    Dashboard_AddLog("Scan Curto (SRS) ativado. Quadrante Q%.1f-%.1f.", (float)ent.quadX, (float)ent.quadY);
}

void Cmd_LRS(int argc, char** argv) {
    currentMockupMode = SCAN_MODE_LRS;
    Dashboard_AddLog("Scan Longo (LRS) ativado.");
}

void Cmd_MAP(int argc, char** argv) {
    Dashboard_AddLog("Mapa Galactico acessado. Em construcao.");
}

void Cmd_HELP(int argc, char** argv) {
    Dashboard_AddLog("Comandos validos: SRS, LRS, MAP, HELP, DBG, QUT.");
}

void Cmd_DBG(int argc, char** argv) {
    showFPS = !showFPS;
    Dashboard_AddLog("Debug mode (FPS): %s", showFPS ? "ON" : "OFF");
}

void Cmd_QUT(int argc, char** argv) {
    Dashboard_AddLog("Desligando modulos... Pressione SHIFT+Q para sair.");
    shouldQuit = true;
}

static CommandEntry cmd_table[] = {
    {"SRS", Cmd_SRS},
    {"LRS", Cmd_LRS},
    {"MAP", Cmd_MAP},
    {"HELP", Cmd_HELP},
    {"DBG", Cmd_DBG},
    {"QUT", Cmd_QUT}
};

void Shell_ProcessCommand(const char* input) {
    char buffer[256];
    strncpy(buffer, input, sizeof(buffer)-1);
    buffer[255] = '\\0';

    char* argv[16];
    int argc = 0;

    char* token = strtok(buffer, " ");
    while (token != NULL && argc < 16) {
        argv[argc++] = token;
        token = strtok(NULL, " ");
    }

    if (argc == 0) return;

    // Converte para uppercase por simplicidade (assumindo single-byte)
    for (int i = 0; argv[0][i]; i++) {
        if (argv[0][i] >= 'a' && argv[0][i] <= 'z') {
            argv[0][i] -= 32;
        }
    }

    Dashboard_AddLog("> %s", input);

    for (int i = 0; i < sizeof(cmd_table)/sizeof(cmd_table[0]); i++) {
        if (strcmp(cmd_table[i].name, argv[0]) == 0) {
            cmd_table[i].handler(argc, argv);
            return;
        }
    }

    Dashboard_AddLog("Comando nao reconhecido: '%s'. Digite HELP.", argv[0]);
}

"""
content = re.sub(r"int main\(void\) \{", shell_engine + "int main(void) {\n    char input_buffer[33] = {0};\n    int input_length = 0;\n", content)

# 2. Replace the while loop body
new_loop_body = """    while (!WindowShouldClose()) {
        if (shouldQuit && IsKeyDown(KEY_LEFT_SHIFT) && IsKeyPressed(KEY_Q)) {
            break;
        }

        // Handle text input
        int key = GetCharPressed();
        while (key > 0) {
            if ((key >= 32) && (key <= 125) && (input_length < 32)) {
                input_buffer[input_length] = (char)key;
                input_buffer[input_length + 1] = '\\0';
                input_length++;
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE)) {
            if (input_length > 0) {
                input_length--;
                input_buffer[input_length] = '\\0';
            }
        }

        if (IsKeyPressed(KEY_ENTER)) {
            if (input_length > 0) {
                Shell_ProcessCommand(input_buffer);
                input_buffer[0] = '\\0';
                input_length = 0;
            }
        }
        
        Terminal_Clear(0);
        DrawMockupDashboard(currentMockupMode);
        Dashboard_DrawLogs();
        Dashboard_DrawPrompt(input_buffer);
        
        Terminal_SwapBuffers();

        BeginDrawing();
            ClearBackground(BLACK);
            Terminal_Render();
            if (showFPS) DrawFPS(10, 10);
        EndDrawing();
    }"""
    
content = re.sub(r"    while \(\!WindowShouldClose\(\)\) \{.*EndDrawing\(\);\n    \}", new_loop_body, content, flags=re.DOTALL)

with open("src/video/test_video.c", "w") as f:
    f.write(content)

