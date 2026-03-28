with open("src/game/dashboard.c", "r") as f:
    data = f.read()

# SRS position fix
data = data.replace('int sX = CENTER_PANEL_X;\n    int sY = CENTER_PANEL_Y;', 'int sX = 18;\n    int sY = 2;')

# MAP header fix
data = data.replace('DrawTextColor(21, 2, "      GALAXY MAP      ", C_WHITE, C_BLACK, 0);', 'DrawTextColor(25, 1, " GALAXY MAP ", C_WHITE, C_BLACK, 0);')

with open("src/game/dashboard.c", "w") as f:
    f.write(data)
