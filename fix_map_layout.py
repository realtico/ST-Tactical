import re

with open("src/game/dashboard.c", "r") as f:
    text = f.read()

# Fix the map
map_impl = """
void Dashboard_DrawGalaxyMap(GalaxyState* galaxy, const Enterprise* ent) {
    int sX = 16; 
    int sY = 2;
    DrawTextColor(16, 1, "          GALACTIC CHART            ", C_WHITE, C_BLACK, 0);

    // Headers (12 * 3 = 36 chars)
    char colHeader[40] = "";
    for(int x = 0; x < 12; x++) {
        char buf[5];
        snprintf(buf, sizeof(buf), "%02d ", x);
        strcat(colHeader, buf);
    }
    DrawTextColor(sX + 1, sY, colHeader, C_WHITE, C_BLACK, 0);
    
    for (int y = 0; y < 12; y++) {
        // Draw row headers inside the left panel area without breaking it completely? No, let's keep them in the center panel. 
        // Actually, we can draw the row header at the start of the row, but that takes 3 chars. 
        // Wait, W=37. 3 + 12*3 = 3 + 36 = 39. Too big! 
        // What if column headers and data don't have spaces? "0001020304" etc? That's 24 chars. 
    }
}
"""

