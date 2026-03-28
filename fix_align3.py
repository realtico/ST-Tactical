import re

with open("src/video/test_video.c", "r") as f:
    data = f.read()

# position terminal command down more
data = data.replace('Terminal_DrawText(1, 14', 'Terminal_DrawText(1, 24')
data = data.replace('Terminal_Print(3, 15', 'Terminal_Print(3, 25')
data = data.replace('Terminal_DrawText(1, 15', 'Terminal_DrawText(1, 25')

with open("src/video/test_video.c", "w") as f:
    f.write(data)

with open("src/game/dashboard.c", "r") as f:
    data2 = f.read()

data2 = data2.replace('void Dashboard_AddLog', 'int log_count = 0;\nchar logs[5][100];\n\nvoid Dashboard_AddLog')
data2 = data2.replace('// placeholder', 'log_count++;')
data2 = data2.replace('void Dashboard_DrawPrompt', 'void Dashboard_DrawPrompt')
data2 = data2.replace('void Dashboard_DrawLogs', 'void Dashboard_DrawLogs(void) {\n    for (int i = 0; i < 3; i++) {\n        DrawTextColor(1, 21+i, logs[i], C_WHITE, C_BLACK, 0);\n    }\n}')

with open("src/game/dashboard.c", "w") as f:
    f.write(data2)

