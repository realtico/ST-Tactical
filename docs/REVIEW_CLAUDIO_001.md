# 📋 Code Review #001 — Cláudio (Ombudsman)

**Projeto:** ST-Tactical  
**Escopo:** Card 0.0 (Zenit-Video Pack) + Card 0.1 (LRS/SRS/Câmera)  
**Revisado em:** 25/03/2026  
**Autor do código:** Polux (Gemini)  
**Arquitetura:** Castor (Gemini)  

---

## 1. O BOM (Pontos Fortes) ✅

### 1.1 Arquitetura Zero-Malloc para o Domínio de Jogo
A decisão de usar pools estáticos (`enemy_pool[MAX_ENEMIES]`, `galaxy_map[12][12]`) é acertadíssima para um jogo deste tipo. Elimina fragmentação de heap, torna o estado do jogo serializável trivialmente e evita toda uma classe de bugs de lifetime. O estilo "Angband-like" mencionado nos comentários é honrado de verdade.

### 1.2 Geração Procedural Determinística
O uso de uma LCG customizada (`Galaxy_SRand`/`Galaxy_Rand`) desacoplada da `rand()` da libc é uma decisão madura. Garante reprodutibilidade cross-platform, o que é essencial para debug e para um eventual modo replay/seed-sharing. A variação ±5% no HP dos inimigos é um toque elegante de variabilidade controlada.

### 1.3 Gradiente Geográfico de Perigo (Gamma)
O sistema `gamma = (qx + (11 - qy)) / 22.0` que escala a dificuldade do Sudoeste (base segura) ao Nordeste (zona de guerra) é simples, legível e eficaz. A interação com os pesos do bestiário (B'rel diminui, Vor'cha sobe) cria um gradiente emergente muito bom sem complexidade desnecessária.

### 1.4 Double-Buffering da VRAM
A simulação de hardware de vídeo com `VRAM_Front`/`VRAM_Back` e `SwapBuffers` é uma abstração limpa. Impede artefatos de tearing na renderização e separa claramente a fase de escrita (lógica) da fase de leitura (render). Boa engenharia.

### 1.5 Renderização Geométrica de Box-Drawing
A interceptação dos codepoints Unicode de Box-Drawing (`0x2500–0x257F`) para desenhar retângulos pixel-perfect em vez de usar a fonte é brilhante. Resolve definitivamente o problema clássico de "gaps" entre caracteres de grade em terminais emulados. O suporte adicional a linhas duplas (`0x2550+`) para demarcar fronteiras de quadrante no SRS demonstra atenção ao detalhe visual.

### 1.6 Build System Cross-Platform
CMake com FetchContent para o Raylib é a abordagem correta. Remove a dependência de pacotes do sistema, garante a mesma versão da lib em todos os ambientes, e o `Makefile` wrapper de conveniência (`make run`) é um bom toque de DX.

### 1.7 Separação de Camadas
A divisão `video/terminal` ↔ `game/dashboard` ↔ `game/galaxy` respeita o princípio de que o terminal não sabe nada do jogo e o jogo não sabe nada do Raylib. O `terminal.c` é um driver puro; o `dashboard.c` é uma camada de apresentação; o `galaxy.c` é domínio puro. Isso está correto.

---

## 2. O RUIM (Pontos Fracos) ⚠️

### 2.1 Estado Global Estático Extensivo
Embora pools estáticos sejam bons, **todo** o estado do jogo vive em `static` file-scope dentro de `galaxy.c` e `terminal.c`. Isso cria os seguintes problemas:
- **Testabilidade:** Impossível criar duas galáxias simultaneamente (ex: para testes comparativos ou para um futuro modo de campanha com múltiplas instâncias).
- **Reentrância:** Nenhuma função é reentrante. Se no futuro houver qualquer forma de threading (ex: IA rodando em background), isso será uma bomba.
- **Acoplamento oculto:** `Dashboard_DrawScan` tem `static int camX, camY` — estado persistente escondido dentro de uma função de rendering. Isso dificulta reset de câmera e viola o princípio de que funções de draw devem ser puras.

**Sugestão:** Considerar agrupar o estado em structs explícitas (`GalaxyState`, `TerminalState` — esta última já existe mas é estática) e passar via ponteiro. Não precisa ser agora, mas o desenho atual já deveria ter isso como diretriz para os próximos cards.

### 2.2 `Dashboard_DrawScan` — Complexidade Ciclomática Alta
Essa função tem ~180 linhas com dois caminhos radicalmente diferentes (LRS vs SRS) dentro de um mesmo `if/else`. Cada ramo é essencialmente uma função independente. Deveria ser dividida em `Dashboard_DrawLRS()` e `Dashboard_DrawSRS()` com `Dashboard_DrawScan` sendo mera dispatch.

### 2.3 Magic Numbers por Toda Parte
O `dashboard.c` está repleto de coordenadas literais hardcoded:
- `DrawTextColor(54, 3, ...)`, `DrawTextColor(56, 8, ...)`, `Terminal_SetCell(sX+32, ...)`, etc.
- `int sX = 27;` (LRS), `int sX = 19;` (SRS)
- `for(int x=53; x<=78; x++)`

