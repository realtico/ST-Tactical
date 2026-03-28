with open("src/game/dashboard.c", "r") as f:
    data = f.read()

data = data.replace('#include <stdint.h>', '#include <stdint.h>\n#include <stdarg.h>')

with open("src/game/dashboard.c", "w") as f:
    f.write(data)

