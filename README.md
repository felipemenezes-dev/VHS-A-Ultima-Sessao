# VHS — A Última Sessão

Jogo em C99 com Raylib, com código em português.

## Executar no WSL

Na pasta `/home/danilo/projetos/VHS-A-Ultima-Sessao`:

```sh
make executar
```

`make run` também funciona. Para somente compilar, use `make`.
A Raylib deve estar em `$HOME/.local`; para outra instalação,
use `make RAYLIB=/caminho/da/raylib`.

## Arquivos

- `src/`: código C e cabeçalhos do jogo.
- `assets/`: imagens atuais e imagens preparadas para as próximas etapas.
- `build/vhs`: executável gerado pelo Makefile.

WASD ou setas movimentam John. Tab abre e fecha o inventário.
Esc fecha o inventário ou encerra o jogo se ele já estiver fechado.
F1 mostra as colisões e F11 alterna a tela cheia.
