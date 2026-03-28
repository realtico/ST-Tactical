with open("src/video/test_video.c", "r") as f:
    data = f.read()

# reposition terminal logs, map, and prompts down even more
data = data.replace('Terminal_DrawText(1, 14, "COMMAND? > _", C_WHITE, C_BLACK, 0);', 'Terminal_DrawText(1, 19, "COMMAND? > _", C_WHITE, C_BLACK, 0);')
data = data.replace('Terminal_Print(3, 19, input_buffer);', 'Terminal_Print(3, 19, input_buffer);') 

with open("src/video/test_video.c", "w") as f:
    f.write(data)

with open("src/game/dashboard.c", "r") as f:
    data2 = f.read()

data2 = data2.replace('void Dashboard_DrawPrompt', 'void Dashboard_DrawPrompt')
data2 = data2.replace('DrawTextColor(1, 21+i', 'DrawTextColor(1, 20+i') 

with open("src/game/dashboard.c", "w") as f:
    f.write(data2)

