# VHS — A Última Sessão

Jogo de terror desenvolvido em C99 com Raylib, com código e interface em português.

## Executar no WSL

Na raiz do projeto, execute:

```sh
make run
```

Para somente compilar, use:

```sh
make
```

A Raylib deve estar instalada em `$HOME/.local`. Para utilizar outra instalação:

```sh
make RAYLIB=/caminho/da/raylib
```

## Controles

| Tecla | Ação |
|---|---|
| W, A, S, D / Setas | Movimentar John |
| E | Interagir com objetos e cenários |
| Z / X | Selecionar item no inventário |
| Roda do mouse | Alternar item selecionado |
| F | Usar o kit médico selecionado |
| Q | Atalho para usar o kit médico |
| Esc | Abrir o menu de pausa durante a exploração |
| F1 | Mostrar ou ocultar as colisões |

No menu de pausa, use as setas e Enter para navegar. Esc seleciona Continuar. Ao escolher voltar ao menu principal, o jogo solicita confirmação.

Nas interfaces do aparelho VHS e do teclado, Esc fecha a interface atual. Nas telas secundárias do menu principal, Esc retorna à tela anterior.

As telas de vitória e derrota oferecem opções navegáveis pelas setas e confirmadas com Enter.

## Objetivo

Explore a Videolocadora Aurora, encontre as três fitas VHS, reconstrua a transmissão no aparelho VHS e descubra o código necessário para liberar a saída.

Sobreviva ao Rebobinador e escape da locadora.

## Estrutura dos arquivos

- `src/`: código-fonte C e arquivos de cabeçalho.
- `assets/`: imagens, cenários, personagens e sons utilizados no jogo.
- `build/vhs`: executável gerado pelo Makefile.
