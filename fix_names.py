with open("src/game/dashboard.c", "r") as f:
    text = f.read()

text = text.replace("void Dashboard_DrawTemplate(void)", "void Dashboard_DrawFrame(void)")

# Split DrawScan into DrawLRS and DrawSRS
# We should probably just provide DrawLRS and DrawSRS
