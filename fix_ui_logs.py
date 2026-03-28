with open("src/game/dashboard.c", "r") as f:
    text = f.read()

text = text.replace("int logY = 20;\n    int display_count = 4;", "int logY = 20;\n    int display_count = 5;")
text = text.replace("Terminal_SetCell(0, 24, B_V, C_DGRAY, C_BLACK, 0);", "")
text = text.replace("DrawTextColor(2, 24, \"COMMAND? > \", C_CYAN, C_BLACK, 0);", "DrawTextColor(2, 19, \"COMMAND? > \", C_CYAN, C_BLACK, 0);")
text = text.replace("DrawTextColor(13, 24, input_buffer, C_WHITE, C_BLACK, 0);", "DrawTextColor(13, 19, input_buffer, C_WHITE, C_BLACK, 0);")
text = text.replace("Terminal_SetCell(13 + len, 24,", "Terminal_SetCell(13 + len, 19,")

with open("src/game/dashboard.c", "w") as f:
    f.write(text)
