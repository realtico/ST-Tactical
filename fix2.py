with open("src/game/dashboard.c", "r") as f:
    data = f.read()

# fix map Y to fit
data = data.replace('int mapY = 4;', 'int mapY = 3;')
data = data.replace('int sX = 20;\n    int sY = 4;', 'int sX = 28;\n    int sY = 4;')

with open("src/game/dashboard.c", "w") as f:
    f.write(data)
