#include "video/terminal.h"

#include <raylib.h>
#include <rlgl.h>
#include <stdlib.h>
#include <string.h>

// Terminal State
typedef struct {
    int cols;
    int rows;
    VRAMCell *VRAM_Front;
    VRAMCell *VRAM_Back;
    
    Font font;
    int charWidth;
    int charHeight;
    
    Color palette[MAX_COLORS];
    RenderTexture2D renderTarget;
} TerminalState;

static TerminalState term = {0};

bool Terminal_Init(int cols, int rows, const char *fontPath, int fontSize) {
    if (term.VRAM_Front != NULL) Terminal_Close();

    term.cols = cols;
    term.rows = rows;
    
    term.VRAM_Front = (VRAMCell *)malloc(cols * rows * sizeof(VRAMCell));
    term.VRAM_Back = (VRAMCell *)malloc(cols * rows * sizeof(VRAMCell));
    
    if (!term.VRAM_Front || !term.VRAM_Back) return false;

    memset(term.VRAM_Front, 0, cols * rows * sizeof(VRAMCell));
    memset(term.VRAM_Back, 0, cols * rows * sizeof(VRAMCell));

    // Montar mapa de codepoints para incluir Box Drawing (0x2500 - 0x257F) e Block Elements (0x2580 - 0x259F)
    int codepoints[512];
    int count = 0;
    for (int i = 32; i < 127; i++) codepoints[count++] = i;
    for (int i = 0; i < 256; i++) codepoints[count++] = 0x2500 + i; // Inclui Box e Block inteiro
    
    // Carregar fonte monoespaçada com nosso mapa
    term.font = LoadFontEx(fontPath, fontSize, codepoints, count);
    SetTextureFilter(term.font.texture, TEXTURE_FILTER_POINT); // Modern clear font

    // Calcular tamanho de cada caractere na tela (assumimos fonte mono)
    Vector2 mSize = MeasureTextEx(term.font, "M", fontSize, 0);
    term.charWidth = (int)mSize.x;
    term.charHeight = (int)mSize.y;

    // RenderTexture usando os tamanhos da VRAM * tam do char
    term.renderTarget = LoadRenderTexture(cols * term.charWidth, rows * term.charHeight);
    SetTextureFilter(term.renderTarget.texture, TEXTURE_FILTER_POINT); // Suavizar no upscaling

    // Paleta padrão retro (ANSI base + grayscale)
    for(int i = 0; i < MAX_COLORS; i++){
        term.palette[i] = RAYWHITE;
    }
    term.palette[0]  = BLACK;
    term.palette[1]  = RED;
    term.palette[2]  = GREEN;
    term.palette[3]  = YELLOW;
    term.palette[4]  = BLUE;
    term.palette[5]  = MAGENTA;
    term.palette[6]  = (Color){ 0, 255, 255, 255 }; // CYAN equivalente
    term.palette[7]  = LIGHTGRAY;
    term.palette[8]  = DARKGRAY;
    term.palette[9]  = (Color){255, 100, 100, 255}; // Bright Red
    term.palette[10] = (Color){100, 255, 100, 255}; // Bright Green
    term.palette[11] = (Color){255, 255,   0, 255}; // Bright Yellow
    term.palette[12] = (Color){100, 100, 255, 255}; // Bright Blue
    term.palette[13] = (Color){255, 100, 255, 255}; // Bright Magenta
    term.palette[14] = (Color){100, 255, 255, 255}; // Bright Cyan
    term.palette[15] = WHITE;

    return true;
}

void Terminal_Close(void) {
    if (term.VRAM_Front) free(term.VRAM_Front);
    if (term.VRAM_Back) free(term.VRAM_Back);
    
    UnloadRenderTexture(term.renderTarget);
    UnloadFont(term.font);

    term.VRAM_Front = NULL;
    term.VRAM_Back = NULL;
}

