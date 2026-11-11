#include "raylib.h"
#include "mundo.h"
#include "colisoes.h"
#include "camadas.h"
#include "inimigo.h"
#include "menu.h"
#include "derrota.h"
#include "interface_inventario.h"
#include "interface_vhs.h"
#include "interface_teclado.h"
#include "estado_jogo.h"
#include "objetos.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "legendas.h"
#include "cutscenes.h"

#define DURACAO_DANO_VISUAL 0.95f
#define LINHA_SAIDA_Y 728.0f
#define LINHA_SAIDA_ESQUERDA 700.0f
#define LINHA_SAIDA_DIREITA 978.0f

static Texture2D carregarImagemPersonagem(const char *caminho) {
    Image imagem = LoadImage(caminho);
    Texture2D textura = {0};
    if (imagem.data) {
        ImageAlphaCrop(&imagem, 0.05f);
        textura = LoadTextureFromImage(imagem);
        UnloadImage(imagem);
        if (textura.id)
            SetTextureFilter(textura, TEXTURE_FILTER_POINT);
    }
    return textura;
}

static void localizarRecursos(void) {
    if (FileExists("assets/cenarios/salao_principal.png"))
        return;
    char caminho[4096];
    ssize_t quantidade = readlink(
        "/proc/self/exe",
        caminho,
        sizeof caminho - 1
    );
    if (quantidade <= 0)
        return;
    caminho[quantidade] = '\0';
    char *barra = strrchr(caminho, '/');
    if (!barra)
        return;
    *barra = '\0';
    if (ChangeDirectory(caminho) &&
        !FileExists("assets/cenarios/salao_principal.png")) {
        ChangeDirectory("..");
    }
}

static bool tentarCruzarLinhaSaida(
    int sala,
    Vector2 posicaoAnterior,
    Vector2 deslocamento,
    bool saidaLiberada
) {
    if (!saidaLiberada || sala != SALAO || deslocamento.y <= 0.0f)
        return false;
    float proximaX = posicaoAnterior.x + deslocamento.x;
    float proximaY = posicaoAnterior.y + deslocamento.y;
    float margem = LARGURA_PES / 2.0f;
    bool dentroDaPorta =
        proximaX >= LINHA_SAIDA_ESQUERDA + margem &&
        proximaX <= LINHA_SAIDA_DIREITA - margem;
    bool atravessandoLinha =
        posicaoAnterior.y <= LINHA_SAIDA_Y + 0.5f &&
        proximaY >= LINHA_SAIDA_Y;
    return dentroDaPorta && atravessandoLinha;
}

static void desenharDepuracao(
    int sala,
    Vector2 personagem,
    Texture2D mascara
) {
    DrawTexture(
        mascara,
        0,
        0,
        (Color){125, 255, 150, 210}
    );
    DrawRectangleLinesEx(
        caixaPes(personagem),
        2,
        YELLOW
    );
    const LimiteChao *chao = &limitesChao[sala];
    for (int i = 0; i < chao->quantidade; i++) {
        DrawLineEx(
            chao->pontos[i],
            chao->pontos[(i + 1) % chao->quantidade],
            3,
            VIOLET
        );
    }
    for (int i = 0; i < quantidadePortas; i++) {
        const Porta *porta = &portas[i];
        if (porta->origem == sala) {
            DrawRectangleLinesEx(
                porta->area,
                3,
                SKYBLUE
            );
            DrawText(
                TextFormat("Porta %d: %s", i, porta->rotulo),
                (int)porta->area.x,
                (int)porta->area.y - 22,
                20,
                SKYBLUE
            );
            if (porta->limiteA.x != 0) {
                DrawLineEx(
                    porta->limiteA,
                    porta->limiteB,
                    3,
                    ORANGE
                );
            }
        }
        if (porta->destino == sala) {
            DrawCircleLines(
                porta->chegada.x,
                porta->chegada.y,
                18,
                YELLOW
            );
            DrawText(
                TextFormat("Entrada %d", i),
                (int)porta->chegada.x + 22,
                (int)porta->chegada.y - 10,
                19,
                YELLOW
            );
        }
    }
    DrawRectangle(
        20,
        90,
        440,
        116,
        (Color){6, 8, 14, 230}
    );
    DrawText(
        "F1 - MOSTRAR COLISOES",
        34,
        102,
        22,
        YELLOW
    );
    DrawText(
        TextFormat(
            "Pes: %.1f, %.1f | %.1f x %.1f",
            personagem.x,
            personagem.y,
            LARGURA_PES,
            ALTURA_PES
        ),
        34,
        135,
        19,
        RAYWHITE
    );
    DrawText(
        "Roxo: limite do piso",
        34,
        184,
        18,
        VIOLET
    );
    DrawText(
        "Azul: passagem | Amarelo: entrada",
        34,
        162,
        18,
        SKYBLUE
    );
}

static Texture2D carregarBilheteGerente(void) {
    const char *caminhos[] = {
        "assets/objetos/bilhete_escrito_gerente.png",
        "assets/itens/bilhete_escrito_gerente.png",
        "assets/objetos/bilhete_gerente_escrito.png",
        "assets/itens/bilhete_gerente_escrito.png"
    };
    for (int i = 0; i < (int)(sizeof caminhos / sizeof caminhos[0]); i++) {
        if (!FileExists(caminhos[i]))
            continue;
        Texture2D textura = LoadTexture(caminhos[i]);
        if (textura.id) {
            SetTextureFilter(textura, TEXTURE_FILTER_POINT);
            return textura;
        }
    }
    TraceLog(LOG_WARNING, "Imagem do bilhete do gerente nao encontrada.");
    return (Texture2D){0};
}

