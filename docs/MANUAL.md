# 🛰️ Operação ST-Tactical: Manual Técnico do Arquiteto

> **Destinatário:** Castor (Arquiteto de Software IA)  
> **Remetente:** Polux / Hardy

Este manual serve como log oficial de arquitetura para nivelar o contexto técnico do projeto.

---

## 🏗️ Status Atual: Card 1.x — Motor de Jogo Funcional

O projeto evoluiu do Card 0.0 (Zenit-Video) para um motor de jogo funcional com galáxia procedural, nave Enterprise, sistema de comandos por texto e dashboard completo em terminal virtual.

---

## 📁 Estrutura do Projeto

```
src/
  video/
    main.c          ← Entry point, game loop, input e renderização
    terminal.c      ← Motor do terminal virtual (VRAM 80x25, double buffer)
  game/
    dashboard.c     ← Toda a UI: frame, stats, SRS, LRS, mapa, logs, prompt
    commands.c      ← Motor de comandos: tokenização, dispatch, tabela de handlers
    enterprise.c    ← Inicialização e estado da Enterprise
    galaxy.c        ← Geração procedural da galáxia, sensores, entidades
include/
  video/
    terminal.h      ← API do terminal (VRAMCell, Init/Clear/SetCell/Render)
  game/
    dashboard.h     ← API do dashboard (ScanMode enum, Draw*, AddLog)
    commands.h      ← API de comandos (Init, Execute, GetScanMode, ShouldQuit)
    enterprise.h    ← Struct Enterprise, Condition enum
    galaxy.h        ← GalaxyState, Quadrant, Sector, KlingonGroup structs
```

---

## 🖥️ Terminal Virtual (Zenit-Video)

Motor de terminal baseado em Raylib com VRAM de células.

- **Double Buffering**: `VRAM_Back` (escrita) → `Terminal_SwapBuffers()` → `VRAM_Front` (render)
- **VRAMCell**: `int glyph` (32-bit Unicode codepoint), `uint8_t fg/bg/attr`
- **Fonte**: JetBrains Mono 36px, `TEXTURE_FILTER_POINT` (pixel-perfect)
- **Atributos**: Bold (overstrike 1px), Italic (skew matricial), Blink (2Hz clock)
- **Letterboxing**: Proporção mantida com preenchimento preto

---

## 🎮 Arquitetura do Game Loop (`main.c`)

```
InitWindow → Terminal_Init → Galaxy_Generate → Enterprise_Init → Commands_Init

while (!WindowShouldClose() && !Commands_ShouldQuit()):
    1. Input de movimento (arrow keys → sectX/sectY)
    2. Recalcular quadrante (sectX/8, sectY/8)
    3. Se mudou de quadrante → Galaxy_UpdateSensors
    4. Input de comandos (GetCharPressed → buffer → ENTER → Commands_Execute)
    5. Terminal_Clear → DrawDashboard → Terminal_SwapBuffers
    6. BeginDrawing → Terminal_Render → EndDrawing

Terminal_Close → CloseWindow
```

**Globals**: `Enterprise ent`, `GalaxyState galaxy` (estáticos em main.c).

---

## 🌌 Galáxia (`galaxy.c / galaxy.h`)

- **Grid**: 12×12 quadrantes, cada um com 8×8 setores = 96×96 setores totais
- **Geração**: `Galaxy_Generate(seed)` — procedural determinístico com seed
- **Entidades por setor**: EMPTY, STAR, BASE, ENEMY (via `entity_type`)
- **Inimigos**: Pool global `KlingonGroup[MAX]` com `class_id` (0=Bird, 1=Cruiser, 2=Commander)
- **Sensores**: `Galaxy_UpdateSensors(quadX, quadY, stardate, isLRS)` atualiza `last_scan_date[12][12]`
  - SRS: marca apenas o quadrante atual
  - LRS: marca quadrante atual + 8 adjacentes (3×3)

---

## 🚀 Enterprise (`enterprise.c / enterprise.h`)

```c
typedef struct {
    int sectX, sectY;          // posição global (0-95)
    int quadX, quadY;          // quadrante derivado (0-11)
    float stardate;
    int energy, energyMax;
    int shields, shieldsMax;
    int torpedoes, torpedoesMax;
    float sysWarp, sysPhaser, sysSensor, sysLRS, sysComputer; // 0.0-1.0
    Condition condition;       // GREEN, YELLOW, RED
} Enterprise;
```

---

## 📟 Sistemas de Comando (`commands.c / commands.h`)

Motor de comandos por texto baseado em tabela de callbacks.

