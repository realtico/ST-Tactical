with open("src/video/test_video.c", "r") as f:
    data = f.read()

import re

old_func = """void DrawMockupDashboard(ScanMode mode, GalaxyState* galaxy, const char* input_buffer, int cursor_blink) {
    Dashboard_DrawFrame();
    Dashboard_DrawPrompt(input_buffer, cursor_blink);
    Dashboard_DrawLogs();
    Dashboard_DrawEnterpriseStats(&ent);
    if (mode == SCAN_MODE_SRS) Dashboard_DrawSRS(galaxy, &ent);
    else if (mode == SCAN_MODE_LRS) Dashboard_DrawLRS(galaxy, &ent);
    else if (mode == SCAN_MODE_MAP) Dashboard_DrawGalaxyMap(galaxy, &ent);
}"""

new_func = """void DrawMockupDashboard(ScanMode mode, GalaxyState* galaxy, const char* input_buffer, int cursor_blink) {
    if (mode == SCAN_MODE_MAP) {
        Dashboard_DrawGalaxyMap(galaxy, &ent);
    } else {
        Dashboard_DrawFrame();
        Dashboard_DrawEnterpriseStats(&ent);
        if (mode == SCAN_MODE_SRS) Dashboard_DrawSRS(galaxy, &ent);
        else if (mode == SCAN_MODE_LRS) Dashboard_DrawLRS(galaxy, &ent);
    }
    Dashboard_DrawPrompt(input_buffer, cursor_blink);
    Dashboard_DrawLogs();
}"""

data = data.replace(old_func, new_func)

with open("src/video/test_video.c", "w") as f:
    f.write(data)