static void desenharBilheteGerente(Texture2D textura) {
    DrawRectangle(
        0,
        0,
        LARGURA_MUNDO,
        ALTURA_MUNDO,
        (Color){0, 0, 0, 230}
    );
    if (textura.id && textura.width > 0 && textura.height > 0) {
        float escalaLargura = (LARGURA_MUNDO * 0.78f) / textura.width;
        float escalaAltura = (ALTURA_MUNDO * 0.78f) / textura.height;
        float escala = fminf(escalaLargura, escalaAltura);
        float largura = textura.width * escala;
        float altura = textura.height * escala;
        Rectangle destino = {
            (LARGURA_MUNDO - largura) * 0.5f,
            (ALTURA_MUNDO - altura) * 0.5f - 12.0f,
            largura,
            altura
        };
        DrawTexturePro(
            textura,
            (Rectangle){
                0,
                0,
                (float)textura.width,
                (float)textura.height
            },
            destino,
            (Vector2){0, 0},
            0.0f,
            WHITE
        );
    } else {
        const char *mensagem = "IMAGEM DO BILHETE NAO ENCONTRADA";
        DrawText(
            mensagem,
            (LARGURA_MUNDO - MeasureText(mensagem, 30)) / 2,
            ALTURA_MUNDO / 2,
            30,
            RAYWHITE
        );
    }
    const char *instrucao = "E / ESC - FECHAR BILHETE";
    DrawRectangle(
        0,
        ALTURA_MUNDO - 62,
        LARGURA_MUNDO,
        62,
        (Color){0, 0, 0, 220}
    );
    DrawText(
        instrucao,
        (LARGURA_MUNDO - MeasureText(instrucao, 24)) / 2,
        ALTURA_MUNDO - 43,
        24,
        RAYWHITE
    );
}

static void mostrarLegendaObjeto(int idObjeto) {
    switch (idObjeto) {
        case 1:
            break;
        case 17:
            mostrarLegenda(
                "FITA ENCONTRADA - A CHAMADA\n"
                "Frequencia: 17\n"
                "[GRAVACAO] Quatro pulsos. "
                "Uma voz sussurra: nao olhe para tras.",
                5.0f
            );
            break;
        case 2:
            mostrarLegenda(
                "CHAVE DO ESTOQUE ENCONTRADA.",
                3.0f
            );
            break;
        case 42:
            mostrarLegenda(
                "SINAL DETECTADO.",
                3.0f
            );
            break;
        case 3:
            mostrarLegenda(
                "FUSIVEL ENCONTRADO.",
                3.0f
            );
            break;
        case 89:
            mostrarLegenda(
                "FITA ENCONTRADA - SALA VAZIA\n"
                "Frequencia: 89\n"
                "[GRAVACAO] A ultima sala esta vazia. "
                "A transmissao, nao.",
                5.0f
            );
            break;
        case 4:
            break;
        default:
            break;
    }
}

