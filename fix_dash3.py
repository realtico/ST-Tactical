import re

with open("src/game/dashboard.c", "r") as f:
    text = f.read()

# Fix signatures
text = text.replace("void Dashboard_DrawLRS(const Enterprise* ent) {", "void Dashboard_DrawLRS(GalaxyState* galaxy, const Enterprise* ent) {")
text = text.replace("void Dashboard_DrawSRS(const Enterprise* ent) {", "void Dashboard_DrawSRS(GalaxyState* galaxy, const Enterprise* ent) {")
text = text.replace("void Dashboard_DrawGalaxyMap(const Enterprise* ent) {", "void Dashboard_DrawGalaxyMap(GalaxyState* galaxy, const Enterprise* ent) {")

# Fix Galaxy calls
text = re.sub(r'Galaxy_GetQuadrant\(([^,]+), ([^)]+)\)', r'Galaxy_GetQuadrant(galaxy, \1, \2)', text)
text = re.sub(r'Galaxy_GetEnemy\(([^)]+)\)', r'Galaxy_GetEnemy(galaxy, \1)', text)

# Fix strcmp conditions
text = text.replace('strcmp(ent->condition, "RED") == 0', 'ent->condition == COND_RED')
text = text.replace('strcmp(ent->condition, "YELLOW") == 0', 'ent->condition == COND_YELLOW')
text = text.replace('snprintf(buf, sizeof(buf), "CONDITION: %s", ent->condition)', 'snprintf(buf, sizeof(buf), "CONDITION: %s", ent->condition == COND_RED ? "RED" : (ent->condition == COND_YELLOW ? "YELLOW" : "GREEN"))')

with open("src/game/dashboard.c", "w") as f:
    f.write(text)