Essas coordenadas refletem um layout fixo e foram claramente ajustadas manualmente por tentativa e erro ("deu trabalho", como o Hardy disse). O problema é que **qualquer** mudança de layout exige caçar dezenas de literais espalhados. Constantes nomeadas ou ao menos um bloco de `#define` no topo do arquivo com as âncoras do layout ajudariam enormemente.

### 2.4 `Enterprise` como Struct Burra sem Lógica
A `Enterprise` é uma struct em `enterprise.h` sem **nenhuma** função associada. O `test_video.c` manipula seus campos diretamente com `memset` + atribuições literais. Isso funciona agora, mas conforme a Enterprise ganhar mecânicas (dano, reparo, consumo de energia, warp), essa struct precisará de uma API (`Enterprise_Init`, `Enterprise_TakeDamage`, `Enterprise_Move`, etc.) ou a lógica vazará por todo o codebase.

### 2.5 `condition` como String em vez de Enum
```c
char condition[10]; // "GREEN", "YELLOW", "RED"
```
Comparação com `strcmp` no Dashboard é frágil e desperdiça ciclos. Deveria ser um enum (`CONDITION_GREEN`, `CONDITION_YELLOW`, `CONDITION_RED`) com conversão para string apenas na camada de apresentação.

### 2.6 Ausência Total de Testes Automatizados
O `test_galaxy.c` é um dumper standalone, não um teste. O `test_video.c` é o entry-point do app. Não há nenhum teste unitário real. Para um sistema de geração procedural determinística, testes de snapshot (gerar com seed X, comparar com resultado esperado) seriam triviais e valiosíssimos.

### 2.7 `Dashboard_DrawLogs` Declarado mas Não Implementado
O header `dashboard.h` declara `void Dashboard_DrawLogs(const char* logs[], int log_count);` mas não há implementação em `dashboard.c`. Isso causará erro de linker se alguém tentar usá-la. Deve ou ser implementada ou removida do header.

### 2.8 Falta de Bounds-Check no `KlingonBestiary`
```c
const ShipClassDef* def = &KlingonBestiary[class_id];
```
Se `class_id >= NumKlingonClasses`, isso é acesso fora dos limites (undefined behavior). `Galaxy_SpawnEnemy` recebe `class_id` como `uint8_t` sem validação. O mesmo ocorre em `Galaxy_DumpToLog` ao acessar `KlingonBestiary[e->class_id]`.

### 2.9 Inconsistência no Manual vs Código
O `MANUAL.md` diz:
> "Mudou-se de `TEXTURE_FILTER_POINT` para `TEXTURE_FILTER_BILINEAR`"

Mas o código em `terminal.c` usa `TEXTURE_FILTER_POINT` em múltiplos lugares. A documentação está desatualizada ou reflete uma decisão revertida que não foi sincronizada. Isso gera confusão para quem lê o manual primeiro.

---

## 3. O FEIO (Problemas Técnicos Sérios) 🔴

### 3.1 LCG Fraca — Distribuição Não-Uniforme Potencial
```c
static uint32_t Galaxy_Rand(void) {
    seed_state = (seed_state * 1103515245U + 12345U);
    return (seed_state >> 16) & 0x7FFF;
}
```
- Retorna no máximo 15 bits (0–32767). Quando usado com `% 100`, `% 9`, `% 3`, etc., a distorção é mínima, mas quando usado em `% total_weight` onde `total_weight` pode ser ~270, o viés modular começa a ser perceptível.
- Para procedural generation isto pode ser aceitável, mas convém documentar essa limitação expliciramente ou considerar subir para uma PRNG de 32 bits de saída (xorshift32 seria drop-in e trivial).

### 3.2 Loop Infinito Potencial na Colocação de Entidades
```c
do {
    sx = Galaxy_Rand() % SECTOR_GRID_SIZE;
    sy = Galaxy_Rand() % SECTOR_GRID_SIZE;
} while (q->sectors[sy][sx].entity_type != ENTITY_EMPTY);
```
Se um quadrante ficasse lotado (8×8 = 64 setores, com 9 estrelas + 1 base + até ~4 inimigos = ~14 entidades no máximo, então é improvável), este loop rodaria infinitamente. Contudo, o código **não garante** que o número de entidades nunca exceda 64 — o `e_count` pode ser 4 e haverá múltiplos quadrantes com estrelas. É um bug latente que deve ter ao menos um contador de tentativas ou um check `if slots_available > 0`.

### 3.3 Divisão por Zero no Progress Bar
```c
snprintf(buf, sizeof(buf), "(%d%%)", (int)(((float)ent->shields/(float)ent->shieldsMax)*100));
```
Se `shieldsMax == 0`, isto é divisão por zero em ponto flutuante. O `DrawProgressBar` tem `if (max <= 0) return;` — a proteção existe no bar, mas a string formatada logo abaixo dele não está protegida. O mesmo vale para torpedo e energia.

