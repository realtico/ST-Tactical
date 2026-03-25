#ifndef ENTERPRISE_H
#define ENTERPRISE_H

#include <stdint.h>

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
    
    char condition[10]; // "GREEN", "YELLOW", "RED"
    
    // Integridade dos Sistemas (0.0 a 1.0)
    float sysWarp;
    float sysPhaser;
    float sysSensor;
    float sysLRS;
    float sysComputer;
} Enterprise;

#endif // ENTERPRISE_H