int main(void) {
    localizarRecursos();
    int monitor = GetCurrentMonitor();
    int larguraTela = GetMonitorWidth(monitor);
    int alturaTela = GetMonitorHeight(monitor);
    SetConfigFlags(
        FLAG_FULLSCREEN_MODE |
        FLAG_VSYNC_HINT
    );
    InitWindow(
        larguraTela,
        alturaTela,
        "VHS - A Ultima Sessao"
    );
    if (!IsWindowReady())
        return 1;
    InitAudioDevice();
    Music musicaJogo = {0};
    bool musicaJogoIniciada = false;
    if (IsAudioDeviceReady()) {
        musicaJogo = LoadMusicStream("assets/sons/musica_jogo.mp3");
        if (IsMusicValid(musicaJogo)) {
            musicaJogo.looping = true;
            SetMusicVolume(musicaJogo, 0.35f);
        }
    }
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);
    Texture2D cenarios[NUM_CENARIOS] = {0};
    Texture2D texturasMascaras[NUM_CENARIOS] = {0};
    MapaColisao mascaras[NUM_CENARIOS] = {0};
    Texture2D animacoes[NUM_DIRECOES][NUM_QUADROS] = {0};
    Texture2D personagemParado[NUM_DIRECOES] = {0};
    const char *pastas[NUM_DIRECOES] = {
        "baixo",
        "cima",
        "direita",
        "esquerda"
    };
    bool erro = false;
    SistemaCutscenes cutscenes = {0};
    Texture2D texturaBilheteGerente = {0};
    for (int sala = 0; sala < NUM_CENARIOS; sala++) {
        cenarios[sala] = LoadTexture(salas[sala].imagem);
        texturasMascaras[sala] = LoadTexture(salas[sala].mascara);
        if (
            !cenarios[sala].id ||
            cenarios[sala].width != LARGURA_MUNDO ||
            cenarios[sala].height != ALTURA_MUNDO ||
            !texturasMascaras[sala].id ||
            !carregarColisoesSala(&mascaras[sala], sala)
        ) {
            TraceLog(
                LOG_ERROR,
                "Cenario ou mascara invalida: %s",
                salas[sala].nome
            );
            erro = true;
        } else {
            SetTextureFilter(cenarios[sala], TEXTURE_FILTER_POINT);
            SetTextureFilter(texturasMascaras[sala], TEXTURE_FILTER_POINT);
        }
    }
    for (int direcao = 0; direcao < NUM_DIRECOES; direcao++) {
        char caminho[256];
        snprintf(
            caminho,
            sizeof caminho,
            "assets/personagens/john/%s/"
            "john_parado_%s.png",
            pastas[direcao],
            pastas[direcao]
        );
        personagemParado[direcao] = carregarImagemPersonagem(caminho);
        if (!personagemParado[direcao].id)
            erro = true;
        for (int quadro = 0; quadro < NUM_QUADROS; quadro++) {
            snprintf(
                caminho,
                sizeof caminho,
                "assets/personagens/john/%s/"
                "john_andando_%s%d.png",
                pastas[direcao],
                pastas[direcao],
                quadro + 1
            );
            animacoes[direcao][quadro] = carregarImagemPersonagem(caminho);
            if (!animacoes[direcao][quadro].id)
                erro = true;
        }
    }
    for (int sala = 0; sala < NUM_CENARIOS && !erro; sala++) {
        if (pesBloqueados(&mascaras[sala], salas[sala].entrada)) {
            TraceLog(
                LOG_ERROR,
                "Entrada bloqueada: %s",
                salas[sala].nome
            );
            erro = true;
        }
    }
    for (int i = 0; i < quantidadePortas && !erro; i++) {
        if (pesBloqueados(
            &mascaras[portas[i].destino],
            portas[i].chegada
        )) {
            TraceLog(
                LOG_ERROR,
                "Destino da porta %d bloqueado",
                i
            );
            erro = true;
        }
    }
    if (!erro && !iniciarCamadas()) {
        TraceLog(
            LOG_ERROR,
            "Nao foi possivel preparar as camadas visuais"
        );
        erro = true;
    }
    Inimigo inimigo = {0};
    if (!erro && !iniciarInimigo(&inimigo, mascaras))
        erro = true;
    TelaInventario inventario = {0};
    EstadoJogo estadoJogo = {0};
    InterfaceVHS interfaceVHS = {0};
    TecladoSaida tecladoSaida = {0};
    reiniciarEstadoJogo(&estadoJogo);
    iniciarInterfaceVHS(&interfaceVHS);
    iniciarTecladoSaida(&tecladoSaida);
    if (!erro && !iniciarTelaInventario(&inventario)) {
        TraceLog(LOG_ERROR, "Nao foi possivel iniciar o inventario");
        erro = true;
    }
    if (!erro && !carregarObjetosMapa()) {
        TraceLog(LOG_ERROR, "Nao foi possivel carregar os objetos do cenario");
        erro = true;
    }
    if (!erro)
        iniciarLegendas();
    if (!erro && !carregarCutscenes(&cutscenes))
        erro = true;
    if (!erro)
        texturaBilheteGerente = carregarBilheteGerente();
    if (!erro && (!carregarMenu() || !carregarDerrota()))
        erro = true;
    RenderTexture2D quadro = {0};
    if (!erro) {
        quadro = LoadRenderTexture(LARGURA_MUNDO, ALTURA_MUNDO);
        if (!quadro.id)
            erro = true;
        else
            SetTextureFilter(quadro.texture, TEXTURE_FILTER_BILINEAR);
    }
    Vector2 personagem = salas[SALAO].entrada;
    int vida = 100;
    int danoRecebidoVisual = 0;
    float tempoDanoVisual = 0.0f;
    Vector2 posicaoDanoVisual = {0};
    int salaAtual = SALAO;
    int direcao = BAIXO;
    int quadroAnimacao = 0;
    float tempoAnimacao = 0;
    float duracaoQuadro = 0.11f;
    float esperaTransicao = 0;
    float esperaAvisoPorta = 0;
    float escurecimento = 0;
    float brilhoFlash = 0;
    bool mostrarColisoes = false;
    bool andando = false;
    bool noMenu = true;
    bool novaPartida = true;
    bool jogoConcluido = false;
    bool iniciarPartidaDireto = false;
    bool cutsceneDespertarIniciada = false;
    bool cutsceneVitoriaIniciada = false;
    bool cutsceneDerrotaIniciada = false;
    bool cutsceneDerrotaConcluida = false;
    bool bilheteAberto = false;
    bool dicaControlesPendente = false;
    float tempoDicaControles = 0.0f;

    /* Alteração 1: nova flag */
    bool aguardarSoltarConfirmacaoMenu = false;

    while (!erro && !WindowShouldClose()) {
        if (cutsceneEstaAtiva(&cutscenes) && IsKeyPressed(KEY_ESCAPE))
            break;
        if (IsMusicValid(musicaJogo)) {
            bool musicaPausada =
                noMenu ||
                inventario.aberto ||
                bilheteAberto ||
                interfaceVHSEstaAberta(&interfaceVHS) ||
                tecladoSaidaEstaAberto(&tecladoSaida) ||
                vida == 0 ||
                jogoConcluido ||
                cutsceneEstaAtiva(&cutscenes);
            if (musicaPausada) {
                if (IsMusicStreamPlaying(musicaJogo))
                    PauseMusicStream(musicaJogo);
            } else {
                if (!musicaJogoIniciada) {
                    PlayMusicStream(musicaJogo);
                    musicaJogoIniciada = true;
                } else if (!IsMusicStreamPlaying(musicaJogo)) {
                    ResumeMusicStream(musicaJogo);
                }
                UpdateMusicStream(musicaJogo);
            }
        }
        if (
            !noMenu &&
            vida == 0 &&
            !cutsceneDerrotaIniciada &&
            !jogoConcluido &&
            !cutsceneEstaAtiva(&cutscenes) &&
            !cutsceneVitoriaIniciada
        ) {
            cutsceneDerrotaIniciada =
                iniciarCutscene(&cutscenes, CUTSCENE_DERROTA);
            if (!cutsceneDerrotaIniciada)
                erro = true;
        }
        if (cutsceneEstaAtiva(&cutscenes))
            atualizarCutscene(&cutscenes, fminf(GetFrameTime(), 0.25f));
        TipoCutscene cutsceneConcluida =
            consumirCutsceneConcluida(&cutscenes);
        if (cutsceneConcluida == CUTSCENE_INTRODUCAO) {
            if (!iniciarCutscene(&cutscenes, CUTSCENE_MISSAO))
                erro = true;
        } else if (cutsceneConcluida == CUTSCENE_MISSAO) {
            mostrarLegenda(
                "OBJETIVO: ENCONTRE AS TRES FITAS VHS, RECONSTRUA A TRANSMISSAO\n"
                "E ESCAPE DA LOCADORA.",
                5.0f
            );
            dicaControlesPendente = true;
            tempoDicaControles = 5.0f;
        } else if (cutsceneConcluida == CUTSCENE_VITORIA) {
            jogoConcluido = true;
        } else if (cutsceneConcluida == CUTSCENE_DERROTA) {
            cutsceneDerrotaConcluida = true;
            reiniciarMenuDerrota();
        }
        if (
            dicaControlesPendente &&
            !cutsceneEstaAtiva(&cutscenes) &&
            vida > 0 &&
            !noMenu
        ) {
            tempoDicaControles -= fminf(GetFrameTime(), 0.25f);
            if (tempoDicaControles <= 0.0f) {
                mostrarLegenda(
                    "WASD / SETAS: MOVER    E: INTERAGIR    F: FLASH",
                    3.5f
                );
                dicaControlesPendente = false;
            }
        }

        /* Alteração 2: bloco jogoConcluido atualizado */
        if (jogoConcluido) {
            if (IsKeyPressed(KEY_ENTER) ||
                IsKeyPressed(KEY_KP_ENTER) ||
                IsKeyPressed(KEY_SPACE)) {
                jogoConcluido = false;
                cutsceneVitoriaIniciada = false;

                noMenu = true;
                novaPartida = true;
                iniciarPartidaDireto = false;

                bilheteAberto = false;
                dicaControlesPendente = false;
                tempoDicaControles = 0.0f;

                aguardarSoltarConfirmacaoMenu = true;

                iniciarLegendas();

                if (!carregarMenu())
                    erro = true;

                continue;
            }
        }

        if (vida == 0 && cutsceneDerrotaConcluida &&
            !cutsceneEstaAtiva(&cutscenes)) {
            AcaoDerrota acaoDerrota = atualizarMenuDerrota();
            if (acaoDerrota == ACAO_DERROTA_TENTAR_NOVAMENTE) {
                noMenu = true;
                novaPartida = true;
                iniciarPartidaDireto = true;
                iniciarLegendas();
                continue;
            }
            if (acaoDerrota == ACAO_DERROTA_MENU_PRINCIPAL) {
                noMenu = true;
                novaPartida = true;
                iniciarPartidaDireto = false;
                iniciarLegendas();
                if (!carregarMenu())
                    erro = true;
                continue;
            }
            if (acaoDerrota == ACAO_DERROTA_SAIR)
                break;
        }

        if (noMenu) {
            atualizarSomInimigo(&inimigo, salaAtual, personagem, true);

            /* Alteração 3: bloqueio de entrada do menu até soltar Enter/Espaço */
            if (aguardarSoltarConfirmacaoMenu) {
                if (!IsKeyDown(KEY_ENTER) &&
                    !IsKeyDown(KEY_KP_ENTER) &&
                    !IsKeyDown(KEY_SPACE)) {
                    aguardarSoltarConfirmacaoMenu = false;
                }

                BeginDrawing();
                ClearBackground(BLACK);
                desenharMenu();
                EndDrawing();

                continue;
            }

            EstadoSubMenu opcao;
            if (iniciarPartidaDireto) {
                opcao = ESTADO_MENU_INICIAR_JOGO;
                iniciarPartidaDireto = false;
            } else {
                opcao = atualizarMenu();
            }
            if (opcao == ESTADO_MENU_SAIR)
                break;
            if (opcao == ESTADO_MENU_INICIAR_JOGO) {
                if (novaPartida) {
                    liberarTelaInventario(&inventario);
                    iniciarTelaInventario(&inventario);
                    reiniciarObjetosMapa();
                    reiniciarEstadoJogo(&estadoJogo);
                    iniciarInterfaceVHS(&interfaceVHS);
                    iniciarTecladoSaida(&tecladoSaida);
                    iniciarLegendas();
                    personagem = salas[SALAO].entrada;
                    salaAtual = SALAO;
                    vida = 100;
                    danoRecebidoVisual = 0;
                    tempoDanoVisual = 0;
                    posicaoDanoVisual = (Vector2){0};
                    direcao = BAIXO;
                    quadroAnimacao = 0;
                    tempoAnimacao = 0;
                    esperaTransicao = 0;
                    esperaAvisoPorta = 0;
                    escurecimento = 0;
                    brilhoFlash = 0;
                    andando = false;
                    jogoConcluido = false;
                    inimigo.ativo = false;
                    cutsceneDespertarIniciada = false;
                    cutsceneVitoriaIniciada = false;
                    cutsceneDerrotaIniciada = false;
                    cutsceneDerrotaConcluida = false;
                    bilheteAberto = false;
                    dicaControlesPendente = false;
                    tempoDicaControles = 0;
                    if (!iniciarCutscene(&cutscenes, CUTSCENE_INTRODUCAO))
                        erro = true;
                    novaPartida = false;
                }
                noMenu = false;
                continue;
            }
            BeginDrawing();
            ClearBackground(BLACK);
            desenharMenu();
            EndDrawing();
            continue;
        }
        float deltaLegendas = fminf(GetFrameTime(), 0.25f);
        atualizarLegendas(deltaLegendas);
        bool interfaceVHSJaAberta =
            interfaceVHSEstaAberta(&interfaceVHS);
        bool tecladoJaAberto =
            tecladoSaidaEstaAberto(&tecladoSaida);
        bool teclaEsc = IsKeyPressed(KEY_ESCAPE);
        bool bilheteAbertoNoInicio = bilheteAberto;
        if (
            bilheteAberto &&
            (teclaEsc || IsKeyPressed(KEY_E) || IsKeyPressed(KEY_ENTER))
        ) {
            bilheteAberto = false;
            teclaEsc = false;
        }
        bool bilheteBloqueiaJogo =
            bilheteAberto || bilheteAbertoNoInicio;
        if (
            vida > 0 &&
            teclaEsc &&
            !cutsceneEstaAtiva(&cutscenes) &&
            !bilheteBloqueiaJogo &&
            !inventario.aberto &&
            !interfaceVHSJaAberta &&
            !tecladoJaAberto
        ) {
            noMenu = true;
            if (!carregarMenu())
                erro = true;
            continue;
        }
        if (interfaceVHSJaAberta)
            atualizarInterfaceVHS(
                &interfaceVHS,
                &inventario.dados,
                deltaLegendas
            );
        if (tecladoJaAberto)
            atualizarTecladoSaida(
                &tecladoSaida,
                deltaLegendas
            );
        bool interfaceVHSAberta =
            interfaceVHSEstaAberta(&interfaceVHS);
        bool tecladoAberto =
            tecladoSaidaEstaAberto(&tecladoSaida);
        bool teclaTab =
            vida > 0 &&
            !cutsceneEstaAtiva(&cutscenes) &&
            !bilheteBloqueiaJogo &&
            !interfaceVHSAberta &&
            !tecladoAberto &&
            IsKeyPressed(KEY_TAB);
        bool inventarioAberto =
            (cutsceneEstaAtiva(&cutscenes) || bilheteBloqueiaJogo)
            ? false
            : ((interfaceVHSAberta || tecladoAberto)
                ? false
                : atualizarTelaInventario(
                    &inventario,
                    teclaTab,
                    teclaEsc
                ));
        if (inventario.aberto && !teclaTab &&
            !cutsceneEstaAtiva(&cutscenes)) {
            int passoX =
                (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) -
                (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A));
            int passoY =
                (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) -
                (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W));
            Vector2 cursor = telaParaMundo(
                GetMousePosition(),
                areaExibicao(
                    GetScreenWidth(),
                    GetScreenHeight()
                )
            );
            selecionarEspacoInventario(
                &inventario,
                passoX,
                passoY,
                IsMouseButtonPressed(MOUSE_BUTTON_LEFT),
                cursor,
                LARGURA_MUNDO,
                ALTURA_MUNDO
            );
        }
        if (
            vida > 0 &&
            !cutsceneEstaAtiva(&cutscenes) &&
            !bilheteBloqueiaJogo &&
            !tecladoAberto &&
            IsKeyPressed(KEY_F7)
        ) {
            alternarInimigo(
                &inimigo,
                mascaras,
                salaAtual,
                personagem
            );
        }
        bool mudouSala = false;
        if (
            !inventarioAberto &&
            !interfaceVHSAberta &&
            !tecladoAberto &&
            !bilheteBloqueiaJogo &&
            !jogoConcluido &&
            !cutsceneEstaAtiva(&cutscenes) &&
            vida > 0
        ) {
            float tempoQuadro = fminf(GetFrameTime(), 0.25f);
            esperaTransicao = fmaxf(
                0,
                esperaTransicao - tempoQuadro
            );
            esperaAvisoPorta = fmaxf(
                0,
                esperaAvisoPorta - tempoQuadro
            );
            escurecimento = fmaxf(
                0,
                escurecimento - tempoQuadro
            );
            if (IsKeyPressed(KEY_Q)) {
                if (vida >= 100) {
                    mostrarLegenda(
                        "SUA VIDA JA ESTA COMPLETA.",
                        2.5f
                    );
                } else if (
                    inventarioConsumirObjeto(
                        &inventario.dados,
                        ID_KIT_MEDICO
                    )
                ) {
                    int vidaAnterior = vida;
                    vida += 30;
                    if (vida > 100)
                        vida = 100;
                    int vidaRecuperada = vida - vidaAnterior;
                    mostrarLegenda(
                        TextFormat(
                            "KIT MEDICO UTILIZADO. +%d DE VIDA.",
                            vidaRecuperada
                        ),
                        3.0f
                    );
                } else {
                    mostrarLegenda(
                        "VOCE NAO POSSUI UM KIT MEDICO.",
                        2.5f
                    );
                }
            }
            Vector2 movimento = {0};
            if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
                movimento.x -= 1;
            if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
                movimento.x += 1;
            if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))
                movimento.y -= 1;
            if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))
                movimento.y += 1;
            andando = false;
            if (movimento.x != 0 || movimento.y != 0) {
                if (fabsf(movimento.y) > fabsf(movimento.x)) {
                    direcao = movimento.y < 0 ? CIMA : BAIXO;
                } else {
                    direcao = movimento.x < 0 ? ESQUERDA : DIREITA;
                }
                float comprimento = hypotf(
                    movimento.x,
                    movimento.y
                );
                movimento.x /= comprimento;
                movimento.y /= comprimento;
                Vector2 posicaoAnterior = personagem;
                Vector2 deslocamento = {
                    movimento.x * VELOCIDADE_JOHN * tempoQuadro,
                    movimento.y * VELOCIDADE_JOHN * tempoQuadro
                };
                if (
                    tentarCruzarLinhaSaida(
                        salaAtual,
                        posicaoAnterior,
                        deslocamento,
                        tecladoSaidaEstaLiberado(&tecladoSaida)
                    )
                ) {
                    personagem = posicaoAnterior;
                    andando = false;
                    if (!cutsceneVitoriaIniciada) {
                        cutsceneVitoriaIniciada =
                            iniciarCutscene(
                                &cutscenes,
                                CUTSCENE_VITORIA
                            );
                        if (!cutsceneVitoriaIniciada)
                            erro = true;
                    }
                } else {
                    personagem = moverComPersonagem(
                        &mascaras[salaAtual],
                        personagem,
                        deslocamento,
                        inimigo.ativo && inimigo.sala == salaAtual,
                        inimigo.posicao
                    );
                    andando = hypotf(
                        personagem.x - posicaoAnterior.x,
                        personagem.y - posicaoAnterior.y
                    ) > 0.01f;
                    int indicePorta = encontrarPorta(
                        salaAtual,
                        personagem,
                        movimento
                    );
                    bool portaBloqueada =
                        indicePorta >= 0 &&
                        !estadoJogoPodeEntrarSala(
                            &estadoJogo,
                            portas[indicePorta].destino
                        );
                    if (portaBloqueada) {
                        personagem = posicaoAnterior;
                        andando = false;
                        if (esperaAvisoPorta <= 0) {
                            if (portas[indicePorta].destino == ESTOQUE) {
                                mostrarLegenda(
                                    "PORTA TRANCADA. ENCONTRE A CHAVE DO ESTOQUE.",
                                    2.5f
                                );
                            } else if (
                                portas[indicePorta].destino == SALA_PROJECAO
                            ) {
                                mostrarLegenda(
                                    "PORTA TRANCADA. INSTALE O FUSIVEL NO QUADRO ELETRICO.",
                                    2.5f
                                );
                            }
                            esperaAvisoPorta = 2.5f;
                        }
                    } else {
                        mudouSala = trocarSala(
                            &salaAtual,
                            &personagem,
                            &direcao,
                            movimento,
                            &esperaTransicao
                        );
                    }
                    if (mudouSala) {
                        escurecimento = 0.14f;
                        andando = false;
                        TraceLog(
                            LOG_INFO,
                            "Entrada na sala: %s | posicao %.0f, %.0f",
                            salas[salaAtual].nome,
                            personagem.x,
                            personagem.y
                        );
                    }
                }
            }
            if (!cutsceneEstaAtiva(&cutscenes)) {
                brilhoFlash = fmaxf(0, brilhoFlash - tempoQuadro);
                if (
                    IsKeyPressed(KEY_F) &&
                    usarFlash(
                        &inimigo,
                        mascaras,
                        salaAtual,
                        personagem
                    )
                ) {
                    brilhoFlash = 0.2f;
                }
                atualizarInimigo(
                    &inimigo,
                    mascaras,
                    salaAtual,
                    personagem,
                    inventarioContarFitas(&inventario.dados),
                    tempoQuadro
                );
            }
            if (
                !cutsceneEstaAtiva(&cutscenes) &&
                IsKeyPressed(KEY_E) &&
                !interfaceVHSJaAberta &&
                !tecladoJaAberto &&
                !jogoConcluido
            ) {
                if (
                    tecladoSaidaProximo(salaAtual, personagem) &&
                    !tecladoSaidaEstaLiberado(&tecladoSaida)
                ) {
                    abrirTecladoSaida(
                        &tecladoSaida,
                        interfaceVHSCodigo(&interfaceVHS)
                    );
                } else if (
                    tecladoSaidaProximo(salaAtual, personagem) &&
                    tecladoSaidaEstaLiberado(&tecladoSaida)
                ) {
                    mostrarLegenda(
                        "FECHADURA DESTRAVADA. ATRAVESSE A PORTA PARA SAIR.",
                        3.0f
                    );
                } else if (
                    portaSaidaProxima(salaAtual, personagem) &&
                    !tecladoSaidaEstaLiberado(&tecladoSaida)
                ) {
                    mostrarLegenda(
                        "A SAIDA AINDA ESTA BLOQUEADA.",
                        2.5f
                    );
                } else if (aparelhoVhsProximo(salaAtual, personagem)) {
                    abrirInterfaceVHS(&interfaceVHS);
                } else if (quadroEletricoProximo(salaAtual, personagem)) {
                    if (
                        estadoJogoInstalarFusivel(
                            &estadoJogo,
                            &inventario.dados
                        )
                    ) {
                        marcarObjetoConcluido(ID_QUADRO_ELETRICO);
                        mostrarLegenda(
                            "FUSIVEL INSTALADO. A ENERGIA DA SALA DE PROJECAO FOI RESTAURADA.",
                            3.5f
                        );
                    } else if (estadoJogo.fusivelInstalado) {
                        mostrarLegenda(
                            "O FUSIVEL JA ESTA INSTALADO.",
                            2.5f
                        );
                    } else {
                        mostrarLegenda(
                            "VOCE PRECISA ENCONTRAR O FUSIVEL ANTES.",
                            2.5f
                        );
                    }
                } else if (bilheteGerenteProximo(salaAtual, personagem)) {
                    bilheteAberto = true;
                } else {
                    int idColetado = 0;
                    if (
                        coletarObjetoProximo(
                            salaAtual,
                            personagem,
                            &inventario.dados,
                            &idColetado
                        )
                    ) {
                        registrarObjetoColetado(
                            &estadoJogo,
                            idColetado
                        );
                        mostrarLegendaObjeto(idColetado);
                        if (
                            idColetado == ID_FITA_42 &&
                            !inimigo.ativo &&
                            !cutsceneDespertarIniciada
                        ) {
                            alternarInimigo(
                                &inimigo,
                                mascaras,
                                salaAtual,
                                personagem
                            );
                            cutsceneDespertarIniciada =
                                iniciarCutscene(
                                    &cutscenes,
                                    CUTSCENE_DESPERTAR
                                );
                            if (!cutsceneDespertarIniciada)
                                erro = true;
                        }
                    }
                }
            }
            tecladoAberto = tecladoSaidaEstaAberto(&tecladoSaida);
            tempoDanoVisual = fmaxf(
                0.0f,
                tempoDanoVisual - tempoQuadro
            );
            if (!cutsceneEstaAtiva(&cutscenes)) {
                int vidaAntesDoAtaque = vida;
                atacarJohn(
                    &inimigo,
                    mascaras,
                    salaAtual,
                    personagem,
                    inventarioContarFitas(&inventario.dados),
                    tempoQuadro,
                    &vida
                );
                if (vida < vidaAntesDoAtaque) {
                    danoRecebidoVisual = vidaAntesDoAtaque - vida;
                    tempoDanoVisual = DURACAO_DANO_VISUAL;
                    posicaoDanoVisual = (Vector2){
                        personagem.x + 35.0f,
                        personagem.y - ALTURA_JOHN * 0.72f
                    };
                }
            }
            if (vida == 0)
                andando = false;
            if (andando) {
                tempoAnimacao += tempoQuadro;
                while (tempoAnimacao >= duracaoQuadro) {
                    tempoAnimacao -= duracaoQuadro;
                    quadroAnimacao =
                        (quadroAnimacao + 1) % NUM_QUADROS;
                }
            } else {
                quadroAnimacao = 0;
                tempoAnimacao = 0;
            }
            if (IsKeyPressed(KEY_F1))
                mostrarColisoes = !mostrarColisoes;
        }
        if (
            !inventarioAberto &&
            !interfaceVHSAberta &&
            !tecladoAberto &&
            mostrarColisoes &&
            IsMouseButtonPressed(MOUSE_BUTTON_LEFT)
        ) {
            Vector2 ponto = telaParaMundo(
                GetMousePosition(),
                areaExibicao(
                    GetScreenWidth(),
                    GetScreenHeight()
                )
            );
            printf(
                "Ponto: X = %.2f%%, Y = %.2f%% | "
                "Imagem: %.0f, %.0f\n",
                100 * ponto.x / LARGURA_MUNDO,
                100 * ponto.y / ALTURA_MUNDO,
                ponto.x,
                ponto.y
            );
            fflush(stdout);
        }
        atualizarSomInimigo(
            &inimigo,
            salaAtual,
            personagem,
            inventarioAberto ||
            bilheteBloqueiaJogo ||
            interfaceVHSAberta ||
            tecladoAberto ||
            jogoConcluido ||
            cutsceneEstaAtiva(&cutscenes) ||
            vida == 0
        );
        Texture2D personagemAtual =
            andando && !mudouSala
            ? animacoes[direcao][quadroAnimacao]
            : personagemParado[direcao];
        BeginTextureMode(quadro);
        ClearBackground(BLACK);
        DrawTexture(
            cenarios[salaAtual],
            0,
            0,
            WHITE
        );
        if (inimigo.ativo && inimigo.sala == salaAtual) {
            desenharPersonagensComCamadas(
                personagemAtual,
                personagem,
                imagemInimigo(&inimigo),
                inimigo.posicao,
                salaAtual
            );
        } else {
            desenharJohnComCamadas(
                cenarios[salaAtual],
                personagemAtual,
                salaAtual,
                personagem
            );
        }
        desenharObjetosMapa(salaAtual);
        if (tempoDanoVisual > 0.0f && danoRecebidoVisual > 0) {
            float progresso =
                1.0f - tempoDanoVisual / DURACAO_DANO_VISUAL;
            float deslocamento = progresso * 45.0f;
            float transparencia =
                tempoDanoVisual < 0.25f
                ? tempoDanoVisual / 0.25f
                : 1.0f;
            unsigned char alpha =
                (unsigned char)(255.0f * transparencia);
            char textoDano[32];
            snprintf(
                textoDano,
                sizeof textoDano,
                "-%d VIDA",
                danoRecebidoVisual
            );
            int x = (int)posicaoDanoVisual.x;
            int y = (int)(posicaoDanoVisual.y - deslocamento);
            DrawText(
                textoDano,
                x + 2,
                y + 2,
                30,
                (Color){10, 0, 0, alpha}
            );
            DrawText(
                textoDano,
                x,
                y,
                30,
                (Color){255, 65, 65, alpha}
            );
        }
        if (escurecimento > 0) {
            DrawRectangle(
                0,
                0,
                LARGURA_MUNDO,
                ALTURA_MUNDO,
                Fade(BLACK, escurecimento * 2)
            );
        }
        if (mostrarColisoes) {
            desenharDepuracao(
                salaAtual,
                personagem,
                texturasMascaras[salaAtual]
            );
        }
        DrawRectangle(
            18,
            16,
            320,
            94,
            (Color){6, 8, 14, 195}
        );
        DrawText(
            salas[salaAtual].nome,
            30,
            24,
            20,
            RAYWHITE
        );
        DrawText(
            TextFormat(
                "FITAS: %d / 3",
                inventarioContarFitas(&inventario.dados)
            ),
            30,
            52,
            18,
            GOLD
        );
        DrawText(
            TextFormat("VIDA: %d / 100", vida),
            30,
            80,
            18,
            RED
        );
        if (
            !inventarioAberto &&
            !interfaceVHSAberta &&
            !tecladoAberto &&
            !jogoConcluido &&
            vida > 0
        ) {
            desenharIndicacaoColeta(
                salaAtual,
                personagem
            );
        }
        desenharInventario(
            &inventario,
            LARGURA_MUNDO,
            ALTURA_MUNDO
        );
        desenharInterfaceVHS(
            &interfaceVHS,
            &inventario.dados
        );
        desenharTecladoSaida(&tecladoSaida);
        if (jogoConcluido) {
            DrawRectangle(
                0,
                0,
                LARGURA_MUNDO,
                ALTURA_MUNDO,
                (Color){0, 0, 0, 225}
            );
            DrawRectangle(
                330,
                205,
                1012,
                560,
                (Color){8, 10, 16, 250}
            );
            DrawRectangleLinesEx(
                (Rectangle){330, 205, 1012, 560},
                4,
                GOLD
            );
            DrawText(
                "SESSAO ENCERRADA",
                540,
                250,
                62,
                GOLD
            );
            DrawText(
                "VOCE ESCAPOU DA VIDEOLOCADORA AURORA.",
                465,
                345,
                28,
                RAYWHITE
            );
            DrawText(
                "FITAS RECUPERADAS: 3/3",
                535,
                410,
                22,
                LIGHTGRAY
            );
            DrawText(
                "TRANSMISSAO: RECONSTRUIDA",
                535,
                445,
                22,
                LIGHTGRAY
            );
            DrawText(
                inimigo.ativo
                    ? "REBOBINADOR: AINDA ATIVO"
                    : "REBOBINADOR: DERROTADO",
                535,
                480,
                22,
                LIGHTGRAY
            );
            DrawRectangle(
                570,
                555,
                530,
                65,
                (Color){35, 12, 18, 255}
            );
            DrawRectangleLinesEx(
                (Rectangle){570, 555, 530, 65},
                2,
                GOLD
            );
            DrawText(
                "RETORNAR AO MENU",
                650,
                575,
                26,
                GOLD
            );
            DrawText(
                "PRESSIONE ENTER PARA CONTINUAR",
                620,
                650,
                19,
                LIGHTGRAY
            );
        }
        if (brilhoFlash > 0) {
            DrawRectangle(
                0,
                0,
                LARGURA_MUNDO,
                ALTURA_MUNDO,
                Fade(WHITE, brilhoFlash * 3)
            );
        }
        if (
            vida == 0 &&
            cutsceneDerrotaConcluida &&
            !cutsceneEstaAtiva(&cutscenes)
        ) {
            desenharDerrota();
        }
        if (bilheteAberto) {
            desenharBilheteGerente(texturaBilheteGerente);
        } else if (cutsceneEstaAtiva(&cutscenes)) {
            desenharCutscene(&cutscenes);
        }
        EndTextureMode();
        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexturePro(
            quadro.texture,
            (Rectangle){
                0,
                0,
                LARGURA_MUNDO,
                -ALTURA_MUNDO
            },
            areaExibicao(
                GetScreenWidth(),
                GetScreenHeight()
            ),
            (Vector2){0, 0},
            0,
            WHITE
        );
        if (
            vida > 0 &&
            !cutsceneEstaAtiva(&cutscenes) &&
            !bilheteAberto
        ) {
            desenharLegendas();
        }
        EndDrawing();
    }
    if (quadro.id)
        UnloadRenderTexture(quadro);
    for (int sala = 0; sala < NUM_CENARIOS; sala++) {
        if (cenarios[sala].id)
            UnloadTexture(cenarios[sala]);
        if (texturasMascaras[sala].id)
            UnloadTexture(texturasMascaras[sala]);
        liberarColisoes(&mascaras[sala]);
    }
    for (int direcaoSprite = 0;
         direcaoSprite < NUM_DIRECOES;
         direcaoSprite++) {
        if (personagemParado[direcaoSprite].id) {
            UnloadTexture(
                personagemParado[direcaoSprite]
            );
        }
        for (int quadroSprite = 0;
             quadroSprite < NUM_QUADROS;
             quadroSprite++) {
            if (animacoes[direcaoSprite][quadroSprite].id) {
                UnloadTexture(
                    animacoes[direcaoSprite][quadroSprite]
                );
            }
        }
    }
    descarregarMenu();
    liberarCutscenes(&cutscenes);
    if (texturaBilheteGerente.id)
        UnloadTexture(texturaBilheteGerente);
    if (IsMusicValid(musicaJogo))
        UnloadMusicStream(musicaJogo);
    descarregarDerrota();
    liberarInimigo(&inimigo);
    liberarObjetosMapa();
    liberarTelaInventario(&inventario);
    liberarCamadas();
    liberarLegendas();
    if (IsAudioDeviceReady())
        CloseAudioDevice();
    CloseWindow();
    return erro ? 1 : 0;
}