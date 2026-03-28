#include "game/commands.h"
#include "game/dashboard.h"
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_ARGS  8
#define MAX_INPUT 64

static ScanMode current_mode = SCAN_MODE_SRS;
static bool quit_requested = false;
static bool quit_pending   = false;  // aguardando confirmação

// --- Handlers ---

static void Cmd_SRS(int argc, char** argv) {
    (void)argc; (void)argv;
    current_mode = SCAN_MODE_SRS;
    Dashboard_AddLog("SHORT RANGE SCAN ACTIVATED.");
}

static void Cmd_LRS(int argc, char** argv) {
    (void)argc; (void)argv;
    current_mode = SCAN_MODE_LRS;
    Dashboard_AddLog("LONG RANGE SCAN ACTIVATED.");
}

static void Cmd_MAP(int argc, char** argv) {
    (void)argc; (void)argv;
    current_mode = SCAN_MODE_MAP;
    Dashboard_AddLog("GALACTIC CHART DISPLAYED.");
}

static void Cmd_HELP(int argc, char** argv);
static void Cmd_QUIT(int argc, char** argv);

// --- Tabela de Comandos ---

static CommandEntry commands[] = {
    { "SRS",  "Short Range Scan",  Cmd_SRS  },
    { "LRS",  "Long Range Scan",   Cmd_LRS  },
    { "MAP",  "Galactic Chart",    Cmd_MAP  },
    { "HELP", "List commands",     Cmd_HELP },
    { "QUIT", "Exit game",         Cmd_QUIT },
    { NULL,   NULL,                NULL     }
};

static void Cmd_HELP(int argc, char** argv) {
    (void)argc; (void)argv;
    Dashboard_AddLog("--- AVAILABLE COMMANDS ---");
    for (int i = 0; commands[i].cmd_name != NULL; i++) {
        Dashboard_AddLog("  %-6s %s", commands[i].cmd_name, commands[i].description);
    }
}

static void Cmd_QUIT(int argc, char** argv) {
    (void)argc; (void)argv;
    if (!quit_pending) {
        quit_pending = true;
        Dashboard_AddLog("ARE YOU SURE? TYPE 'Y' TO CONFIRM.");
    }
}

// --- API Pública ---

void Commands_Init(void) {
    current_mode = SCAN_MODE_SRS;
    quit_requested = false;
    quit_pending = false;
}

ScanMode Commands_GetScanMode(void) {
    return current_mode;
}

bool Commands_ShouldQuit(void) {
    return quit_requested;
}

void Commands_Execute(const char* input) {
    if (!input || input[0] == '\0') return;

    char buf[MAX_INPUT];
    strncpy(buf, input, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';

    // Converter para maiúsculas
    for (int i = 0; buf[i]; i++)
        buf[i] = (char)toupper((unsigned char)buf[i]);

    // Se QUIT pendente, checar confirmação
    if (quit_pending) {
        if (strcmp(buf, "Y") == 0 || strcmp(buf, "YES") == 0) {
            quit_requested = true;
        } else {
            quit_pending = false;
            Dashboard_AddLog("QUIT CANCELLED.");
        }
        return;
    }

    // Tokenizar
    char* argv[MAX_ARGS];
    int argc = 0;

    char* token = strtok(buf, " \t");
    while (token && argc < MAX_ARGS) {
        argv[argc++] = token;
        token = strtok(NULL, " \t");
    }

    if (argc == 0) return;

    // Buscar comando na tabela
    for (int i = 0; commands[i].cmd_name != NULL; i++) {
        if (strcmp(argv[0], commands[i].cmd_name) == 0) {
            commands[i].handler(argc, argv);
            return;
        }
    }

    Dashboard_AddLog("ERROR: UNKNOWN COMMAND '%s'.", argv[0]);
}
