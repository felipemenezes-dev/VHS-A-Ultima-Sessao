# VHS: A Última Sessão — base consolidada e verificada

29 de outubro de 2026 · Passos 1 e 2

**Registro histórico do commit `a310368`.** Para a implementação atual após a
refatoração, consulte [Passo 3](PASSO_3.md). Os hashes e resultados abaixo se
referem à base anterior, e não à compilação com inventário e colisões.

## Base integrada

- Commit de origem: `4e217aee8ea9f7d16747852959f3eabf658be7ca`.
- Mensagem: **ultimo cenario adicionado** (28/10/2026).
- Branch de trabalho: `codex/wsl-setup`.
- As cinco salas e suas transições foram preservadas.
- Ajuste de inicialização: iniciar a biblioteca antes de depender das dimensões
  do monitor, oferecer janela padrão e manter a opção de tela cheia.
- `compilar.sh` e `iniciar.sh` tornam a execução reproduzível no Ubuntu do WSL.
- `build/` é ignorado pelo Git. O executável original `vhs` foi preservado.

## Resultados verificados

| Verificação | Resultado |
| --- | --- |
| Integração da origem | Fast-forward sem conflitos |
| Compilação C99 com `-Wall -Wextra -Wpedantic` | Aprovada sem avisos |
| Sintaxe dos scripts | Aprovada |
| Percurso pelas cinco salas | Aprovado, com oito transições e retorno ao salão |
| Sprites | 20 carregados; 16 quadros de caminhada e quatro poses desenhados |
| Movimento | Quatro direções; posições finitas e dentro da tela no percurso |
| Imagens PNG | 95 decodificadas pela raylib; nenhuma inválida |
| Abertura por `./iniciar.sh` | Executável normal ativo durante o teste de 3 segundos |
| Registros de execução | Sem avisos ou erros da raylib |
| Formatação das alterações | `git diff --check` aprovado |

O teste de regressão concluído usou 688 atualizações a 1/60 s e o percurso:

Salão → banheiro → salão → escritório → salão → estoque → projeção → estoque → salão.

O runner está versionado em `tests/`. Ele utiliza o código, as imagens e a
renderização reais, com entradas programadas e janela oculta. O teste de abertura
separado usa o iniciador normal e é encerrado intencionalmente após três segundos.
Para repetir o percurso: `./tests/test_base.sh`.

## Identificação da compilação verificada

SHA-256 de `src/main.c`:

```text
10b0adba8776b10ff382996dea298fa0c7d45b51deecaf4bf38e027a90ed1101
```

SHA-256 de `build/vhs`:

```text
23e476b1280606aafc529194faa171eeb323cb7ba610514273ec8c561940f84f
```

O executável não é acrescentado ao Git. Compiladores ou opções diferentes podem
produzir outro hash. O commit dos passos 1 e 2 registra o fonte, os scripts, as
regras e o teste que permitem reproduzir a base.

## Preservação

Os ajustes anteriores à integração foram mantidos em um backup local e no Git
stash `af6bad56a444469138c116901da54e24c1611991`. Esses backups não são necessários para
executar o projeto e não são enviados junto com o fonte.

## Estado real e limites

Implementado: cinco cenários navegáveis, animações de John nas quatro direções,
carregamento de recursos e limites externos da tela.

Ainda não implementado: colisões internas, requisitos de chave/fusível,
interações, lista das fitas, puzzle, inimigo, sobrevivência, menus, cutscenes e finais.
Esc ainda fecha esta base; no jogo final abrirá a pausa.

A validação gráfica atual usa janela no WSL, não certifica o modo tela cheia e não
cobre todas as combinações possíveis. A documentação de regras descreve o destino
do projeto; não deve ser confundida com funcionalidades já programadas.

Próxima etapa: organizar o código em módulos, preservando o comportamento testado.
