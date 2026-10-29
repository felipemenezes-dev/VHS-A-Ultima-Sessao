# VHS: A Última Sessão — regras definitivas da primeira versão

Versão 1.1 · 29 de outubro de 2026

Este documento fecha o passo 2 do desenvolvimento. Ele descreve o comportamento
que será implementado nas próximas etapas; não afirma que esses sistemas já estão
prontos. A base de código consolidada no passo 1 parte do commit `4e217ae`, que inclui a navegação pelas cinco salas.

Referência narrativa: **Roteiro VHS.pdf**, versão 1.0 de 20/10/2026. Os valores
originais são mantidos. As decisões adicionais abaixo resolvem casos que o roteiro
deixava abertos, sem acrescentar salas, inimigos ou finais.

## 1. Escopo e objetivo

- Jogo 2D em C com raylib; John é o único personagem controlável.
- Cinco áreas: salão, banheiro, escritório, estoque e sala de projeção.
- Uma entidade: o Rebobinador. Dois resultados: vitória ou derrota.
- Objetivo: recuperar três fitas, ordenar suas frequências, reproduzir a lista,
  descobrir o código, destravar a porta e sair da locadora.
- Não haverá armas convencionais, corrida, stamina, fome, peso, salvamento,
  novas áreas, finais secretos ou outros monstros na primeira versão.
- A apresentação deve funcionar sem dublagem e sem depender de áudio para pistas.
- Fechar o programa ou reiniciar a sessão descarta o progresso da partida.

### Decisões adicionais fechadas neste passo

Estas definições completam os pontos abertos do roteiro e serão a referência da
implementação futura:

- Despertar pela identidade 42; escalada pela quantidade de três fitas, sem impor
  uma ordem de coleta.
- Inventário, VHS e teclado suspendem a simulação da partida.
- Projeção conectada ao estoque, sem exigir uma porta adicional no salão.
- John a 252 unidades/s e interação a 64 unidades, em coordenadas virtuais.
- Flash frontal, bloqueado por obstáculos, com intervalo de 0,5 segundo.
- Aviso de um segundo em reaparições e distância mínima de 220 unidades.
- Introdução pulável somente depois de já vista na mesma execução, segurando
  Enter por 1,5 segundo; nenhum progresso de gameplay é preservado.

## 2. Controles e funcionamento das telas

| Controle | Exploração | Interfaces |
| --- | --- | --- |
| WASD / setas | Mover John | Setas navegam nas opções |
| E | Interagir com o alvo próximo | Não repete coleta nem confirma menus |
| F | Usar uma carga de flash | Desabilitado |
| Q | Usar o kit médico | Desabilitado |
| Tab | Abrir inventário | Fecha o inventário quando ele estiver aberto |
| Esc | Abrir pausa | Volta/fecha a interface permitida |
| Enter | Sem ação de combate | Confirma a opção ou o código |
| 0–9 / Backspace | Sem ação | Digitar/apagar no teclado da saída |

- Movimento é contínuo; coleta, flash, cura e confirmação usam uma ativação por
  pressão. Segurar E, F ou Q não repete ações.
- Uma pressão usada para abrir uma interface não pode também acionar sua opção.
- Esc nunca fecha diretamente o programa durante a exploração.
- O menu principal oferece Iniciar sessão, Como jogar, Créditos e Sair.
- A pausa oferece Continuar, Controles, Voltar ao menu e Sair. Abandonar uma partida
  em andamento pede confirmação e não salva o progresso.
- O botão de fechar a janela encerra o programa e libera os recursos.

### O que fica suspenso

| Estado | Movimento, inimigo, dano e temporizadores da partida | Animações da interface/cena |
| --- | --- | --- |
| Exploração | Ativos | Ativas |
| Inventário | Suspensos | Ativas |
| Aparelho VHS / teclado | Suspensos | Ativas |
| Pausa e seus submenus | Suspensos | Ativas |
| Cutscene | Suspensos | Ativas |
| Vitória / derrota / menu | Não atualizam a partida | Ativas |

