with open("src/game/dashboard.c", "r") as f:
    data = f.read()

# Replace the single dot with three middle dots (U+00B7 is \xB7)
data = data.replace('DrawTextColor(px, y, " . ", C_BLACK, C_CYAN, 0);', 'DrawTextColor(px, y, "\\xB7\\xB7\\xB7", C_BLACK, C_CYAN, 0);')
data = data.replace('DrawTextColor(px, y, " . ", C_DGRAY, C_BLACK, 0);', 'DrawTextColor(px, y, "\\xB7\\xB7\\xB7", C_DGRAY, C_BLACK, 0);')

with open("src/game/dashboard.c", "w") as f:
    f.write(data)
