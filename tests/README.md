# Teste da base dos passos 1 e 2

Na raiz do projeto, execute `./tests/test_base.sh`.

Pré-requisitos: GCC, raylib 5.5 e acesso ao ambiente gráfico do WSL/WSLg.
O script compila `tests/test_base.c` em `build/test_base` e limita a execução
a 40 segundos para detectar percursos que ficam presos.

O runner inclui o fonte real do jogo com outro nome para a função principal.
Ele mantém o carregamento, o desenho e a liberação reais da raylib. Somente as
entradas de movimento, o tempo por atualização e o limite de execução são
controlados. A janela fica oculta e o limite artificial de FPS é removido.

## O que é verificado

- Salão → banheiro → salão → escritório → salão → estoque → projeção → estoque → salão.
- Sequência exata das transições, detectada pelo cenário realmente desenhado.
- Posições finitas e dentro da tela durante esse percurso.
- Carregamento dos 20 sprites e desenho dos 16 quadros de caminhada e quatro poses.
- Movimento nas quatro direções.
- Decodificação de todos os PNGs sob `assets/`, com falha se alguma imagem for inválida.
- Retorno normal do laço principal e descarregamento de seus recursos.

Os números de salas e a sequência representam a base atual. O teste deve ser
adaptado quando portas, interações, bloqueios ou a arquitetura forem alterados.

Isso não verifica colisões internas, regras do puzzle, inimigo, menus ou finais,
pois esses sistemas ainda não estão implementados. Os limites de tela são
observados durante o percurso, sem varredura exaustiva de todos os cantos.