A suspensão inclui aparição do inimigo, intervalo de dano, atordoamento e recarga
do flash. Fechar a interface retoma o tempo restante, sem acumular dano ou movimento.
Mensagens contextuais curtas e o HUD não pausam a exploração.

## 3. Espaço, movimentação e portas

- Todas as posições, velocidades e alcances de gameplay usam coordenadas virtuais
  de **1280 × 720**, independentes do tamanho da janela.
- Velocidade de referência de John: **252 unidades/s**, preservando a velocidade
  da base atual em 720p. Diagonais normalizadas não podem ser mais rápidas.
- O ponto de apoio do sprite é o centro dos pés. Colisão considera uma área curta
  junto aos pés, não a altura inteira da imagem.
- John e o inimigo respeitam os obstáculos sólidos. O desenho de um móvel, sozinho,
  não cria colisão: cada sala precisa de uma configuração explícita.
- Portas abertas são ativadas ao atravessar sua região de passagem. Portas com
  requisito exigem interação com E para destravar; essa interação pode completar
  a entrada na sala quando o requisito estiver satisfeito.
- A chegada sempre fica fora de obstáculos e fora do gatilho de retorno imediato.
- A região de chegada precisa ser abandonada antes de a mesma passagem poder
  ativar novamente. Apenas um destino pode ser processado por atualização.

### Conexões da primeira versão

- Salão ↔ banheiro.
- Salão ↔ escritório.
- Salão ↔ estoque, após obter a chave.
- Estoque ↔ projeção, após instalar o fusível no quadro do estoque.
- Salão → exterior somente pela condição de vitória.

Não é necessária uma ligação direta salão–projeção. O retorno da projeção passa
pelo estoque. As posições exatas serão ajustadas às imagens na etapa de colisões.

## 4. Interação e persistência

- Alcance inicial de interação: **64 unidades** entre os pés de John e o ponto de
  interação do objeto. O ponto deve ficar em uma posição alcançável.
- Interação não atravessa paredes ou obstáculos sólidos.
- Se houver mais de um candidato, selecionar o mais próximo; empates usam uma
  prioridade estável definida na sala. A indicação na tela identifica o alvo.
- Uma pressão de E gera, no máximo, uma ação e uma mensagem.
- Objetos coletados somem da sala e não retornam ao mudar de ambiente.
- Fitas não podem existir duas vezes na lista. Chave obtida continua obtida;
  fusível instalado continua instalado; kit e bateria usados não reaparecem.
- Objetos decorativos podem apresentar texto, mas não viram itens do inventário.

## 5. Progressão e ordem livre de exploração

| Local | Objeto/ação | Efeito |
| --- | --- | --- |
| Salão | Kit médico | Disponibiliza uma cura de uso único |
| Banheiro | Fita 17 — A Chamada | Adiciona a fita à lista |
| Escritório | Chave | Permite destravar o estoque; não é consumida |
| Escritório | Aparelho VHS / bilhete | Apresenta a pista e o puzzle |
| Estoque | Fusível | Pode ser coletado antes ou depois da Fita 42 |
| Estoque | Instalar fusível no quadro | Restaura acesso à projeção; instalação única |
| Estoque | Fita 42 — O Corredor | Adiciona a fita e desperta o Rebobinador uma vez |
| Projeção | Fita 89 — Sala Vazia | Adiciona a fita à lista |
| Projeção | Bateria | Adiciona duas cargas imediatamente; coleta única |

- Banheiro e escritório estão acessíveis no início.
- A Fita 42 pode ser a primeira, segunda ou terceira coletada. O despertar depende
  de sua identidade, não da quantidade anterior de fitas.
- A Fita 89 pode ser coletada antes da Fita 17, e até antes da Fita 42 se John
  recolher e instalar o fusível primeiro.
