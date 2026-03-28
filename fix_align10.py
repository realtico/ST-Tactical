with open("src/game/dashboard.c", "r") as f:
    data = f.read()

data = data.replace('va_start(args, format);', '__builtin_va_start(args, format);')
data = data.replace('va_end(args);', '__builtin_va_end(args);')

with open("src/game/dashboard.c", "w") as f:
    f.write(data)

