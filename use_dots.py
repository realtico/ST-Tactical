import re

with open("src/game/dashboard.c", "r") as f:
    text = f.read()

# Add B_DOT definition if not there
if "#define B_DOT" not in text:
    text = text.replace("#define B_X   0x253C", "#define B_X   0x253C \n#define B_DOT 0x00B7")

replacement = """
            char data[5] = "   ";
            uint8_t color = C_DGRAY;
            bool undiscovered = true;
            
            if (galaxy->last_scan_date[y][x] >= 0) {
                undiscovered = false;
                if (q && q->has_base) {
                    snprintf(data, sizeof(data), "%d%d%d", galaxy->last_known_klingons[y][x], 1, q->num_stars);
                    color = C_CYAN;
                } else if (age < 5.0) {
                    snprintf(data, sizeof(data), "%d%d%d", galaxy->last_known_klingons[y][x], 0, q ? q->num_stars : 0);
                    color = C_GREEN;
                } else if (age <= 15.0) {
                    snprintf(data, sizeof(data), "%d%d%d", galaxy->last_known_klingons[y][x], 0, q ? q->num_stars : 0);
                    color = C_YELLOW;
                } else {
                    snprintf(data, sizeof(data), "%d%d%d", galaxy->last_known_klingons[y][x], 0, q ? q->num_stars : 0);
                    color = C_BROWN;
                }
            }
            
            if (undiscovered) {
                Terminal_SetCell(sX+1 + x*4, rY, B_DOT, C_DGRAY, C_BLACK, 0);
                Terminal_SetCell(sX+2 + x*4, rY, B_DOT, C_DGRAY, C_BLACK, 0);
                Terminal_SetCell(sX+3 + x*4, rY, B_DOT, C_DGRAY, C_BLACK, 0);
            }
            else if (x == ent->quadX && y == ent->quadY) {
                // Highlight current quad
                Terminal_SetCell(sX+1 + x*4, rY, data[0], C_BLACK, color, 0);
                Terminal_SetCell(sX+2 + x*4, rY, data[1], C_BLACK, color, 0);
                Terminal_SetCell(sX+3 + x*4, rY, data[2], C_BLACK, color, 0);
            } else {
                DrawTextColor(sX+1 + x*4, rY, data, color, C_BLACK, 0);
            }
"""

# Find the block to replace
pattern = r"char data\[5\] = \"\.\.\.\";.*?DrawTextColor\(sX\+1 \+ x\*4, rY, data, color, C_BLACK, 0\);\n            \}"
text = re.sub(pattern, replacement.strip(), text, flags=re.DOTALL)

with open("src/game/dashboard.c", "w") as f:
    f.write(text)
