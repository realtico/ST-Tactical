with open("src/game/dashboard.c", "r") as f:
    text = f.read()

text = text.replace('strcmp(ent->condition, "RED") == 0', 'ent->condition == COND_RED')
text = text.replace('strcmp(ent->condition, "YELLOW") == 0', 'ent->condition == COND_YELLOW')
text = text.replace('snprintf(buf, sizeof(buf), "CONDITION: %s", ent->condition)', 'snprintf(buf, sizeof(buf), "CONDITION: %s", ent->condition == COND_RED ? "RED" : (ent->condition == COND_YELLOW ? "YELLOW" : "GREEN"))')

text = text.replace('Quadrant* q = Galaxy_GetQuadrant(checkQx, checkQy);', 'Quadrant* q = Galaxy_GetQuadrant(galaxy, checkQx, checkQy);')
text = text.replace('Quadrant* q = Galaxy_GetQuadrant(qx, qy);', 'Quadrant* q = Galaxy_GetQuadrant(galaxy, qx, qy);')
text = text.replace('KlingonGroup* e = Galaxy_GetEnemy(s->entity_id);', 'KlingonGroup* e = Galaxy_GetEnemy(galaxy, s->entity_id);')

with open("src/game/dashboard.c", "w") as f:
    f.write(text)

