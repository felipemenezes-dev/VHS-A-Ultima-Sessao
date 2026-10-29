# Verificação da base modular

Execute `./tests/test_base.sh` na raiz, com GCC, raylib 5.5 e WSLg disponíveis.
O runner usa janela oculta e liga os módulos reais do jogo, sem incluir `main.c`.
Os testes exercitam a mesma função `GameStep` usada pela entrada normal.

Verificações: percursos e chegadas nas cinco salas com colisões; deslizamento;
movimento longo contra estante; Tab/Esc e pausa da animação; redimensionamento;
carregamento de todos os PNGs; encerramento repetido seguro.

O teste constrói caminhos em grade apenas para alcançar as portas sem atravessar
os obstáculos. A inteligência de navegação não pertence ao gameplay.
Também salva imagens de conferência sob `build/qa_sala_*.png` e
`build/inventario_no_jogo.png`. Saída de geração de screenshot não é adicionada ao Git.

Não há teste de preenchimento: armazenamento, coleta, uso e inventário de itens
foram explicitamente deixados para uma próxima entrega.

A geometria do salão usa a máscara alpha do commit c7b52ea. As demais salas
continuam usando os retângulos manuais. A máscara é validada no percurso real.
