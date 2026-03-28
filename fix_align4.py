with open("src/game/dashboard.c", "r") as f:
    data = f.read()

data = data.replace('}(void) {\n    for (int i = 0; i < 3; i++) {\n        DrawTextColor(1, 21+i, logs[i], C_WHITE, C_BLACK, 0);\n    }\n}', '}')

with open("src/game/dashboard.c", "w") as f:
    f.write(data)

