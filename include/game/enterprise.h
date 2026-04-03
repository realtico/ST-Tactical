#ifndef ENTERPRISE_H
#define ENTERPRISE_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    COND_GREEN,
    COND_YELLOW,
    COND_RED
} Condition;

typedef struct {
    float x, y;          // Posição no grid 96x96
    
    // Recursos (Sistemas de Barras)
    int energy;
    int energyMax;
    int shields;
    int shieldsMax;
    int torpedoes;
    int torpedoesMax;
    
    double stardate;
    bool is_docked;
    
    Condition condition;
    
    // Integridade dos Sistemas (0.0 a 1.0)
    float sysWarp;
    float sysPhaser;
    float sysSensor;
    float sysLRS;
    float sysComputer;
} Enterprise;

void Enterprise_Init(Enterprise* ent);

bool calculate_impulse(float dist, int *out_e, float *out_t);
bool calculate_warp(float dist, float w, int *out_e, float *out_t);

#endif // ENTERPRISE_H
