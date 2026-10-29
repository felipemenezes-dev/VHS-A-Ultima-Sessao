# Passo 3 — módulos, colisões e inventário vazio

29 de outubro de 2026

## O que mudou

O laço principal foi reduzido e as responsabilidades estão separadas:

| Arquivo | Responsabilidade |
| --- | --- |
| `src/main.c` | Janela, laço principal e encerramento |
| `src/game.c/.h` | Coordenação, leitura de entrada, pausa do inventário e apresentação |
| `src/types.h` | Resolução virtual e identificadores compartilhados |
| `src/assets.c/.h` | Carregamento e liberação de cenários, sprites e imagem da interface |
| `src/player.c/.h` | Movimento normalizado, direção, animação e desenho do John |
| `src/rooms.c/.h` | Retângulos manuais, portas e posições de chegada de cada sala |
| `src/world.c/.h` | Caixa dos pés, colisão, deslizamento e transições |
| `src/inventory_ui.c/.h` | Desenho do inventário vazio |

## Inventário: limite solicitado

- Tab abre; pressionar Tab novamente fecha.
- Esc fecha o inventário e retorna ao jogo sem encerrar a janela.
- Enquanto estiver aberto, movimento, animação do John e troca de salas ficam suspensos.
- Uma pressão que abre/fecha a interface não movimenta o personagem no mesmo quadro.
- A imagem é somente a moldura: título, rótulos e sete espaços vazios são desenhados
  pelo código. Não há item escondido ou desenhado de forma permanente na imagem.
- Não foram implementados coleta, armazenamento, preenchimento, navegação entre
  itens, uso de itens, lista de fitas ou puzzle. Não há itens de demonstração.
- Fora do inventário, Esc ainda encerra esta base. O menu de pausa geral é futuro.

## Colisões

Implementação baseada no texto fornecido: áreas sólidas definidas manualmente,
caixa de 24 × 12 unidades nos pés e verificação de X e Y separadamente.
O salão usa a máscara alpha do repositório; as outras salas usam retângulos.
Subpassos de até quatro unidades evitam atravessar obstáculos finos em um movimento.
A velocidade de referência permanece 252 unidades/s; diagonais são normalizadas.

O salão passou a usar a máscara alpha recebida em `c7b52ea`; as outras quatro
salas mantêm retângulos de paredes e dos móveis sólidos principais.
As paredes são divididas nas passagens; a porta do banheiro foi ajustada à imagem.
As posições de chegada ficam fora dos obstáculos e dos gatilhos de retorno.
Os pontos de colisão representam a base dos móveis; não o topo desenhado.

F1 liga/desliga a inspeção visual: a máscara do salão fica verde, retângulos
das outras salas ficam vermelhos, portas ficam verdes e a caixa dos pés fica azul.
O mouse mostra coordenadas e o clique registra o ponto no terminal. Esse modo começa desligado.
Os retângulos ficam em `src/rooms.c`; a máscara fica em
`assets/cenarios/colisoes_salao.png`. Ambos são configurações manuais.

## Apresentação e profundidade

O mundo usa 1280 × 720 unidades. A janela redimensiona a apresentação com barras
quando necessário, mantendo velocidades, colisões e interface nas mesmas coordenadas.

A profundidade ainda não foi implementada: os mapas continuam sendo imagens
completas e John é desenhado depois do fundo. A próxima etapa visual precisa de
recortes transparentes próprios para os móveis; não desenhar o fundo inteiro
novamente por cima do personagem. Nenhum cenário original foi alterado nesta entrega.

## Verificação

Execute `./tests/test_base.sh` na raiz do projeto. O teste usa raylib e imagens reais,
com janela oculta, e verifica:

- Todos os pontos de chegada livres de colisão.
- Existência de caminho para todas as portas de cada sala.
- Salão → banheiro → salão → escritório → salão → estoque → projeção → estoque → salão.
- Ausência de sobreposição com obstáculos a cada atualização do percurso.
- Bloqueio de uma estante, deslizamento diagonal e movimento longo sem atravessá-la.
- Tab abre/fecha e Esc fecha antes de solicitar a saída do jogo.
- Posição e tempo de animação congelados com o inventário aberto.
- Redimensionamento sem mudança da posição lógica.
- Carregamento das 97 imagens PNG, incluindo o novo fundo da interface.
- Liberação de recursos, inclusive chamada repetida de encerramento.

A procura de caminho por grade é usada apenas pelo teste para verificar as portas;
não é uma inteligência artificial acrescentada ao jogo.
Os resultados não substituem uma revisão manual de todas as bordas de móveis.

## Recursos e preservação

- Moldura: `assets/ui/inventario_fundo.png`.
- Geração: ferramenta nativa de geração de imagens, em 29/10/2026.
- Prompt: `docs/PROMPT_INVENTARIO.txt`.
- Backup anterior à refatoração: `build/backup-antes-passo3.tar.gz` e
  `build/vhs.antes-passo3`, locais e ignorados pelo Git.
- Branch da entrega: `codex/wsl-setup`. Os módulos, a interface vazia e a integração
  das colisões estão registrados no histórico desta branch.

Integração da máscara e detalhes: [Atualização c7b52ea](ATUALIZACAO_c7b52ea.md).
