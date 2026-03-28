import re

with open("include/game/dashboard.h", "r") as f:
    text = f.read()

if "Dashboard_AddLog" not in text:
    text = text.replace("void Dashboard_DrawGalaxyMap(GalaxyState* galaxy, const Enterprise* ent);", "void Dashboard_DrawGalaxyMap(GalaxyState* galaxy, const Enterprise* ent);\nvoid Dashboard_AddLog(const char* format, ...);\nvoid Dashboard_DrawLogs(void);\nvoid Dashboard_DrawPrompt(const char* input_buffer, int cursor_blink);")

with open("include/game/dashboard.h", "w") as f:
    f.write(text)

