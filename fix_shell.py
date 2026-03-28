import re

with open("src/video/test_video.c", "r") as f:
    text = f.read()


engine_code = """
// --- COMMAND SHELL ENGINE ---
typedef void (*CmdHandler)(int argc, char** argv);

typedef struct {
    const char* cmd_name;
    CmdHandler handler;
} CommandEntry;

static ScanMode currentMockupMode = SCAN_MODE_LRS;
static GalaxyState* p_galaxy = NULL;
static bool show_debug = true;
static bool require_quit_confirm = false;
static bool quit_requested = false;

static void Cmd_SRS(int argc, char** argv) {
    currentMockupMode = SCAN_MODE_SRS;
    Dashboard_AddLog("Sensors set to Short Range Scan (SRS).");
}

static void Cmd_LRS(int argc, char** argv) {
    currentMockupMode = SCAN_MODE_LRS;
    if (p_galaxy) Galaxy_UpdateSensors(p_galaxy, ent.quadX, ent.quadY, ent.stardate, true);
    Dashboard_AddLog("Sensors set to Long Range Scan (LRS).");
}

static void Cmd_MAP(int argc, char** argv) {
    currentMockupMode = SCAN_MODE_MAP;
    Dashboard_AddLog("Overlay set to Galactic Chart.");
}

static void Cmd_HELP(int argc, char** argv) {
    Dashboard_AddLog("Available Commands: SRS, LRS, MAP, HELP, DBG, QUT");
}

static void Cmd_DBG(int argc, char** argv) {
    show_debug = !show_debug;
    Dashboard_AddLog(show_debug ? "Debug overlay ENABLED." : "Debug overlay DISABLED.");
}

static void Cmd_QUT(int argc, char** argv) {
    require_quit_confirm = true;
    Dashboard_AddLog("WARNING: Are you sure you want to quit? (Y/N)");
}

static CommandEntry cmd_table[] = {
    {"SRS", Cmd_SRS},
    {"LRS", Cmd_LRS},
    {"MAP", Cmd_MAP},
    {"HELP", Cmd_HELP},
    {"DBG", Cmd_DBG},
    {"QUT", Cmd_QUT},
    {NULL, NULL}
};

void Shell_ProcessCommand(const char* input) {
    if (require_quit_confirm) {
        if (input[0] == 'Y' || input[0] == 'y') {
            quit_requested = true;
        } else {
            require_quit_confirm = false;
            Dashboard_AddLog("Quit cancelled.");
        }
        return;
    }

    char buffer[64];
    strncpy(buffer, input, sizeof(buffer)-1);
    buffer[sizeof(buffer)-1] = '\\0';
    
    char* argv[10];
    int argc = 0;
    
    char* token = strtok(buffer, " ");
    while (token != NULL && argc < 10) {
        argv[argc++] = token;
        token = strtok(NULL, " ");
    }
    
    if (argc == 0) return;
    
    // Check commands
    for (int i = 0; cmd_table[i].cmd_name != NULL; i++) {
        if (strcasecmp(argv[0], cmd_table[i].cmd_name) == 0) {
            cmd_table[i].handler(argc, argv);
            return;
        }
    }
    
    Dashboard_AddLog("ERROR: UNKNOWN COMMAND '%s'.", argv[0]);
}

// ---------------------------
"""

if "COMMAND SHELL ENGINE" not in text:
    text = text.replace("Enterprise ent;\nvoid DrawMockupDashboard", engine_code + "\nEnterprise ent;\nvoid DrawMockupDashboard")


setup_game_loop = """
    p_galaxy = &galaxy;
    Dashboard_AddLog("USS Enterprise NCC-1701 Initialized.");
    Dashboard_AddLog("Type HELP for a list of commands.");
    
    char input_buffer[33] = "";
    int input_length = 0;
    
    SetExitKey(0); // Disable ESC to quit automatically
    
    int blink_timer = 0;
"""

text = text.replace("Enterprise_Init(&ent);", "Enterprise_Init(&ent);\n" + setup_game_loop)

key_handling = """
        // Handle input buffer
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

        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) {
            if (input_length > 0) {
                Shell_ProcessCommand(input_buffer);
                input_buffer[0] = '\\0';
                input_length = 0;
            }
        }
        
        blink_timer++;
        
        if (quit_requested) {
            break; // Exit loop
        }
"""

text = re.sub(r"static ScanMode currentMockupMode = SCAN_MODE_LRS;.*?if \(IsKeyPressed\(KEY_S\)\) {\n            currentMockupMode = SCAN_MODE_SRS;\n        }", key_handling, text, flags=re.DOTALL)

draw_additions = """        Terminal_Clear(0);
        DrawMockupDashboard(&galaxy, currentMockupMode);
        
        if (currentMockupMode != SCAN_MODE_MAP) {
            Dashboard_DrawLogs();
            Dashboard_DrawPrompt(input_buffer, (blink_timer % 60) < 30);
        } else {
            // Draw prompt minimally or floating on map? 
            // The prompt said: "No modo Mapa, o dashboard lateral não deve ser desenhado..."
            // We can draw prompt at bottom of map
            Dashboard_DrawPrompt(input_buffer, (blink_timer % 60) < 30);
            Dashboard_DrawLogs();
        }
"""
text = text.replace("Terminal_Clear(0);\n        DrawMockupDashboard(&galaxy, currentMockupMode);", draw_additions)
text = text.replace("DrawFPS(10, 10);", "if (show_debug) DrawFPS(10, 10);")

with open("src/video/test_video.c", "w") as f:
    f.write(text)

