import re

with open("src/video/test_video.c", "r") as f:
    data = f.read()

# I will replace the main function
old_version = """void DrawMockupDashboard(ScanMode mode) {
    
    
    

    Dashboard_DrawTemplate();
    Dashboard_DrawEnterpriseStats(&ent);
    Dashboard_DrawScan(mode, &ent);
}"""

new_version = """void DrawMockupDashboard(ScanMode mode, GalaxyState* galaxy, const char* input_buffer, int cursor_blink) {
    if (mode == SCAN_MODE_MAP) {
        Dashboard_DrawGalaxyMap(galaxy, &ent);
    } else {
        Dashboard_DrawFrame();
        Dashboard_DrawEnterpriseStats(&ent);
        if (mode == SCAN_MODE_SRS) Dashboard_DrawSRS(galaxy, &ent);
        else if (mode == SCAN_MODE_LRS) Dashboard_DrawLRS(galaxy, &ent);
    }
    Dashboard_DrawPrompt(input_buffer, cursor_blink);
    Dashboard_DrawLogs();
}"""

data = data.replace(old_version, new_version)

# Wait, the signature in FillStressTest probably had a call too. We need to fix the call loop in FillStressTest
# Let me just check what the old call was.

with open("src/video/test_video.c", "w") as f:
    f.write(data)
