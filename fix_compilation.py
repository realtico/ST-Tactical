import re

with open("src/video/test_video.c", "r") as f:
    content = f.read()

# Fix DrawMockupDashboard signature & variables
content = content.replace("void DrawMockupDashboard(ScanMode mode) {", "void DrawMockupDashboard(ScanMode mode, GalaxyState* galaxy, const char* input_buffer, int cursor_blink) {")

content = content.replace("Dashboard_DrawPrompt(input_buffer);\n    Dashboard_DrawLogs();\n    Dashboard_DrawEnterpriseStats(&ent);\n    if (mode == SCAN_MODE_SRS) Dashboard_DrawSRS(&ent);\n    else Dashboard_DrawLRS(&ent);", 
"Dashboard_DrawPrompt(input_buffer, cursor_blink);\n    Dashboard_DrawLogs();\n    Dashboard_DrawEnterpriseStats(&ent);\n    if (mode == SCAN_MODE_SRS) Dashboard_DrawSRS(galaxy, &ent);\n    else Dashboard_DrawLRS(galaxy, &ent);")

# Update call in main
content = content.replace("DrawMockupDashboard(currentMockupMode);", "DrawMockupDashboard(currentMockupMode, &galaxy, input_buffer, (GetTime() * 2) % 2 == 0);")

# Wait, there's another Dashboard_DrawPrompt call that might be duplicated or broken
content = re.sub(r"Dashboard_DrawLogs\(\);\n\s*Dashboard_DrawPrompt\(input_buffer\);", "", content)

with open("src/video/test_video.c", "w") as f:
    f.write(content)

