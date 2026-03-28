import re

with open("src/game/dashboard.c", "r") as f:
    content = f.read()

# Replace colors
content = content.replace("C_CYAN", "11").replace("C_WHITE", "15").replace("C_BLACK", "0").replace("C_DGRAY", "8")

# Move DrawTextColor to the top
match = re.search(r"// Auxiliar para texto\nstatic void DrawTextColor\(.*?\)\s*\{.*?\}", content, re.DOTALL)
if match:
    func = match.group(0)
    content = content.replace(func, "")
    # insert it after includes
    insert_pos = content.find("#include <stdio.h>")
    if insert_pos == -1:
        insert_pos = content.find("#include <stdarg.h>")
    insert_pos = content.find("\n", insert_pos) + 1
    content = content[:insert_pos] + "\n" + func + "\n" + content[insert_pos:]

with open("src/game/dashboard.c", "w") as f:
    f.write(content)

