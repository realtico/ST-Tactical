import re

with open("src/game/dashboard.c", "r") as f:
    text = f.read()

# Add log buffers
log_buffer_codes = """
#include <stdarg.h>

#define MAX_LOG_MESSAGES 6
#define MAX_LOG_LENGTH 78
static char log_messages[MAX_LOG_MESSAGES][MAX_LOG_LENGTH];
static int log_head = 0;
static int log_count = 0;

void Dashboard_AddLog(const char* format, ...) {
    char buffer[MAX_LOG_LENGTH];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, MAX_LOG_LENGTH, args, format);
    va_end(args);

    strncpy(log_messages[log_head], buffer, MAX_LOG_LENGTH - 1);
    log_messages[log_head][MAX_LOG_LENGTH - 1] = '\\0';
    
    log_head = (log_head + 1) % MAX_LOG_MESSAGES;
    if (log_count < MAX_LOG_MESSAGES) log_count++;
}

void Dashboard_DrawLogs(void) {
    int logY = 20;
    int display_count = 4; // can only fit 4 rows in Y 20..23
    int start_idx = (log_count < display_count) ? 0 : log_count - display_count;
    
    for (int i = 0; i < display_count && i < log_count; i++) {
        int idx = (log_head - log_count + start_idx + i + MAX_LOG_MESSAGES) % MAX_LOG_MESSAGES;
        DrawTextColor(1, logY + i, log_messages[idx], C_DGRAY, C_BLACK, 0);
    }
}

void Dashboard_DrawPrompt(const char* input_buffer, int cursor_blink) {
    Terminal_SetCell(0, 24, B_V, C_DGRAY, C_BLACK, 0);
    DrawTextColor(2, 24, "COMMAND? > ", C_CYAN, C_BLACK, 0);
    DrawTextColor(13, 24, input_buffer, C_WHITE, C_BLACK, 0);
    if (cursor_blink) {
        int len = strlen(input_buffer);
        Terminal_SetCell(13 + len, 24, '_', C_WHITE, C_BLACK, ATTR_BLINK);
    }
}
"""

if "Dashboard_AddLog" not in text:
    # replace includes part to add stdarg safely
    text = text.replace('#include <string.h>', '#include <string.h>\n' + log_buffer_codes)
    # remove old CLI prompt from DrawFrame
    text = text.replace('DrawTextColor(0, 20, "COMMAND? > _", C_WHITE, C_BLACK, 0);', '')
    # fix right bottom border for the prompt/logs
    # we need border at right side
    # terminal is 80 width.
    pass

with open("src/game/dashboard.c", "w") as f:
    f.write(text)

