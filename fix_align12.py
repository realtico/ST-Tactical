with open("src/video/test_video.c", "r") as f:
    data = f.read()

# reposition terminal logs, map, and prompts down even more
data = data.replace('Terminal_DrawText(1, 14,', 'Terminal_DrawText(1, 20,')
data = data.replace('Terminal_Print(3, 19', 'Terminal_Print(2, 21') 
data = data.replace('Terminal_DrawText(1, 20, "COMMAND? > _",', 'Terminal_DrawText(1, 19, "COMMAND? > _",')

with open("src/video/test_video.c", "w") as f:
    f.write(data)

with open("src/game/dashboard.c", "r") as f:
    data2 = f.read()

data2 = data2.replace('DrawTextColor(0, 20, "COMMAND? > _", C_WHITE, C_BLACK, 0);', '') 
data2 = data2.replace('DrawTextColor(1, 20+i', 'DrawTextColor(1, 22+i') 

with open("src/game/dashboard.c", "w") as f:
    f.write(data2)

