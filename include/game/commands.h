#ifndef COMMANDS_H
#define COMMANDS_H

#include "game/dashboard.h"
#include <stdbool.h>

// Callback de comando: recebe argc/argv tokenizados
typedef void (*CmdHandler)(int argc, char** argv);

typedef struct {
    const char* cmd_name;
    const char* description;
    CmdHandler  handler;
} CommandEntry;

// Inicializa o motor de comandos (modo SRS por padrão)
void Commands_Init(void);

// Executa uma linha de input (tokeniza e despacha)
void Commands_Execute(const char* input);

// Retorna o modo de scan atual (alterado por comandos SRS/LRS/MAP)
ScanMode Commands_GetScanMode(void);

// Retorna true se o jogador confirmou QUIT
bool Commands_ShouldQuit(void);

#endif // COMMANDS_H
