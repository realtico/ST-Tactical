with open("src/game/dashboard.c", "r") as f:
    text = f.read()

text = text.replace('strcmp(ent->condition, "RED") == 0', 'ent->condition == COND_RED')
text = text.replace('strcmp(ent->condition, "YELLOW") == 0', 'ent->condition == COND_YELLOW')

with open("src/game/dashboard.c", "w") as f:
    f.write(text)

