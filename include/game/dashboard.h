#ifndef DASHBOARD_H
#define DASHBOARD_H

#include "game/enterprise.h"
#include "game/galaxy.h"

// Modos de scan do painel central
typedef enum {
    SCAN_MODE_SRS = 0,
    SCAN_MODE_LRS,
    SCAN_MODE_MAP
} ScanMode;

// Rotinas de Desenho
void Dashboard_Init(void);
void Dashboard_DrawFrame(void);
void Dashboard_DrawEnterpriseStats(const Enterprise* ent);
void Dashboard_DrawSRS(GalaxyState* galaxy, const Enterprise* ent);
void Dashboard_DrawLRS(GalaxyState* galaxy, const Enterprise* ent);
void Dashboard_DrawGalaxyMap(GalaxyState* galaxy, const Enterprise* ent);
void Dashboard_AddLog(const char* format, ...);
void Dashboard_DrawLogs(void);
void Dashboard_DrawPrompt(const char* input_buffer, int cursor_blink);


#endif // DASHBOARD_H
