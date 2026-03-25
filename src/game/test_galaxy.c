#include <stdio.h>
#include <stdlib.h>
#include "game/galaxy.h"

int main(int argc, char* argv[]) {
    uint32_t seed = 42;
    if (argc > 1) {
        seed = (uint32_t)atoi(argv[1]);
    }
    
    printf("Gerando Galaxia ST-Tactical com seed %u...\n", seed);
    Galaxy_Generate(seed); printf("Geracao terminada.\n");
    
    printf("Gerando dump ASCII em 'galaxy_debug.txt'...\n");
    Galaxy_DumpToLog();
    
    printf("Concluido!\n");
    return 0;
}
