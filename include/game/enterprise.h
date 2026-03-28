#ifndef ENTERPRISE_H
#define ENTERPRISE_H

#include <stdint.h>

typedef enum {
    COND_GREEN,
    COND_YELLOW,
    COND_RED
} Condition;

typedef struct {
    double stardate;
    int quadX, quadY;
    int sectX, sectY;
    
    // Recursos (Sistemas de Barras)
    int energy;
    int energyMax;
    int shields;
    int shieldsMax;
    int torpedoes;
    int torpedoesMax;
    
    Condition condition;
    
    // Integridade dos Sistemas (0.0 a 1.0)
    float sysWarp;
    float sysPhaser;
    float sysSensor;
    float sysLRS;
    float sysComputer;
} Enterprise;

void Enterprise_Init(Enterprise* ent);

#endif // ENTERPRISE_H
