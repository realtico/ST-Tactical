with open("src/video/test_video.c", "r") as f:
    text = f.read()

text = text.replace("Dashboard_DrawLRS(&galaxy, &galaxy, &ent);", "Dashboard_DrawLRS(&galaxy, &ent);")
text = text.replace("Dashboard_DrawSRS(&galaxy, &galaxy, &ent);", "Dashboard_DrawSRS(&galaxy, &ent);")
text = text.replace("void DrawMockupDashboard(void) {", "void DrawMockupDashboard(GalaxyState* galaxy) {\n")
text = text.replace("void DrawMockupDashboard(GalaxyState* galaxy) {\n    GalaxyState galaxy;", "void DrawMockupDashboard(GalaxyState* ptr) {") # in case I mess up
text = text.replace("DrawMockupDashboard();", "DrawMockupDashboard(&galaxy);")

with open("src/video/test_video.c", "w") as f:
    f.write(text)
