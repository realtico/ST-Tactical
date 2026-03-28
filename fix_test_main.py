with open("src/video/test_video.c", "r") as f:
    data = f.read()

import re
data = data.replace("Galaxy_Generate(1701);", "GalaxyState galaxy;\n    Galaxy_Generate(&galaxy, 1701);")
data = data.replace("void DrawMockupDashboard(ScanMode mode, GalaxyState* galaxy, const char* input_buffer, int cursor_blink) {", "GalaxyState galaxy;\nvoid DrawMockupDashboard(ScanMode mode, GalaxyState* galaxy_ptr, const char* input_buffer, int cursor_blink) {\n    #define galaxy galaxy_ptr")

# remove global if exists
data = data.replace("#define galaxy galaxy_ptr\n", "")
data = data.replace("void DrawMockupDashboard(ScanMode mode, GalaxyState* galaxy_ptr, const char* input_buffer, int cursor_blink) {", "void DrawMockupDashboard(ScanMode mode, GalaxyState* galaxy, const char* input_buffer, int cursor_blink) {")

with open("src/video/test_video.c", "w") as f:
    f.write(data)
