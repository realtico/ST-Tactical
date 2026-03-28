with open("src/game/dashboard.c", "r") as f:
    lines = f.readlines()

with open("src/game/dashboard.c", "w") as f:
    for i, line in enumerate(lines):
        if i >= 427 and i <= 442:
            continue
        if i == 443:
            f.write("void Dashboard_DrawGalaxyMap(GalaxyState* galaxy, const Enterprise* ent) {\n")
            continue
        f.write(line)

