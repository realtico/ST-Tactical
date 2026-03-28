#include "game/enterprise.h"

void Enterprise_Init(Enterprise* ent) {
    ent->stardate = 3421.5;
    ent->quadX = 6;
    ent->quadY = 6;
    ent->sectX = 6 * 8 + 4;
    ent->sectY = 6 * 8 + 2;
    
    ent->energyMax = 5000;
    ent->energy = 3500;
    
    ent->shieldsMax = 100;
    ent->shields = 21;
    
    ent->torpedoesMax = 10;
    ent->torpedoes = 10;
    
    ent->condition = COND_RED;
    
    ent->sysWarp = 1.0f;
    ent->sysPhaser = 1.0f;
    ent->sysSensor = 1.0f;
    ent->sysLRS = 1.0f;
    ent->sysComputer = 1.0f;
}