- O contador consulta a lista real e aumenta em uma unidade por nova fita.
- Ao completar as três fitas, o objetivo passa a ser retornar ao escritório e
  reconstruir a transmissão. O inimigo ativo assume a velocidade de 150.
- Se a Fita 42 for a última, o inimigo faz a aparição inicial normalmente e começa
  a perseguir a 150 após o aviso. Se já estiver derrotado, não reaparece.

## 6. Lista encadeada e aparelho VHS

Cada nó contém identificador, nome, frequência, dígito e ponteiro para o próximo nó.
A lista simplesmente encadeada é a fonte dos dados de inventário, contagem,
consulta, ordenação e reprodução; a interface não mantém uma segunda lista lógica.

| Identificador | Nome | Frequência | Dígito associado |
| --- | --- | --- | --- |
| 17 | A Chamada | 17 | 4 |
| 42 | O Corredor | 42 | 1 |
| 89 | Sala Vazia | 89 | 7 |

- Inserção preserva inicialmente a ordem de coleta.
- Operações previstas: criar nó, inserir, buscar, listar, contar, remover e liberar.
- Não remover fitas apenas para simular a reprodução: elas continuam disponíveis.
- Falha de alocação não pode remover a fita do cenário nem marcar a coleta como
  concluída; apresentar erro recuperável e manter a partida coerente.
- O Insertion Sort deve operar de verdade sobre os dados da lista, em frequência
  crescente. Sua implementação precisa corresponder ao conteúdo da disciplina.
- A lista vazia, a lista já ordenada e as ordens diferentes de coleta são válidas.

### Opções do aparelho

1. Consultar fitas: percorre a lista e mostra nomes, frequências e quantidade.
2. Organizar: exige exatamente as três fitas distintas e executa o Insertion Sort.
3. Reproduzir: exige as três fitas, a execução da ordenação e a lista efetivamente
   em ordem crescente. Um simples booleano não substitui a conferência dos dados.
4. Sair: retorna à exploração sem perder o inventário ou a ordenação.

A reprodução percorre os nós na ordem resultante e mostra um fragmento por vez.
Os dígitos são obtidos dos nós e concatenados nessa ordem; não podem ser apenas um
`417` fixo desenhado na interface. Cada fragmento permanece aproximadamente dois
segundos, com extensão de tempo quando necessária à leitura da legenda.

Os dígitos associados não aparecem no inventário antes de sua revelação no VHS.
Somente concluir a reprodução completa marca a transmissão como reconstruída.
Cancelar antes do término não libera a saída. Uma reprodução já concluída pode
ser consultada novamente sem apagar o resultado ou duplicar eventos.

Se a ordem de coleta já for 17 → 42 → 89, o algoritmo ainda é executado e a
interface informa que a sequência já estava organizada. Isso não constitui erro.

## 7. Vida, kit e dano

| Parâmetro | Valor |
| --- | --- |
| Vida inicial/máxima de John | 100 |
| Dano por contato | 20 |
| Intervalo mínimo entre danos | 1 segundo |
| Cura do kit | 30, limitada ao máximo de 100 |
| Kit disponível | Um por partida |

- Contato é verificado apenas entre John e um inimigo ativo na mesma sala.
- O intervalo é uma proteção do jogador: não é reiniciado a favor do ataque ao
  atravessar portas ou abrir e fechar interfaces.
- O jogador pisca durante a proteção, sem perder o controle por longos períodos.
- Q consome o kit apenas se John estiver vivo, com vida abaixo de 100 e com o item.
- Vida zero leva imediatamente à derrota. Depois disso, coleta, cura e vitória
  não podem continuar sendo processadas naquela atualização.

## 8. Flash

| Parâmetro | Valor |
| --- | --- |
| Cargas iniciais | 3 |
| Cargas da bateria única | +2 |
| Dano ao inimigo por acerto | 25 |
| Alcance | 140 unidades |
| Atordoamento por acerto | 1,5 segundo |
| Intervalo entre ativações | 0,5 segundo |

