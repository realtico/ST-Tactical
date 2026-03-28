with open("src/game/dashboard.c", "r") as f:
    data = f.read()

idx = data.find('int log_count = 0;')
if idx != -1:
    data = data[:idx]

with open("inject.c", "r") as f:
    inject = f.read()

with open("src/game/dashboard.c", "w") as f:
    f.write(data + inject)
