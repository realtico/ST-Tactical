import re

with open("src/video/test_video.c", "r") as f:
    data = f.read()

# position terminal command down more
data = data.replace('Terminal_DrawText(1, 24', 'Terminal_DrawText(1, 14')
data = data.replace('Terminal_Print(3, 25', 'Terminal_Print(3, 19')
data = data.replace('Terminal_DrawText(1, 25', 'Terminal_DrawText(1, 20')
data = data.replace('struct Command {', 'void Dashboard_AddLog(const char* format, ...);\n\nstruct Command {')

with open("src/video/test_video.c", "w") as f:
    f.write(data)

with open("src/game/dashboard.c", "r") as f:
    data2 = f.read()

data2 = data2.replace('void Dashboard_AddLog(const char* format, ...) {', 'void Dashboard_AddLog(const char* format, ...) {\n    va_list args;\n    va_start(args, format);\n    for (int i = 0; i < 4; i++) {\n        strncpy(logs[i], logs[i + 1], sizeof(logs[i]) - 1);\n    }\n    vsnprintf(logs[4], sizeof(logs[4]), format, args);\n    va_end(args);')
data2 = data2.replace('DrawTextColor(1, 21+i', 'DrawTextColor(1, 21+i') # Just in case
data2 = data2.replace('int sX = 16;\n    int sY = 2;\n    int eX = 52;\n    int eY = 13;', 'int sX = 16;\n    int sY = 2;\n    int eX = 52;\n    int eY = 18;')

with open("src/game/dashboard.c", "w") as f:
    f.write(data2)

