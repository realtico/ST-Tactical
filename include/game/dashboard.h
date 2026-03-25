#ifndef DASHBOARD_H
#define DASHBOARD_H

#include "game/enterprise.h"
#include "game/galaxy.h"

// Modos de scan do painel central
typedef enum {
    SCAN_MODE_SRS = 0,
    SCAN_MODE_LRS
} ScanMode;

// Rotinas de Desenho
void Dashboard_Init(void);
void Dashboard_DrawTemplate(void);
void Dashboard_DrawEnterpriseStats(const Enterprise* ent);
void Dashboard_DrawScan(ScanMode mode, const Enterprise* ent);
void Dashboard_DrawLogs(const char* logs[], int log_count);

#endif // DASHBOARD_H
