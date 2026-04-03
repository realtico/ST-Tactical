#include "game/enterprise.h"

void Enterprise_Init(Enterprise* ent) {
    ent->x = 52.0f; // 6 * 8 + 4
    ent->y = 50.0f; // 6 * 8 + 2
    
    ent->energyMax = 5000;
    ent->energy = 5000;
    
    ent->shieldsMax = 1000;
    ent->shields = 1000;
    
    ent->torpedoesMax = 10;
    ent->torpedoes = 10;
    
    ent->stardate = 3400.0;
    ent->is_docked = false;

    ent->condition = COND_GREEN;
    
    ent->sysWarp = 1.0f;
    ent->sysPhaser = 1.0f;
    ent->sysSensor = 1.0f;
    ent->sysLRS = 1.0f;
    ent->sysComputer = 1.0f;
}

bool calculate_impulse(float dist, int *out_e, float *out_t) {
    *out_e = (int)(50.0f * dist);
    *out_t = 0.2f * dist;
    return true;
}

bool calculate_warp(float dist, float w, int *out_e, float *out_t) {
    if (w <= 0.0f) w = 1.0f;
    *out_e = (int)(200.0f + (0.4f * (w * w) * dist));
    *out_t = (0.08f / w) * dist;
    return true;
}
