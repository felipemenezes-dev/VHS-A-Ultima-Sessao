# Compilar e executar no Ubuntu do WSL

Esta base usa C99 e raylib 5.5. O iniciador abre em janela por padrão.
No PowerShell, `wsl -d Ubuntu` entra na distribuição preparada para o projeto.

## Executar

No terminal Ubuntu, dentro da pasta do repositório:

```sh
./iniciar.sh
```

O script recompila quando o fonte ou o script de compilação são mais novos que
o executável. Para forçar a compilação:

```sh
./compilar.sh
./iniciar.sh
```

Para solicitar tela cheia: `./iniciar.sh --fullscreen`.
WASD ou setas movem John. **Nesta base Esc fecha o jogo**; a pausa prevista nas
regras será implementada em uma etapa posterior.

Os scripts entram na pasta correta antes de carregar as imagens. A compilação
gera `build/vhs`; o executável `vhs` original do repositório é preservado.

## Dependências em uma instalação nova

Esta preparação só é necessária quando GCC e raylib ainda não estiverem instalados.
O WSL precisa oferecer suporte gráfico (WSLg) para executar o jogo e o teste visual.

```sh
sudo apt-get update
sudo apt-get install -y build-essential git libasound2-dev libx11-dev \
  libxrandr-dev libxi-dev libgl1-mesa-dev libglu1-mesa-dev \
  libxcursor-dev libxinerama-dev

mkdir -p "$HOME/.local/src"
git clone --depth 1 --branch 5.5 https://github.com/raysan5/raylib.git \
  "$HOME/.local/src/raylib-5.5"
make -C "$HOME/.local/src/raylib-5.5/src" -j4 \
  PLATFORM=PLATFORM_DESKTOP RAYLIB_LIBTYPE=STATIC
install -Dm644 "$HOME/.local/src/raylib-5.5/src/libraylib.a" \
  "$HOME/.local/lib/libraylib.a"
install -Dm644 "$HOME/.local/src/raylib-5.5/src/raylib.h" \
  "$HOME/.local/include/raylib.h"
```

Se a pasta da raylib já existir, reutilize a instalação existente em vez de
sobrescrevê-la. Os scripts também aceitam `RAYLIB_PREFIX` para outra localização.

## Conteúdo atual

Salão, banheiro, escritório, estoque e projeção têm entradas e saídas.
A projeção é acessada pelo estoque. John possui 16 quadros de caminhada e quatro
poses paradas, e as cinco imagens de cenário são carregadas pelo jogo.

Ainda faltam colisões internas, requisitos de chave/fusível, interações,
inventário, puzzle, perseguição, sobrevivência, menus, cutscenes e finais.

## Verificar a base

```sh
./tests/test_base.sh
```

O teste usa a raylib real e uma janela oculta; precisa do ambiente gráfico do WSL.
Ele percorre as cinco salas, retorna ao salão, verifica os sprites e decodifica
todos os PNGs disponíveis. Ele não substitui os testes do jogo completo.

- [Regras da primeira versão](docs/REGRAS_DO_JOGO.md)
- [Base consolidada e resultados dos testes](docs/BASE_TESTADA.md)
- [Descrição do teste de regressão](tests/README.md)
