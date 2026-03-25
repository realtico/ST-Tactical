# DEVLOG: Progress & Handoff Notes

## Onde Paramos (Sessão Atual)
O escopo principal desta iteração foi criar a base de "Hardware Virtual" (Zenit-Video Pack) e a lógica base da galáxia para um roguelike espacial estilo Super Star Trek, contudo muito mais dinâmico.

### Implementações Concluídas:
1. **Engine de Terminal (Raylib)**:
   - Resolução nativa simulando console 80x25 (escalável).
   - `VRAMCell` atualizado para int de 32-bits para suporte nativo a Unicode.
   - Texturas usando `TEXTURE_FILTER_POINT` ativado (sem subpixel bleeding / artefatos visuais).
   - Renderização geométrica customizada (`DrawRectangle`) interceptando os codepoints do Box Drawing para formar linhas sólidas e perfeitas na tela.
2. **Dashboard UI**:
   - Painéis independentes renderizados em C pura: LRS (Long Range Scan), SRS (Short Range Scan), Stats da Enterprise (Dano, Status, Navegação).
   - Correção do layout final do SRS para que ele seja uma caixa flutuante (`Y=2` ao `Y=18`) sem colidir graficamente com as bordas do painel lateral.
   - Área limpa para o `COMMAND? > _` isolada na parte inferior (linha 20 em diante).
3. **Mecânica Base de Galáxia (`galaxy.c`)**:
   - Geração de universo determinística (Seed + Simple LCG gen).
   - Galáxia configurada em Grid: Macro de 12x12 Quadrantes.
   - Geração de Entidades: Estrelas, Starbases e Inimigos baseados em grau de perigo geográfico (Gama).
   - Estrutura de Bestiário configurada (B'rel, K't'inga, Vor'cha).

## Aviso para o Castor (Próximos Passos Obrigatórios)
Caro Castor/Próximo Agente, **NÃO re-invente as bordas visuais**. O setup do painel base está fixo e validado ("Pixel Perfect").

O nosso foco para o próximo Card deverá ser:
1. **Física Contínua (Câmera do SRS)**: 
   - Ao invés de uma tela de SRS bloqueada por Quadrante, implementaremos coordenadas de espaço globais (Setor `0-95, 0-95` no tamanho 12x8).
   - A matemática da câmera vai descentralizar a nave (no LRS teremos sim a visão segmentada por quadrantes, no entanto o SRS operará de forma contínua, permitindo ver frações do "Quadrante 6,7" da borda caso estejamos na beirada do "6,6").
2. **Prompt de Comando e Logs**: 
   - Ler buffer de teclado, exibir na linha "COMMAND? >" de baixo, e fazer feed ascendente limitando-se ao seu offset do grid.

*Data: 24/03/2026*
