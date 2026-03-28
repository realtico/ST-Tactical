with open("src/video/test_video.c", "r") as f:
    data = f.read()

old_loop = """        static ScanMode currentMockupMode = SCAN_MODE_LRS;
        if (IsKeyPressed(KEY_SPACE)) {
            currentMockupMode = (currentMockupMode == SCAN_MODE_LRS) ? SCAN_MODE_SRS : SCAN_MODE_LRS;
        }
        
        Terminal_Clear(0);
        DrawMockupDashboard(currentMockupMode);"""

new_loop = """        static ScanMode currentMockupMode = SCAN_MODE_LRS;
        if (IsKeyPressed(KEY_SPACE)) {
            currentMockupMode = (currentMockupMode == SCAN_MODE_LRS) ? SCAN_MODE_SRS : SCAN_MODE_LRS;
        }
        if (IsKeyPressed(KEY_M)) currentMockupMode = SCAN_MODE_MAP;
        
        static char input_buf[128] = {0};
        static int cursor_timer = 0;
        cursor_timer++;
        
        Terminal_Clear(0);
        DrawMockupDashboard(currentMockupMode, &galaxy, input_buf, (cursor_timer/30)%2);"""

data = data.replace(old_loop, new_loop)

# Also I need to ensure galaxy is declared in FillStressTest. Let's see if it is.
with open("src/video/test_video.c", "w") as f:
    f.write(data)
