with open("src/game/dashboard.c", "r") as f:
    data = f.read()

data = data.replace('#define CENTER_PANEL_X 27', '#define CENTER_PANEL_X 18')
data = data.replace('int sX = 27;', 'int sX = 20;')

with open("src/game/dashboard.c", "w") as f:
    f.write(data)
