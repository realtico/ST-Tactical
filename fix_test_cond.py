with open("src/video/test_video.c", "r") as f:
    data = f.read()

data = data.replace('strcpy(ent.condition, "RED");', 'ent.condition = COND_RED;')
data = data.replace('strcpy(ent.condition, "GREEN");', 'ent.condition = COND_GREEN;')
data = data.replace('strcpy(ent.condition, "YELLOW");', 'ent.condition = COND_YELLOW;')
data = data.replace('strcpy(ent.condition, "DOCKED");', 'ent.condition = COND_DOCKED;')

with open("src/video/test_video.c", "w") as f:
    f.write(data)
