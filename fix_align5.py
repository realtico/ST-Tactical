with open("src/game/dashboard.c", "r") as f:
    data = f.read()

data = data.replace('void Dashboard_AddLog(const char* format, ...) {\n    log_count++;\n}\n\nvoid Dashboard_DrawPrompt(const char* input_buffer, int cursor_blink) {\n    // placeholder\n}\n\nvoid Dashboard_DrawLogs(void) {\n', 'void Dashboard_AddLog(const char* format, ...) {\n    log_count++;\n}\n\nvoid Dashboard_DrawPrompt(const char* input_buffer, int cursor_blink) {\n    // placeholder\n}\n\nvoid Dashboard_DrawLogs(void) {\n    for (int i = 0; i < 3; i++) {\n        DrawTextColor(1, 21+i, logs[i], C_WHITE, C_BLACK, 0);\n    }\n}\n')

with open("src/game/dashboard.c", "w") as f:
    f.write(data)

