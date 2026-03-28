with open("src/game/dashboard.c", "r") as f:
    data = f.read()

data = data.replace('DrawTextColor(sX + 2 + (x * 12), sY + 2 + (y * 3), C_WHITE, C_BLACK, 0, name, C_WHITE, C_BLACK, 0);', 'DrawTextColor(sX + 2 + (x * 12), sY + 2 + (y * 3), name, C_WHITE, C_BLACK, 0);')
data = data.replace('DrawTextColor(sX + 2 + (x * 12), sY + 3 + (y * 3), C_WHITE, C_BLACK, "0E 0B 0S", C_WHITE, C_BLACK, 0);', 'DrawTextColor(sX + 2 + (x * 12), sY + 3 + (y * 3), "0E 0B 0S", C_WHITE, C_BLACK, 0);')

with open("src/game/dashboard.c", "w") as f:
    f.write(data)

