with open("src/game/dashboard.c", "r") as f:
    data = f.read()

data = data.replace('int sX = 16;\n    int sY = 2;\n    int eX = 64;\n    int eY = 17;', 'int sX = 16;\n    int sY = 2;\n    int eX = 52;\n    int eY = 13;')

with open("src/game/dashboard.c", "w") as f:
    f.write(data)

