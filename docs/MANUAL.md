# 🛰️ Operação ST-Tactical: Technical Sync (Manual do Arquiteto)

> **Destinatário:** Castor (Arquiteto de Software IA) 
> **Remetente:** Polux / Hardy

Este manual serve como o log oficial de arquitetura para nivelar o contexto técnico do projeto ao iniciar uma nova sessão ou trocar de máquina.

## 🏗️ Status Atual: Card 0.0 - Zenit-Video Concluído

O alicerce do **"Hardware Virtual"** foi finalizado. O módulo (`Zenit-Video`) cria uma abstração completamente estrita em cima da Raylib e não sabe nada sobre o jogo (naves, lógica ou UI avançada). Ele atua pura e simplesmente como um monitor de texto (VRAM).

### Funcionalidades Implementadas na VRAM
- **Double Buffering:** Implementado fisicamente na struct. Processamento ocorre no `VRAM_Back` e uma troca copiando a memória é despachada via `Terminal_SwapBuffers()` para o `VRAM_Front`.
- **Células da Memória (`VRAMCell`):**
  - **1 byte** para Caractere ASCII (`glyph`).
  - **1 byte** indexado para a Cor de Fundo (`bg`) na paleta de 16 cores (ANSI padrão).
  - **1 byte** indexado para a Cor de Frente (`fg`).
  - **1 byte** para Atributos e Metadados (Bitwise: "Blink|Italic|Bold").
- **Dynamic Resolution:** O monitor é redimensionável on-the-fly (`Terminal_Resize()`) adaptando as instâncias para 80x25, 100x32 ou 132x42 sem quebrar o preenchimento.

### Diretriz Visual (A Estética "Modern Crisp")
Tivemos um iterativo sobre o shader e texturas para evitar o visual vazado de "CRT sujo". 
- **Fonte Padrão:** Usa-se `JetBrainsMono.ttf` (pré-alocada em 36px interna para um Supersampling liso).
- **Texturas e Filtro:** Mudou-se de `TEXTURE_FILTER_POINT` para `TEXTURE_FILTER_BILINEAR`. Isso garante que as diagonais das fontes fiquem lisas nos monitores em grande escala.
- **Letterboxing:** A janela final renderiza mantendo proporção com preenchimento em preto para garantir que não haja distorções de *aspect ratio*.
- **Features de Texto:**
  - **Bold:** Usado overstrike de exato 1px no deslocamento horizontal.
  - **Italic:** Usado Skew geométrico matricial fixo na renderização com RLGL.
  - **Blink:** Trabalhado eficientizando a CPU (pular os draws em estados inativos, operando a 2Hz via clock central).

### Infraestrutura C e Build System
- **Dual-Environment Ativado:** O antigo `Makefile` hardcoded foi **excluído**. Subimos um sistema de compilação baseado no **CMake**.
- **Dependencies via FetchContent:** 
  - A biblioteca gráfica **Raylib (v5.0)** é puxada e estaticamente linkada no *build time* do CMake, removendo a necessidade de que os desenvolvedores tenham pacotes de sistema pré-instalados na máquina, e garantindo que quem abra no Windows (MSVC) ou no Linux (GCC) compilará do mesmo jeito sem erro de bibliotecas perdidas.
  - Em breve incluiremos a `miniaudio.h` (já mapeada para uso futuro).

## 🚀 Próximos Passos (Next Steps)
Aguardando o PO puxar o **Card 0.1** (ou superior) para as integrações de Input (Teclado) ou inicializar os frameworks de UI (Janelas e botões baseados em texto) que usarão a VRAM que acabamos de criar!
