#include "game/commands.h"
#include "game/dashboard.h"
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

#define MAX_ARGS  8
#define MAX_INPUT 64

static ScanMode current_mode = SCAN_MODE_SRS;
static bool quit_requested = false;
static bool quit_pending   = false;  // aguardando confirmação

// Helper param docagem
static void CheckDockingStatus(Enterprise* ent, GalaxyState* galaxy) {
    if (!ent || !galaxy) return;

    int gridX = (int)ent->x;
    int gridY = (int)ent->y;
    int qx = gridX / 8;
    int qy = gridY / 8;
    int sx = gridX % 8;
    int sy = gridY % 8;

    ent->is_docked = false;
    Quadrant* q = Galaxy_GetQuadrant(galaxy, qx, qy);
    if (!q) return;

    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            int cx = sx + dx;
            int cy = sy + dy;
            if (cx >= 0 && cx < 8 && cy >= 0 && cy < 8) {
                if (q->sectors[cx][cy].entity_type == ENTITY_BASE) {
                    ent->is_docked = true;
                    ent->energy = 5000;
                    ent->shields = 1000;
                    Dashboard_AddLog("DOCKING SEQUENCE INITIATED. SUPPLIES REPLENISHED.");
                    return;
                }
            }
        }
    }
}

// --- Handlers ---

static void Cmd_SRS(int argc, char** argv, Enterprise* ent, GalaxyState* galaxy) {
    (void)argc; (void)argv; (void)ent; (void)galaxy;
    current_mode = SCAN_MODE_SRS;
    Dashboard_AddLog("SHORT RANGE SCAN ACTIVATED.");
}

static void Cmd_LRS(int argc, char** argv, Enterprise* ent, GalaxyState* galaxy) {
    (void)argc; (void)argv; (void)ent; (void)galaxy;
    current_mode = SCAN_MODE_LRS;
    Dashboard_AddLog("LONG RANGE SCAN ACTIVATED.");
}

static void Cmd_MAP(int argc, char** argv, Enterprise* ent, GalaxyState* galaxy) {
    (void)argc; (void)argv; (void)ent; (void)galaxy;
    current_mode = SCAN_MODE_MAP;
    Dashboard_AddLog("GALACTIC CHART DISPLAYED.");
}

static void Cmd_SHI(int argc, char** argv, Enterprise* ent, GalaxyState* galaxy) {
    (void)galaxy;
    if (argc < 2) {
        Dashboard_AddLog("SHIELDS: %d/%d  ENERGY: %d/%d", ent->shields, ent->shieldsMax, ent->energy, ent->energyMax);
        return;
    }
    
    int amount = atoi(argv[1]);
    if (amount == 0) return;

    if (amount > 0) { // Transfer from energy to shields
        if (amount > ent->energy) amount = ent->energy;
        if (ent->shields + amount > ent->shieldsMax) amount = ent->shieldsMax - ent->shields;
        ent->energy -= amount;
        ent->shields += amount;
        ent->stardate += 0.1;
        Dashboard_AddLog("TRANSFERRED %d ENERGY TO SHIELDS.", amount);
    } else { // Transfer from shields to energy
        int transfer = -amount;
        if (transfer > ent->shields) transfer = ent->shields;
        if (ent->energy + transfer > ent->energyMax) transfer = ent->energyMax - ent->energy;
        ent->shields -= transfer;
        ent->energy += transfer;
        ent->stardate += 0.1;
        Dashboard_AddLog("TRANSFERRED %d SHIELDS TO ENERGY.", transfer);
    }
}