void Terminal_Resize(int cols, int rows) {
    if (cols == term.cols && rows == term.rows) return;
    
    // Novo buffer e cópia (básica) - descarta pixels extrapolados
    VRAMCell *newFront = (VRAMCell *)malloc(cols * rows * sizeof(VRAMCell));
    VRAMCell *newBack = (VRAMCell *)malloc(cols * rows * sizeof(VRAMCell));
    
    memset(newFront, 0, cols * rows * sizeof(VRAMCell));
    memset(newBack, 0, cols * rows * sizeof(VRAMCell));

    int minCols = (cols < term.cols) ? cols : term.cols;
    int minRows = (rows < term.rows) ? rows : term.rows;

    for (int y = 0; y < minRows; y++) {
        for (int x = 0; x < minCols; x++) {
            newFront[y * cols + x] = term.VRAM_Front[y * term.cols + x];
            newBack[y * cols + x]  = term.VRAM_Back[y * term.cols + x];
        }
    }

    free(term.VRAM_Front);
    free(term.VRAM_Back);
    
    term.cols = cols;
    term.rows = rows;
    term.VRAM_Front = newFront;
    term.VRAM_Back = newBack;

    UnloadRenderTexture(term.renderTarget);
    term.renderTarget = LoadRenderTexture(term.cols * term.charWidth, term.rows * term.charHeight);
    SetTextureFilter(term.renderTarget.texture, TEXTURE_FILTER_POINT);
}

void Terminal_SetPaletteColor(uint8_t index, uint32_t color_hex) {
    term.palette[index] = GetColor(color_hex);
}

void Terminal_Clear(uint8_t bg_color) {
    for (int i = 0; i < term.cols * term.rows; i++) {
        term.VRAM_Back[i].glyph = ' ';
        term.VRAM_Back[i].fg = 7;
        term.VRAM_Back[i].bg = bg_color;
        term.VRAM_Back[i].attr = 0;
    }
}

void Terminal_SetCell(int x, int y, int glyph, uint8_t fg, uint8_t bg, uint8_t attr) {
    if (x < 0 || x >= term.cols || y < 0 || y >= term.rows) return;
    int idx = y * term.cols + x;
    term.VRAM_Back[idx].glyph = glyph;
    term.VRAM_Back[idx].fg = fg;
    term.VRAM_Back[idx].bg = bg;
    term.VRAM_Back[idx].attr = attr;
}

VRAMCell Terminal_GetCell(int x, int y) {
    if (x < 0 || x >= term.cols || y < 0 || y >= term.rows) {
        VRAMCell empty = {0};
        return empty;
    }
    return term.VRAM_Back[y * term.cols + x];
}

void Terminal_SwapBuffers(void) {
    // Copiar conteúdos back no front - double-buffering real simulado
    memcpy(term.VRAM_Front, term.VRAM_Back, term.cols * term.rows * sizeof(VRAMCell));
}

