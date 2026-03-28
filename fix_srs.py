with open("src/game/dashboard.c", "r") as f:
    text = f.read()

# Fix SRS position to 18 so it doesn't overlap
text = text.replace("int sX = CENTER_PANEL_X;\n    int sY = CENTER_PANEL_Y;", "int sX = 18;\n    int sY = 2;")
text = text.replace("int sX = 27;\n    int sY = 4;", "int sX = 27;\n    int sY = 4;")  # LRS uses 27, fine.

with open("src/game/dashboard.c", "w") as f:
    f.write(text)

