#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdint.h>
#include <stdbool.h>

// Attributes
#define ATTR_BOLD   (1 << 5)
#define ATTR_ITALIC (1 << 6)
#define ATTR_BLINK  (1 << 7)

// Common terminal palettes sizes
#define MAX_COLORS  256

typedef struct {
    char glyph;     // Caractere ASCII
    uint8_t fg;     // Índice de cor de frente
    uint8_t bg;     // Índice de cor de fundo
    uint8_t attr;   // Bitfield: [7:Blink | 6:Italic | 5:Bold | 0-4:Reservado]
} VRAMCell;

// Inicializa o terminal com a resolução dada, carregando a fonte especificada.
bool Terminal_Init(int cols, int rows, const char *fontPath, int fontSize);

// Libera os recursos do terminal
void Terminal_Close(void);

// Redimensiona a quantidade de colunas e linhas, mantendo o conteúdo se possível.
void Terminal_Resize(int cols, int rows);

// Limpa o buffer de back (escrita)
void Terminal_Clear(uint8_t bg_color);

// Modifica uma célula no VRAM_Back
void Terminal_SetCell(int x, int y, char glyph, uint8_t fg, uint8_t bg, uint8_t attr);

// Obtém uma célula no VRAM_Back
VRAMCell Terminal_GetCell(int x, int y);

// Realiza a troca dos buffers (Back -> Front)
void Terminal_SwapBuffers(void);

// Desenha a tela do Terminal (usando o VRAM_Front) na janela raylib, aplicando upscaling
void Terminal_Render(void);

// Função para definir o esquema de cores global
void Terminal_SetPaletteColor(uint8_t index, uint32_t color_hex);

#endif // TERMINAL_H
