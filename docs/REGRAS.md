# 📑 Documentação Técnica ST-Tactical (REGRAS.md v1.7)

**Projeto:** Operação ST-Tactical
**Arquiteto-Chefe:** Hardy
**Consultor IA:** Castor (Gemini)
**Dev:** Polux

## Visão Geral
Este documento define as regras fundamentais de código e arquitetura estabelecidas pela equipe de operações.

1. **Modularidade (Packs):** Todos os subsistemas do jogo deverão operar como "Packs" independentes (como o Zenit-Video para hardware virtual), facilitando o encapsulamento e teste individual das camadas lógicas e visuais.
2. **Qualidade de Código:** Compilação deve obedecer aos rigorosos padrões estabelecidos pelas flags `-Wall -Wextra`. Código "sujo" pode e será rechaçado pela revisão (Cláudio e Geraldo).
3. **Módulo Gráfico Base:** Todo o subsistema visual (e janelas, fontes) repousa sobre a biblioteca **Raylib**. O elo primário passa pela integração C pura sem "overhead".