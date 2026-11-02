# Profundidade visual — 02/11/2026

> Registro da etapa de oclusão. As 957 falhas de limites mencionadas abaixo foram corrigidas posteriormente; veja [LIMITES_DAS_SALAS.md](LIMITES_DAS_SALAS.md).

Correção aplicada sobre `c57597c`, restrita ao desenho dos cinco cenários. Os PNGs originais, as máscaras de colisão, a movimentação e as portas permanecem intactos.

## Funcionamento

O fundo contém a arte original. John é desenhado no mundo de 1672 × 941. Em seguida, o renderizador compõe somente os pixels dos elementos que ficam à frente dele, por meio de texturas transparentes geradas ao iniciar o jogo.

As silhuetas são polígonos visuais independentes das máscaras verdes de colisão. Cada recorte copia os pixels originais dentro de seu contorno e deixa o restante transparente. A caixa retangular do recorte serve apenas para armazenamento e descarte de desenhos fora do personagem; ela não apaga o chão.

Cada móvel livre tem uma profundidade própria, comparada à coordenada dos pés. Partes de uma mesma mesa compartilham sua profundidade, preservando os vãos entre pernas/gaveteiros. Faces laterais ligadas às paredes, paredes dianteiras e batentes usam primeiro plano fixo. As aberturas das portas ficam fora dos recortes. Interface, fade e depuração continuam por cima da cena.

## Arquivos

- `src/scene_draw.c` e `.h`: criação, composição e liberação dos recortes.
- `src/depth_office.inc`: contornos do escritório.
- `src/depth_hall_bath.inc`: contornos do salão e banheiro.
- `src/depth_storage_projection.inc`: contornos do estoque e projeção.
- `src/main.c`: inicialização e encerramento dos recursos visuais.
- `makefile`: recompila ao alterar contornos; alvo `test-depth`.
- `tests/test_depth.c`: testes de renderização com os sprites reais e capturas.

## Verificação

`make test-depth` utiliza uma janela gráfica oculta e grava imagens em `build/depth-after/`. Cobre 25 posições, os quatro sprites parados e os 16 quadros de caminhada; também compara o personagem inteiro com o desenho sem camadas em cinco pontos de chão livre. A folha `contato.png` mostra móveis, corredores e as oito aproximações das portas; `direcoes.png` mostra todos os quadros ao lado dos equipamentos do escritório.

No WSL deste computador, a Raylib está em `$HOME/.local`. Compilação e teste:

```sh
make RAYLIB="$HOME/.local" LDLIBS="-L$HOME/.local/lib -lraylib -lGL -lm -lpthread -ldl -lrt -lX11"
make test-depth RAYLIB="$HOME/.local" LDLIBS="-L$HOME/.local/lib -lraylib -lGL -lm -lpthread -ldl -lrt -lX11"
```

Também foram conferidas a execução em janela de 800 × 600 e em tela cheia de 1920 × 1080. A transformação da cena inteira mantém os recortes alinhados.

## Resultados desta entrega

- Compilação do jogo e dos testes sem avisos.
- 500 poses nos cinco cenários, 100 comparações em chão livre e cinco verificações de fundo: zero falhas.
- Regressão específica do escritório: 40 poses confirmam corpo parcialmente oculto, cabeça visível e pixels originais. O mesmo teste aplicado ao desenho antigo detecta 40 falhas.
- 96 transições entre salas continuam aprovadas. O teste de circulação mantém as mesmas 957 falhas anteriores de limites, sem alteração de contagem.
- 122 polígonos de oclusão, carregados uma vez e liberados ao encerrar.
- Inspeção das capturas antes/depois, das quatro direções e dos 16 quadros de caminhada. Janela e tela cheia conferidas.
- Base de implementação: `c57597c`. As correções de profundidade e limites compõem uma entrega própria.

## Limites

Os contornos foram traçados manualmente para as artes atuais. Pequenos detalhes decorativos, como fios no chão e folhas isoladas, não têm recortes individuais. Se as artes ou a perspectiva mudarem, os contornos precisam ser ajustados junto com elas.

Os testes visuais amostram posições representativas; não provam todos os pontos possíveis do mapa. Algumas posições de estresse colocam os pés dentro de um obstáculo para testar a imagem, sem alterar sua colisão; o teste identifica essas posições como `blocked (visual stress)`.

As 957 falhas preexistentes do teste de limites dos cenários estão fora desta correção. Nenhuma barreira foi acrescentada para esconder erros de desenho.
