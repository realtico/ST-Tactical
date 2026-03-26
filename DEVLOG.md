# DEVLOG: Progress & Handoff Notes

## Sessão Atual (25/03/2026): Polimento do Mapa / LRS & SRS
Nesta iteração sincronizamos o Dashboard com a lógica de navegação linear estrita do mapa global (96x96), trazendo a câmera do SRS à vida e refinando os painéis de radar.

### Implementações Concluídas:
1. **Malha de Coordenadas Globais (Grid Absoluto)**:
   - Os setores deixaram de sofrer wrap modular local (0-7). A mecânica é **puramente linear** onde `x` e `y` definem o setor de 0 a 95. 
   - A navegação pelas setas já respeita os limites absolutos da galáxia.
   - O Quadrante atual não dita mais estado, atuando apenas como um cálculo derivado (`x / 8`, `y / 8`).

2. **Sensores LRS e SRS**:
   - O Long Range Scan (LRS) deixou de ser estático (mostrando sempre 00-02) e agora mostra sempre uma janela relativa **3x3** ao redor do quadrante geográfico no qual a Enterprise (`<E>`) está posicionada. Adicionado suporte estrutural a vazios quando na beirada do grid.
   - O Short Range Scan (SRS) agora age como uma verdadeira câmera contínua 8x8 ancorada no centroide da entidade, navegando livremente sobre o espaço e realizando cortes das bordas em final de mapa.

3. **Polimento UI e Instanciação Gráfica Direta**:
   - Paleta da grade tática interna com redução de contraste original (convertida para Cinza Escuro `C_DGRAY`) a fim de não criar poluição visual excessiva.
   - O Tracker visual da nossa câmera de SRS ganhou um update estético para demarcar as fronteiras dos Quadrantes enquanto navegamos. Os divisores modulares (`% 8`) utilizam Box-Drawing characters duplos (`║`, `═`, `╬`, `╪`, `╫`).
   - Sinergia do Renderizador: O módulo de interceptação de geometria (`Terminal_Render` em `terminal.c`) que bloqueava as quebras comuns de fonte agora abrange o kit de caracteres duplos Unicode (`0x2550`, `0x2551`, `0x256C`, etc). Eles são desenhados diretamente pelo motor via retângulos geométricos, tornando as linhas perfeitamente unidas pixel a pixel de ponta a ponta sem o "bleeding effect" causado pela tipografia.

---

## Sessão Original (24/03): Base do Engine
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
2. **Prompt de Comando e Logs**: 
   - Ler buffer de teclado, exibir na linha "COMMAND? >" de baixo, e fazer feed ascendente limitando-se ao seu offset do grid.

*Data: 24/03/2026*
