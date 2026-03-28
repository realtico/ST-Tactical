with open("src/game/dashboard.c", "r") as f:
    lines = f.readlines()

# add closing brace at line 10
lines.insert(9, "}\n")

with open("src/game/dashboard.c", "w") as f:
    f.writelines(lines)