- A direção é a última direção de John, inclusive quando ele estiver parado.
- O alvo precisa estar na mesma sala, a até 140 unidades, na metade frontal
  determinada pela direção e sem obstáculo sólido entre os pontos de apoio.
- Uma ativação consome uma carga, mesmo errando ou atingindo um obstáculo.
- Sem carga, ou durante o intervalo de ativação, a ação é recusada sem consumo.
- Um acerto atordoa e causa dano uma vez. Novo acerto reinicia o atordoamento em
  1,5 segundo; não soma durações indefinidamente.
- Atordoado, o inimigo não se move nem causa dano de contato.
- Um flash válido que atinja o inimigo é resolvido antes do dano de contato daquela
  atualização, permitindo ao jogador reagir à aproximação.
- A bateria é consumida na própria coleta; não exige uma tecla adicional.

## 9. Rebobinador

| Parâmetro | Valor |
| --- | --- |
| Vida inicial | 100 |
| Velocidade inicial | 110 unidades/s |
| Velocidade com três fitas recuperadas | 150 unidades/s |
| Espera no primeiro despertar | 2 segundos |
| Aviso em transições posteriores | 1 segundo |
| Distância mínima de materialização de John | 220 unidades |

- Começa inativo e só desperta pela primeira coleta da Fita 42.
- Durante a aparição, não causa dano. Só persegue após o aviso.
- Respeita as colisões do cenário e não atravessa móveis sólidos.
- Pode alcançar as cinco áreas; nenhuma é permanentemente segura na exploração.
- A navegação pode começar simples, com pontos de passagem nos corredores se a
  perseguição direta prender a criatura. A* não é requisito da primeira versão.
- Ao John trocar de sala, suspender a entidade da sala anterior e manter vida,
  velocidade, derrota e tempo restante de atordoamento.
- Se estiver viva, a entidade pode materializar-se em um ponto previamente validado
  da nova sala, após aviso visual, sem ocupar John ou bloquear a única passagem.
- Se não houver ponto seguro a 220 unidades, adiar a materialização e reavaliar
  quando houver espaço. A falta de ponto não a derrota nem encerra a perseguição.
- Trocar de sala não pode encurtar o tempo restante do primeiro despertar.
- Quatro acertos de flash retiram seus 100 pontos de vida. Derrotada, desaparece e
  permanece derrotada até o reinício completo, inclusive após novas trocas de sala.
- Não é necessário derrotá-la para vencer.

## 10. Teclado, vitória e derrota

- O teclado recebe três dígitos, aceita Backspace e confirma com Enter.
- Antes da transmissão reconstruída, nenhuma senha libera a saída, inclusive 417.
- Depois da reconstrução, comparar a entrada ao código produzido pela lista.
- Tentativa incorreta informa o erro, limpa a entrada e permite tentar novamente.
- Tentativas não possuem limite. O teclado pausa a exploração para leitura e uso.
- Entrada correta marca a saída como liberada de forma persistente, com indicação
  verde. Ainda é necessário sair da interface e interagir com a porta.
- Vitória exige John vivo, transmissão reconstruída, saída liberada e interação
  final com a porta. Não depende da vida do Rebobinador.
- Derrota acontece quando a vida chega a zero e tem prioridade sobre qualquer
  tentativa de concluir a saída na mesma atualização.
- As duas telas finais oferecem reiniciar, voltar ao menu e sair.

## 11. Narrativa e apresentação

- A introdução mostra: entrega da fita; gravação da Aurora; decisão de John;
  chegada à locadora; porta travando; transmissão com a missão.
- A transmissão inicial é acionada uma vez ao aproximar-se da TV principal.
- Ela explica as três frequências e o aparelho do escritório, mas não revela 417.
- Cutscenes usam imagens estáticas, fades e aproximações simples. Legendas têm
  fundo escuro, contraste suficiente e tempo de leitura.
