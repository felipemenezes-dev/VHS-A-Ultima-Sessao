# Correção dos limites das salas — 02/11/2026

## Problema e causa

John alcançava o fundo preto abaixo da parede frontal do escritório e do estoque. O desenho da parede escondia somente uma faixa do sprite, porque os pés já estavam fora do piso.

As máscaras atuais dessas salas posicionavam parte do contorno inferior além da parede visível. Escritório, estoque e projeção também mantinham contornos abertos que ligavam o mapa ao exterior. A verificação considerava livres os pixels transparentes dentro da imagem, incluindo o fundo preto.

O diagnóstico reproduziu exatamente 319 células alcançáveis junto à borda da imagem em cada uma dessas três salas: 957 falhas no total. Salão e banheiro tinham zero dessas falhas.

## Correção

Cada sala agora possui um contorno fechado do piso em coordenadas da arte original. As reentrâncias das portas fazem parte desse contorno; os corredores terminam dentro dos gatilhos de transição, permitindo a troca mesmo após um movimento muito longo ou um intervalo de proteção da porta.

Ao carregar a colisão, o exterior do piso é marcado como sólido na mesma tabela usada pelos obstáculos. Isso verifica a caixa inteira dos pés, mantém os passos de movimento de no máximo um pixel e preserva a resolução horizontal/vertical separada. A rasterização considera os quatro cantos de cada pixel, evitando pequenas fugas em paredes inclinadas.

A depuração F1 mostra o contorno do piso em roxo. As máscaras PNG existentes continuam verdes. Os dois limites são combinados na colisão. As camadas visuais transparentes anteriores não foram ampliadas nem substituídas; o personagem simplesmente deixa de alcançar uma posição externa indevida.

## Arquivos desta correção

- `src/world.c`: contornos das cinco salas e consulta de ponto no piso.
- `src/world.h`: declaração dos contornos e consulta.
- `src/collision.c`: combina obstáculos com a área exterior sólida no carregamento.
- `src/main.c`: acrescenta o contorno roxo na depuração F1.
- `tests/test_rooms.c`: acrescenta regressões dos limites, caixa inteira dos pés, deslizamento e portas com movimentos longos. Mantém todas as verificações anteriores.
- `docs/LIMITES_DAS_SALAS.md`: este relatório.
- `docs/PROFUNDIDADE.md`: nota indicando que as falhas citadas no registro anterior foram resolvidas nesta etapa.

As demais mudanças pendentes da etapa anterior de profundidade foram preservadas. Nenhuma imagem original, máscara PNG, animação ou regra de transição foi modificada. Base de implementação: `c57597c`. Esta entrega reúne as correções de profundidade e limites das salas.

## Reprodução e capturas

Mesmos trechos indicados nas imagens do usuário, mesma câmera e mesmos comandos de movimento:

| Sala | Posição inicial dos pés | Movimento solicitado | Antes | Depois |
|---|---|---|---|---|
| Escritório | 452,735 | 139 pixels para baixo | 452,874 — no exterior | 452,759 — para na parede |
| Estoque | 600,740 | 113 pixels para baixo | 600,853 — no exterior | 600,780 — para na parede |

Os pontos iniciais são livres e alcançáveis a partir das entradas tanto antes quanto depois. As imagens usam o renderizador real e a mesma arte; o resultado não foi retocado. O teste anterior usa o código de colisão preservado antes desta correção.

## Validação

- 189.284 verificações de colisão/circulação: **zero falhas**.
- As 957 falhas anteriores foram eliminadas sem remover ou relaxar suas verificações.
- Caixa inteira dos pés dentro do piso em todas as células alcançáveis da malha de testes.
- 9.223 abordagens de paredes/cantos em oito direções, incluindo deslocamentos de 512 pixels; 12.500 movimentos variados adicionais.
- Regressões nas cinco paredes frontais com avanço de 2.000 pixels e deslizamento lateral de 32 pixels.
- 96 passagens entre salas, em ambos os sentidos e com quatro taxas de quadros.
- 56 aproximações das portas, incluindo passos equivalentes a 1,5 e 4 segundos; oito verificações adicionais com o intervalo de proteção da porta ativo.
- AddressSanitizer e UndefinedBehaviorSanitizer: mesmos testes aprovados, sem diagnóstico de erro.
- 140 verificações de imagens aprovadas usando o teste original do projeto.
- Compilação sem avisos. Capturas e execução em janela 800 × 600 e tela cheia 1920 × 1080.

O teste original de circulação mantém contagem variável conforme o número de pontos alcançáveis; áreas externas deixaram de participar das rotas. As verificações novas aumentaram a cobertura total.

## Manutenção

Se a geometria das salas mudar, ajustar os contornos em `src/world.c` junto com as colisões dos objetos. As artes atuais são a referência, e os testes não substituem a revisão visual após mudanças de mapa.