### 3.4 Possível Buffer Overflow em `strcat` do LRS/SRS Header
```c
char colHeader[40] = "";
for (int x = 0; x < 3; x++) {
    char buf[5]; snprintf(buf, sizeof(buf), "%02d  ", checkQx);
    strcat(colHeader, buf);
}
```
No LRS: 3 iterações × 4 chars = 12 chars + nul → OK (cabe em 40).

Mas no SRS:
```c
char colHeader[40] = "";
for(int x=0; x<8; x++) {
    char buf[5]; snprintf(buf, sizeof(buf), " %02d ", camX+x);
    strcat(colHeader, buf);
}
```
8 iterações × 4 chars = 32 chars + nul = 33 → cabe em 40, **mas por pouca margem**. Se alguém mudar o formato para 5 chars por coluna, estoura. Considerar usar `snprintf` acumulativo com offset em vez de `strcat` repetido.

### 3.5 Coordenada Invertida: `condition` via `strcat` (Menor mas Sujo)
O `condition` da Enterprise é um array empilhado de 10 bytes. Em `test_video.c`:
```c
strcpy(ent.condition, "RED");
```
Funciona, mas sem proteção de tamanho. Se alguém colocar "DOCKING_BAY" como condição, é buffer overflow. Mais uma razão para converter para enum.

### 3.6 `Galaxy_DumpToLog` Escreve no Diretório de Trabalho
```c
FILE* f = fopen("galaxy_debug.txt", "w");
```
Escreve no CWD, que pode ou não ser o diretório do build. Em ambientes de CI/CD ou se o executável for chamado de outro diretório, o arquivo vai parar em lugar inesperado. Deveria ser relativo ao executável ou receber o path como parâmetro.

### 3.7 Renderizador de Box-Drawing Incompleto e Frágil
O bloco de matching de codepoints no `Terminal_Render` usa longas sequências de OR:
```c
bool left = (cell.glyph == 0x2500 || cell.glyph == 0x2510 || ...);
```
Cada codepoint novo exige adicionar manualmente em múltiplas linhas booleanas, com risco alto de omissão ou erro. Isso já é parcialmente visível: os codepoints `0x256A` e `0x256B` (╪ e ╫) aparecem nos testes de single/double mas não cobrem todos os casos mistos possíveis.

**Sugestão:** Usar uma lookup table ou bitfield indexed por `(glyph - 0x2500)` com flags para cada segmento (LEFT, RIGHT, UP, DOWN, LEFT_D, RIGHT_D, UP_D, DOWN_D). Isso eliminaria toda essa lógica booleana, tornaria a adição de novos caracteres trivial e também melhoraria performance por eliminar branch prediction misses.

### 3.8 Leak de Recursos no Path de Erro do `Terminal_Init`
```c
term.VRAM_Front = (VRAMCell *)malloc(cols * rows * sizeof(VRAMCell));
term.VRAM_Back = (VRAMCell *)malloc(cols * rows * sizeof(VRAMCell));
if (!term.VRAM_Front || !term.VRAM_Back) return false;
```
Se `VRAM_Front` alocar com sucesso mas `VRAM_Back` falhar, `VRAM_Front` vaza. A verificação deveria liberar o que foi alocado antes de retornar `false`.

---

## 4. Resumo Quantitativo

| Categoria | Contagem | Severidade |
|-----------|----------|------------|
| Pontos Fortes | 7 | — |
| Pontos Fracos | 9 | Média (refactoring planejável) |
| Pontos Feios | 8 | Alta (bugs latentes ou riscos) |

---

## 5. Recomendações Priorizadas para o Castor

1. **[URGENTE]** Corrigir leak no `Terminal_Init` e proteção de divisão por zero nas strings de stats. São one-liners.
2. **[URGENTE]** Adicionar guard de tentativas no loop de colocação de entidades em `Galaxy_Generate`.
3. **[IMPORTANTE]** Extrair `Dashboard_DrawLRS`/`Dashboard_DrawSRS` como funções separadas.
4. **[IMPORTANTE]** Converter `condition` para enum. Criar `Enterprise_Init()`.
5. **[PLANEJÁVEL]** Substituir magic numbers do layout por constantes nomeadas.
6. **[PLANEJÁVEL]** Lookup table para o renderizador de Box-Drawing.
7. **[PLANEJÁVEL]** Sincronizar `MANUAL.md` com o código real (TEXTURE_FILTER).
8. **[BACKLOG]** Migrar estado global para structs passáveis por ponteiro.
9. **[BACKLOG]** Implementar ou remover `Dashboard_DrawLogs` do header.
10. **[BACKLOG]** Considerar testes unitários de snapshot para geração procedural.

---

*Revisado por Cláudio (Claude Opus 4.6, Ombudsman do Projeto)*  
*"Código bom é código que o próximo dev entende sem precisar perguntar."*