static void Cmd_IMP(int argc, char** argv, Enterprise* ent, GalaxyState* galaxy) {
    if (argc < 3) {
        Dashboard_AddLog("ERROR: INCOMPLETE COMMAND. USAGE: IMP <X> <Y>");
        return;
    }

    float tx = strtof(argv[1], NULL);
    float ty = strtof(argv[2], NULL);

    float dx = tx - ent->x;
    float dy = ty - ent->y;
    float dist = sqrtf(dx * dx + dy * dy);

    int energy_cost;
    float time_cost;
    if (!calculate_impulse(dist, &energy_cost, &time_cost)) return;

    if (energy_cost > ent->energy) {
        Dashboard_AddLog("ERROR: INSUFFICIENT ENERGY FOR MANEUVER.");
        return;
    }

    ent->energy -= energy_cost;
    ent->stardate += time_cost;
    ent->x = tx;
    ent->y = ty;

    CheckDockingStatus(ent, galaxy);
    Dashboard_AddLog("[IMP] Arrived at Sector %.1f,%.1f. Energy: %d. Stardate: %.1f", ent->x, ent->y, ent->energy, ent->stardate);
}

static void Cmd_WRP(int argc, char** argv, Enterprise* ent, GalaxyState* galaxy) {
    if (argc < 4) {
        Dashboard_AddLog("ERROR: INCOMPLETE COMMAND. USAGE: WRP <X> <Y> <FACTOR>");
        return;
    }

    float tx = strtof(argv[1], NULL);
    float ty = strtof(argv[2], NULL);
    float factor = strtof(argv[3], NULL);

    float dx = tx - ent->x;
    float dy = ty - ent->y;
    float dist = sqrtf(dx * dx + dy * dy);

    if (factor <= 0.0f) factor = 1.0f;
    if (factor > 8.0f) factor = 8.0f; 

    int energy_cost;
    float time_cost;
    if (!calculate_warp(dist, factor, &energy_cost, &time_cost)) return;

    if (energy_cost > ent->energy) {
        Dashboard_AddLog("ERROR: INSUFFICIENT ENERGY FOR MANEUVER.");
        return;
    }

    ent->energy -= energy_cost;
    ent->stardate += time_cost;
    ent->x = tx;
    ent->y = ty;

    CheckDockingStatus(ent, galaxy);
    Dashboard_AddLog("[WRP] Arrived at Sector %.1f,%.1f. Energy: %d. Stardate: %.1f", ent->x, ent->y, ent->energy, ent->stardate);
}

static void Cmd_HELP(int argc, char** argv, Enterprise* ent, GalaxyState* galaxy);
static void Cmd_QUIT(int argc, char** argv, Enterprise* ent, GalaxyState* galaxy);

// --- Tabela de Comandos ---

static CommandEntry commands[] = {
    { "SRS",  "Short Range Scan",  Cmd_SRS  },
    { "LRS",  "Long Range Scan",   Cmd_LRS  },
    { "MAP",  "Galactic Chart",    Cmd_MAP  },
    { "SHI",  "Shield Control",    Cmd_SHI  },
    { "IMP",  "Impulse power",     Cmd_IMP  },
    { "WRP",  "Warp drive",        Cmd_WRP  },
    { "HELP", "List commands",     Cmd_HELP },
    { "QUIT", "Exit game",         Cmd_QUIT },
    { NULL,   NULL,                NULL     }
};

static void Cmd_HELP(int argc, char** argv, Enterprise* ent, GalaxyState* galaxy) {
    (void)argc; (void)argv; (void)ent; (void)galaxy;
    Dashboard_AddLog("--- AVAILABLE COMMANDS ---");
    for (int i = 0; commands[i].cmd_name != NULL; i++) {
        Dashboard_AddLog("  %-6s %s", commands[i].cmd_name, commands[i].description);
    }
}

static void Cmd_QUIT(int argc, char** argv, Enterprise* ent, GalaxyState* galaxy) {
    (void)argc; (void)argv; (void)ent; (void)galaxy;
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

void Commands_Execute(const char* input, Enterprise* ent, GalaxyState* galaxy) {
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
            commands[i].handler(argc, argv, ent, galaxy);
            return;
        }
    }

    Dashboard_AddLog("ERROR: UNKNOWN COMMAND '%s'.", argv[0]);
}
