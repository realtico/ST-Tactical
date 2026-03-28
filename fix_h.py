import re

with open("include/game/galaxy.h", "r") as f:
    text = f.read()

struct_insert = """    uint16_t enemies_count;

    // Memória de Sensores
    double last_scan_date[12][12];
    uint8_t last_known_klingons[12][12];
"""

text = text.replace("    uint16_t enemies_count;", struct_insert)

with open("include/game/galaxy.h", "w") as f:
    f.write(text)

