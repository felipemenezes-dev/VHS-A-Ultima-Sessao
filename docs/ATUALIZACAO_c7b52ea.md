# Atualização da origem — colisões do salão

29 de outubro de 2026

## Commit recebido

- `c7b52ea411e8dd601d605e6242f8a16ce6070415` — adição de colisoes do salao principal.
- Autor: Felipe. Publicado em 29/10/2026 às 16h38 (UTC−03).
- Dois arquivos: `src/main.c` e `assets/cenarios/colisoes_salao.png`.
- A máscara tem 1672 × 941 pixels; pixels transparentes são livres e pixels
  opacos representam os limites sólidos desenhados manualmente.

O commit adiciona colisões do salão usando a máscara, caixa nos pés, movimento X/Y
separado, visualização por F1 e coordenadas do mouse com registro ao clicar.
Na origem, os outros quatro cenários continuam sem colisões internas.
Ele não acrescenta inventário, puzzle, itens ou inimigo.

## Integração local

- A atualização foi mesclada à branch `codex/wsl-setup`; o histórico da origem foi preservado.
- Inventário vazio, módulos, retângulos das outras quatro salas e resolução virtual
  foram preservados e incluídos na entrega da branch `codex/wsl-setup`.
- A máscara substitui os antigos retângulos do salão, evitando duas geometrias
  concorrentes que poderiam fechar portas/corredores.
- A leitura da máscara respeita a regra alpha > 0 do commit recebido.
- Uma tabela acumulada de pixels opacos evita percorrer vários pixels a cada passo.
- A máscara e a tabela pertencem ao módulo `world` e são liberadas com o jogo.
- F1 mostra a máscara no salão e os retângulos nas outras salas. O clique registra
  percentuais e pixels virtuais; o mouse é convertido corretamente nas janelas com barras.
- As imagens originais dos cenários e a nova máscara foram mantidas sem edição.

## Verificações

Compilação sem avisos; passagem pelas cinco salas; retornos acessíveis; bloqueio
pela máscara; deslizamento; Tab/Esc; pausa do inventário; redimensionamento;
carregamento de 97 imagens e encerramento repetido sem erro.

Percurso: salão → banheiro → salão → escritório → salão → estoque → projeção → estoque → salão.

O inventário permanece sem coleta, preenchimento ou uso de itens. As camadas de
profundidade e as demais mecânicas continuam pendentes.
