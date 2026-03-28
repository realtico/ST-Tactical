with open("src/game/dashboard.c", "r") as f:
    data = f.read()

import re

start_idx = data.find('void Dashboard_DrawGalaxyMap')
end_idx = data.find('void Dashboard_DrawLogs(void) {')

with open("fix_map.c", "r") as f:
    inject = f.read()

new_data = data[:start_idx] + inject + "\n" + data[end_idx:]

with open("src/game/dashboard.c", "w") as f:
    f.write(new_data)