- Textos indispensáveis devem ser desenhados de forma legível pelo jogo, evitando
  depender apenas de letras pequenas incorporadas nas imagens.
- Cutscenes não aceitam controles de exploração. Esc não abre a pausa nelas.
- Na primeira exibição da introdução, respeitar os tempos e a leitura. Em partidas
  seguintes da mesma execução, permitir pular a introdução segurando Enter por
  1,5 segundo, com indicação de progresso. Uma pressão isolada não pula cenas.
- Após pular, restaurar o mesmo estado inicial de gameplay e manter a explicação
  da missão disponível na TV/objetivo, sem pular progressão ou adicionar itens.
- Vitória apresenta saída de John e o último sinal da televisão. Derrota usa
  interferência e a mensagem SINAL PERDIDO / JOHN FOI ABSORVIDO PELA FITA.
- Áudio é opcional; os avisos de ameaça, objetivo e dano continuam visuais.

## 12. Reinício e encerramento

Reiniciar precisa, como uma única operação lógica:

1. Liberar todos os nós da lista anterior e criar inventário vazio.
2. Restaurar John no salão com 100 de vida e três cargas.
3. Devolver fitas, chave, fusível, kit e bateria às salas.
4. Destrancar somente as portas inicialmente abertas; zerar os demais requisitos.
5. Desativar e restaurar o Rebobinador com 100 de vida e velocidade 110.
6. Limpar dano, flash, atordoamento, avisos e transições pendentes.
7. Limpar código digitado, revelação, reprodução e liberação da saída.
8. Restaurar todos os eventos de uma única ocorrência e o objetivo inicial.

A indicação de que a introdução já foi vista pode permanecer somente durante a
mesma execução do programa. Ela não preserva nenhum progresso da partida.
Ao fechar o programa, liberar também texturas, fontes e demais recursos.

## 13. Casos de aceitação para as próximas etapas

| Caso | Resultado esperado |
| --- | --- |
| Coletar 42 antes de 17 | Contador 1/3 e despertar único |
| Coletar 89 antes de 17 ou 42 | Progressão válida, sem impor ordem artificial |
| Completar a lista com 17 ou 42 | Ameaça ativa usa velocidade 150; contador 3/3 |
| Voltar a uma sala visitada | Objetos coletados continuam ausentes |
| Ordenar coleta 42 → 89 → 17 | Lista torna-se 17 → 42 → 89 e reprodução gera 417 |
| Coletar já em ordem | Ordenação válida, sem duplicação ou erro |
| Digitar 417 antes da reprodução completa | Porta permanece bloqueada |
| Cancelar uma reprodução incompleta | Transmissão não é concluída |
| Usar flash fora de alcance, atrás ou através de parede | Gasta uma carga; não causa dano |
| Usar flash sem cargas ou em intervalo | Não dispara nem consome |
| Pausar durante aparição, dano ou atordoamento | Tempos permanecem congelados |
| Trocar de sala durante atordoamento | Não cura o inimigo nem elimina o tempo restante |
| Usar kit com vida cheia | Mantém kit e vida 100 |
| Derrotar inimigo e trocar de sala | Inimigo continua derrotado |
| Destravar saída sem interagir com a porta | Partida continua |
| Reiniciar depois de vitória/derrota | Estado inicial integral, sem lista ou eventos antigos |
| Redimensionar a janela | Não muda velocidade, alcance ou geometria do mundo |

## 14. Limites desta entrega

O passo 1 atualiza e verifica a base existente. O passo 2 registra as regras acima.
A navegação pelas cinco salas já faz parte da base integrada. A refatoração,
a resolução virtual, as colisões, as interações e os demais sistemas serão
implementados nos passos seguintes. A documentação não deve
ser interpretada como confirmação de que esses recursos já foram programados.