### Fluxo
1. Jogador digita no prompt (max 32 chars)
2. ENTER → `Commands_Execute(input)`
3. Input convertido para maiúsculas (`toupper`) — **case insensitive**
4. Tokenizado com `strtok` em `argc/argv` (max 8 tokens)
5. Busca na tabela `CommandEntry[]` por nome
6. Handler `CmdHandler(argc, argv)` executado

### Comandos Disponíveis

| Comando | Descrição |
|---------|-----------|
| `SRS` | Short Range Scan — vista 8×8 setores centrada na Enterprise |
| `LRS` | Long Range Scan — grid 3×3 quadrantes ao redor da Enterprise |
| `MAP` | Galactic Chart — mapa completo 12×12 (overlay fullscreen) |
| `HELP` | Lista todos os comandos disponíveis |
| `QUIT` | Sair do jogo — pede confirmação (Y/N) |

### QUIT com confirmação
1. Jogador digita `QUIT` → log exibe "ARE YOU SURE? TYPE 'Y' TO CONFIRM."
2. Se próximo input for `Y` ou `YES` → `Commands_ShouldQuit()` retorna true → loop encerra
3. Qualquer outra resposta → cancela e exibe "QUIT CANCELLED."

---

## 🖼️ Dashboard (`dashboard.c / dashboard.h`)

### Layout 80×25

```
y=0..19: Frame principal (3 painéis)
  Col 0-15:  NAVIGATION (stardate, quadrant, sector, damage report)
  Col 16-52: CENTER (SRS grid / LRS grid / vazio conforme modo)
  Col 53-79: RESOURCES (condition, energy, shields, torpedoes, enemy status)
y=20..23: LOG (4 linhas visíveis do buffer circular de 6)
y=24:     PROMPT ("COMMAND? >" + input + cursor piscante)
```

### Modularização do Display

A renderização é **modular por camada**:

1. `Dashboard_DrawFrame()` — desenha a moldura (bordas box-drawing, títulos, separadores)
2. `Dashboard_DrawEnterpriseStats()` — preenche os painéis laterais com dados da Enterprise
3. `Dashboard_DrawSRS()` / `Dashboard_DrawLRS()` — renderiza **como overlay no painel central** (x=18..50, y=2..18)
4. `Dashboard_DrawGalaxyMap()` — overlay fullscreen (substitui tudo, sem DrawFrame)
5. `Dashboard_DrawPrompt()` — prompt com cursor piscante em y=24
6. `Dashboard_DrawLogs()` — 4 últimos logs em y=20..23

Essa separação permite:
- Desligar SRS/LRS quando sensores estiverem danificados (basta pular a chamada)
- Substituir o conteúdo do centro sem alterar a moldura
- MAP opera independente como overlay total

### Log System
- Buffer circular de 6 entradas (`logs[6][80]`)
- `Dashboard_AddLog(format, ...)` com `vsnprintf`
- Shift: logs[0] ← logs[1] ← ... ← logs[4] ← novo
- Exibe logs[2..5] em y=20..23 (4 mais recentes)
- Log mais recente em branco; anteriores em cinza

---

## 🔧 Build System

- **CMake** com `FetchContent` para Raylib 5.0 (build estático)
- **Flags**: `-Wall -Wextra` (obrigatório por REGRAS.md)
- **Build**: `cd build && cmake .. && make`
- **Executável**: `build/ST_Tactical`

### Sources
```cmake
add_executable(ST_Tactical
    src/video/main.c
    src/video/terminal.c
    src/game/dashboard.c
    src/game/commands.c
    src/game/enterprise.c
    src/game/galaxy.c
)
```

---

## 📐 Convenções de Código

- **Box Drawing**: Constantes Unicode (`B_TL` = 0x250C, `B_H` = 0x2500, etc.) via `Terminal_SetCell`
- **Middle Dot**: `DrawMiddleDots()` usa 0x00B7 diretamente (bypass encoding)
- **DrawTextColor**: Itera byte a byte com cast `(unsigned char)` para evitar sign-extension
- **Encoding**: VRAMCell.glyph é `int` (32-bit) — suporta codepoints Unicode completos
- **Paleta**: 16 cores ANSI padrão (C_WHITE=15, C_BLACK=0, C_GREEN=10, C_RED=9, etc.)

---

## 🚀 Próximos Passos

- Implementar sistema de combate (phasers, torpedos)
- Dano por subsistema afetando gameplay (sensor danificado → SRS escuro)
- Movimento warp entre quadrantes (com custo de energia)
- Starbases (dock, reparos, reabastecimento)
- Contagem de inimigos e condição de vitória/derrota