void Terminal_Render(void) {
    double time = GetTime();
    bool blinkState = ((int)(time * 2.0)) % 2 == 0; // Alterna a cada 500ms (2Hz)

    BeginTextureMode(term.renderTarget);
    ClearBackground(term.palette[0]);

    for (int y = 0; y < term.rows; y++) {
        for (int x = 0; x < term.cols; x++) {
            VRAMCell cell = term.VRAM_Front[y * term.cols + x];
            
            // Background
            if (cell.bg != 0) {
                DrawRectangle(x * term.charWidth, y * term.charHeight, term.charWidth, term.charHeight, term.palette[cell.bg]);
            }

            // Foreground & Glyphs
            if (cell.glyph != ' ' && cell.glyph != 0) {
                // Blink
                if ((cell.attr & ATTR_BLINK) && !blinkState) {
                    continue; // Skip rendering text Se blink visível estiver off
                }

                // Skip char str array
                Vector2 pos = { (float)x * term.charWidth, (float)y * term.charHeight };
                Color fColor = term.palette[cell.fg];

                // Intercept Box Drawing and Full Block for pixel-perfect tile without gaps
                if ((cell.glyph >= 0x2500 && cell.glyph <= 0x257F) || cell.glyph == 0x2588) {
                    int cx = x * term.charWidth + term.charWidth / 2;
                    int cy = y * term.charHeight + term.charHeight / 2;
                    int px = x * term.charWidth;
                    int py = y * term.charHeight;
                    int cw = term.charWidth;
                    int ch = term.charHeight;
                    int lw = 2; // line thickness
                    int cw_half = term.charWidth / 2;
                    int ch_half = term.charHeight / 2;

                    if (cell.glyph == 0x2588) { // BLOCK_FULL
                        DrawRectangle(px, py, cw, ch, fColor);
                    } else {
                        // Horizontal spans
                        bool left  = (cell.glyph == 0x2500 || cell.glyph == 0x2510 || cell.glyph == 0x2518 || cell.glyph == 0x2524 || cell.glyph == 0x252C || cell.glyph == 0x2534 || cell.glyph == 0x253C);
                        bool right = (cell.glyph == 0x2500 || cell.glyph == 0x250C || cell.glyph == 0x2514 || cell.glyph == 0x251C || cell.glyph == 0x252C || cell.glyph == 0x2534 || cell.glyph == 0x253C);
                        // Vertical spans
                        bool up    = (cell.glyph == 0x2502 || cell.glyph == 0x2514 || cell.glyph == 0x2518 || cell.glyph == 0x251C || cell.glyph == 0x2524 || cell.glyph == 0x2534 || cell.glyph == 0x253C);
                        bool down  = (cell.glyph == 0x2502 || cell.glyph == 0x250C || cell.glyph == 0x2510 || cell.glyph == 0x251C || cell.glyph == 0x2524 || cell.glyph == 0x252C || cell.glyph == 0x253C);

                        if (left)  DrawRectangle(px, cy - 1, cw_half, lw, fColor);
                        if (right) DrawRectangle(px + cw_half, cy - 1, cw - cw_half, lw, fColor);
                        if (up)    DrawRectangle(cx - 1, py, lw, ch_half, fColor);
                        if (down)  DrawRectangle(cx - 1, py + ch_half, lw, ch - ch_half, fColor);
                    }
                    continue; // Skip font renderer for these
                }

                // Skew per Italic via transform local
                bool isItalic = (cell.attr & ATTR_ITALIC) != 0;
                
                if (isItalic) {
                    rlPushMatrix();
                    rlTranslatef(pos.x, pos.y, 0.0f);
                    
                    // Emulação de matrix Skew(X) no OpenGL/Raylib
                    // | 1   a   0 |onde a é o fator de skew
                    // | 0   1   0 |
                    // | 0   0   1 |
                    // Usando glMatrix format 4x4 (col max major)
                    // [ 1, 0, 0, 0, 
                    //   a, 1, 0, 0, ...]
                    // Um leve inclinamento fixo, offset a.
                    float a = -0.2f;
                    float m[16] = {
                        1.0f, 0.0f, 0.0f, 0.0f,
                        a,    1.0f, 0.0f, 0.0f,
                        0.0f, 0.0f, 1.0f, 0.0f,
                        0.0f, 0.0f, 0.0f, 1.0f
                    };
                    rlMultMatrixf(m);
                    
                    Vector2 localPos = { (term.charHeight*0.2f), 0.0f }; // ajustar baseline se precisar
                    DrawTextCodepoint(term.font, cell.glyph, localPos, term.font.baseSize, fColor);
                    
                    if (cell.attr & ATTR_BOLD) {
                        Vector2 localPosB = { localPos.x + 1.0f, localPos.y };
                        DrawTextCodepoint(term.font, cell.glyph, localPosB, term.font.baseSize, fColor);
                    }
                    
                    rlPopMatrix();
                } else {
                    // Normal text
                    DrawTextCodepoint(term.font, cell.glyph, pos, term.font.baseSize, fColor);
                    // Bold (Overstrike 1px X)
                    if (cell.attr & ATTR_BOLD) {
                        Vector2 posB = { pos.x + 1.0f, pos.y };
                        DrawTextCodepoint(term.font, cell.glyph, posB, term.font.baseSize, fColor);
                    }
                }
            }
        }
    }
    EndTextureMode();

    // Renderizando a Textura na Janela Principal fazendo Upscaling
    // Calcula um source e dest pra manter escala correta baseada no tamanho da janela Raylib vs Virtual
    Rectangle sourceRect = { 0.0f, 0.0f, (float)term.renderTarget.texture.width, -(float)term.renderTarget.texture.height };
    
    // Mantendo Proporção (Letterbox) para evitar distorção de aspect ratio (causa do efeito "bleed" de linhas irregulares)
    float scaleX = (float)GetScreenWidth() / term.renderTarget.texture.width;
    float scaleY = (float)GetScreenHeight() / term.renderTarget.texture.height;
    float scale = (scaleX < scaleY) ? scaleX : scaleY; // fit
    
    Rectangle destRect = { 
        (GetScreenWidth() - (term.renderTarget.texture.width * scale)) * 0.5f, 
        (GetScreenHeight() - (term.renderTarget.texture.height * scale)) * 0.5f, 
        term.renderTarget.texture.width * scale, 
        term.renderTarget.texture.height * scale 
    };
    Vector2 origin = { 0.0f, 0.0f };

    DrawTexturePro(term.renderTarget.texture, sourceRect, destRect, origin, 0.0f, WHITE);
}
