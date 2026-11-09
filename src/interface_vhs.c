#include "raylib.h"
#include "interface_vhs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define QUANTIDADE_FITAS INVENTARIO_MAX_FITAS
#define PAINEL_X 205
#define PAINEL_Y 55
#define PAINEL_LARGURA 1262
#define PAINEL_ALTURA 830
#define LARGURA_CARTAO 350.0f
#define ALTURA_SLOT 130.0f
#define ALTURA_FITA 150.0f

static const Color COR_FUNDO = {8, 10, 16, 250};
static const Color COR_BORDA = {143, 43, 53, 255};
static const Color COR_DESTAQUE = {220, 177, 76, 255};
static const Color COR_TEXTO = {235, 235, 235, 255};
static const Color COR_SECUNDARIA = {145, 148, 156, 255};
static const Color COR_VERDE = {70, 190, 125, 255};
static const Color COR_VERMELHO = {220, 65, 75, 255};

static void definirMensagem(InterfaceVHS *interface, const char *texto, float duracao) {
    if (!interface || !texto)
        return;

    snprintf(interface->mensagem, sizeof interface->mensagem, "%s", texto);
    interface->tempoMensagem = duracao;
}

static int contarInseridas(const InterfaceVHS *interface) {
    int total = 0;

    for (int i = 0; i < QUANTIDADE_FITAS; i++) {
        if (interface->fitasInseridas[i] != 0)
            total++;
    }

    return total;
}

static bool fitaEstaInserida(const InterfaceVHS *interface, int id) {
    if (!interface || id <= 0)
        return false;

    for (int i = 0; i < QUANTIDADE_FITAS; i++) {
        if (interface->fitasInseridas[i] == id)
            return true;
    }

    return false;
}

static int proximaFitaDisponivel(
    const InterfaceVHS *interface,
    const Inventario *inventario,
    int inicio,
    int sentido
) {
    if (!interface || !inventario)
        return -1;

    int total = inventarioContarFitas(inventario);
    if (total <= 0 || total > QUANTIDADE_FITAS)
        return -1;

    for (int tentativa = 0; tentativa < total; tentativa++) {
        int indice = (inicio + sentido * tentativa) % total;
        if (indice < 0)
            indice += total;

        const Fita *fita = fitaNaPosicao(inventario->fitas, indice);
        if (fita && !fitaEstaInserida(interface, fita->id))
            return indice;
    }

    return -1;
}

static Fita *copiarListaFitas(const Fita *original) {
    Fita *inicio = NULL;
    Fita *fim = NULL;

    while (original) {
        Fita *nova = malloc(sizeof(Fita));

        if (!nova) {
            liberarLista(&inicio);
            return NULL;
        }

        *nova = *original;
        nova->proxima = NULL;

        if (!inicio)
            inicio = nova;
        else
            fim->proxima = nova;

        fim = nova;
        original = original->proxima;
    }

    return inicio;
}

static void reiniciarTentativa(InterfaceVHS *interface, const Inventario *inventario) {
    if (!interface)
        return;

    memset(interface->fitasInseridas, 0, sizeof interface->fitasInseridas);
    interface->etapaInsercao = 0;
    interface->sequenciaIncorreta = false;
    interface->fitaSelecionada = proximaFitaDisponivel(interface, inventario, 0, 1);
    interface->mensagem[0] = '\0';
    interface->tempoMensagem = 0.0f;
}

static bool verificarSequencia(InterfaceVHS *interface, Inventario *inventario) {
    if (!interface || !inventario)
        return false;

    if (inventarioContarFitas(inventario) != QUANTIDADE_FITAS ||
        contarInseridas(interface) != QUANTIDADE_FITAS) {
        return false;
    }

    Fita *listaOrdenada = copiarListaFitas(inventario->fitas);

    if (!listaOrdenada) {
        definirMensagem(interface, "ERRO AO PROCESSAR A SEQUENCIA.", 3.0f);
        return false;
    }

    ordenarFitasPorInsercao(&listaOrdenada);

    const Fita *atual = listaOrdenada;

    for (int i = 0; i < QUANTIDADE_FITAS; i++) {
        if (!atual || interface->fitasInseridas[i] != atual->id) {
            liberarLista(&listaOrdenada);
            interface->sequenciaIncorreta = true;
            definirMensagem(
                interface,
                "SEQUENCIA INCORRETA - ENTER PARA TENTAR NOVAMENTE.",
                0.0f
            );
            return false;
        }

        atual = atual->proxima;
    }

    atual = listaOrdenada;

    for (int i = 0; i < QUANTIDADE_FITAS; i++) {
        if (!atual || atual->digito < 0 || atual->digito > 9) {
            liberarLista(&listaOrdenada);
            definirMensagem(
                interface,
                "NAO FOI POSSIVEL RECONSTRUIR A TRANSMISSAO.",
                0.0f
            );
            return false;
        }

        interface->codigo[i] = (char)('0' + atual->digito);
        atual = atual->proxima;
    }

    interface->codigo[QUANTIDADE_FITAS] = '\0';

    liberarLista(&listaOrdenada);

    interface->ordenado = true;
    interface->transmissaoReconstruida = true;
    interface->sequenciaIncorreta = false;
    definirMensagem(interface, "TRANSMISSAO RECONSTRUIDA COM SUCESSO.", 0.0f);

    return true;
}

static void inserirFitaSelecionada(InterfaceVHS *interface, Inventario *inventario) {
    if (!interface || !inventario)
        return;

    int total = inventarioContarFitas(inventario);

    if (total < QUANTIDADE_FITAS) {
        definirMensagem(
            interface,
            "RECUPERE AS TRES FITAS ANTES DE USAR O APARELHO.",
            3.0f
        );
        return;
    }

    if (interface->etapaInsercao >= QUANTIDADE_FITAS)
        return;

    if (interface->fitaSelecionada < 0 ||
        interface->fitaSelecionada >= total) {
        definirMensagem(interface, "SELECIONE UMA FITA DISPONIVEL.", 2.5f);
        return;
    }

    const Fita *fita = fitaNaPosicao(
        inventario->fitas,
        interface->fitaSelecionada
    );

    if (!fita) {
        definirMensagem(interface, "NAO FOI POSSIVEL LOCALIZAR A FITA.", 2.5f);
        return;
    }

    if (fitaEstaInserida(interface, fita->id)) {
        definirMensagem(interface, "ESSA FITA JA FOI INSERIDA.", 2.5f);
        return;
    }

    int posicao = interface->etapaInsercao;
    interface->fitasInseridas[posicao] = fita->id;
    interface->etapaInsercao++;

    interface->mensagem[0] = '\0';
    interface->tempoMensagem = 0.0f;

    if (interface->etapaInsercao < QUANTIDADE_FITAS) {
        interface->fitaSelecionada = proximaFitaDisponivel(
            interface,
            inventario,
            interface->fitaSelecionada + 1,
            1
        );
        return;
    }

    verificarSequencia(interface, inventario);
}

void iniciarInterfaceVHS(InterfaceVHS *interface) {
    if (!interface)
        return;

    *interface = (InterfaceVHS){0};
    interface->fitaSelecionada = -1;
}

void abrirInterfaceVHS(InterfaceVHS *interface) {
    if (!interface)
        return;

    interface->aberto = true;
    interface->mensagem[0] = '\0';
    interface->tempoMensagem = 0.0f;
}

void atualizarInterfaceVHS(
    InterfaceVHS *interface,
    Inventario *inventario,
    float delta
) {
    if (!interface || !interface->aberto || !inventario)
        return;

    if (delta < 0.0f)
        delta = 0.0f;
    if (delta > 0.25f)
        delta = 0.25f;

    if (interface->tempoMensagem > 0.0f) {
        interface->tempoMensagem -= delta;
        if (interface->tempoMensagem <= 0.0f) {
            interface->tempoMensagem = 0.0f;
            interface->mensagem[0] = '\0';
        }
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        interface->aberto = false;
        return;
    }

    if (interface->transmissaoReconstruida) {
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_E))
            interface->aberto = false;
        return;
    }

    if (interface->sequenciaIncorreta) {
        if (IsKeyPressed(KEY_ENTER))
            reiniciarTentativa(interface, inventario);
        return;
    }

    int total = inventarioContarFitas(inventario);

    if (total < QUANTIDADE_FITAS) {
        interface->fitaSelecionada = -1;
        return;
    }

    if (total > QUANTIDADE_FITAS)
        return;

    if (interface->etapaInsercao >= QUANTIDADE_FITAS)
        return;

    if (interface->fitaSelecionada < 0 ||
        interface->fitaSelecionada >= total) {
        interface->fitaSelecionada = proximaFitaDisponivel(
            interface,
            inventario,
            0,
            1
        );
    } else {
        const Fita *selecionada = fitaNaPosicao(
            inventario->fitas,
            interface->fitaSelecionada
        );

        if (!selecionada || fitaEstaInserida(interface, selecionada->id)) {
            interface->fitaSelecionada = proximaFitaDisponivel(
                interface,
                inventario,
                0,
                1
            );
        }
    }

    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
        interface->fitaSelecionada = proximaFitaDisponivel(
            interface,
            inventario,
            interface->fitaSelecionada - 1,
            -1
        );
    }

    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
        interface->fitaSelecionada = proximaFitaDisponivel(
            interface,
            inventario,
            interface->fitaSelecionada + 1,
            1
        );
    }

    if (IsKeyPressed(KEY_ENTER))
        inserirFitaSelecionada(interface, inventario);
}

static void desenharMoldura(void) {
    DrawRectangle(0, 0, 1672, 941, (Color){0, 0, 0, 210});

    DrawRectangle(
        PAINEL_X,
        PAINEL_Y,
        PAINEL_LARGURA,
        PAINEL_ALTURA,
        COR_FUNDO
    );

    DrawRectangleLinesEx(
        (Rectangle){PAINEL_X, PAINEL_Y, PAINEL_LARGURA, PAINEL_ALTURA},
        3.0f,
        COR_BORDA
    );

    DrawText(
        "RECONSTRUCAO DA TRANSMISSAO",
        270,
        105,
        34,
        COR_TEXTO
    );

    DrawRectangle(270, 157, 1125, 2, COR_BORDA);
}

static void desenharSlot(
    const InterfaceVHS *interface,
    const Inventario *inventario,
    int indice
) {
    float x = 270.0f + indice * 385.0f;
    float y = 245.0f;

    Rectangle caixa = {x, y, LARGURA_CARTAO, ALTURA_SLOT};
    bool preenchido = interface->fitasInseridas[indice] != 0;
    bool ativo = indice == interface->etapaInsercao &&
                 !interface->sequenciaIncorreta &&
                 !interface->transmissaoReconstruida;

    Color borda = preenchido
        ? COR_VERDE
        : (ativo ? COR_DESTAQUE : (Color){75, 78, 88, 255});

    DrawRectangleRec(caixa, (Color){17, 20, 27, 255});
    DrawRectangleLinesEx(caixa, ativo ? 3.0f : 2.0f, borda);

    DrawText(
        TextFormat("POSICAO %d", indice + 1),
        (int)x + 16,
        (int)y + 13,
        18,
        COR_DESTAQUE
    );

    if (preenchido) {
        const Fita *fita = buscarFita(
            inventario->fitas,
            interface->fitasInseridas[indice]
        );

        if (fita) {
            DrawText(fita->nome, (int)x + 18, (int)y + 55, 23, COR_TEXTO);
            DrawText(
                TextFormat("%02d MHz", fita->frequencia),
                (int)x + 18,
                (int)y + 91,
                18,
                SKYBLUE
            );
        }
    } else {
        DrawText(
            "ESPACO VAZIO",
            (int)x + 18,
            (int)y + 62,
            20,
            COR_SECUNDARIA
        );
    }
}

static void desenharFitaDisponivel(
    const InterfaceVHS *interface,
    const Inventario *inventario,
    int indice
) {
    float x = 270.0f + indice * 385.0f;
    float y = 565.0f;

    const Fita *fita = fitaNaPosicao(inventario->fitas, indice);
    bool selecionada = indice == interface->fitaSelecionada;
    bool inserida = fita && fitaEstaInserida(interface, fita->id);

    Rectangle caixa = {x, y, LARGURA_CARTAO, ALTURA_FITA};

    Color borda = selecionada
        ? COR_DESTAQUE
        : (inserida ? COR_VERDE : (Color){75, 78, 88, 255});

    DrawRectangleRec(caixa, (Color){17, 20, 27, 255});
    DrawRectangleLinesEx(caixa, selecionada ? 3.0f : 2.0f, borda);

    if (!fita) {
        DrawText("FITA INDISPONIVEL", (int)x + 18, (int)y + 55, 18, GRAY);
        return;
    }

    DrawText(
        fita->nome,
        (int)x + 18,
        (int)y + 20,
        21,
        COR_TEXTO
    );

    DrawRectangle(
        (int)x + 18,
        (int)y + 58,
        314,
        43,
        (Color){34, 38, 47, 255}
    );

    DrawCircle(
        (int)x + 105,
        (int)y + 79,
        13,
        (Color){120, 124, 133, 255}
    );

    DrawCircle(
        (int)x + 245,
        (int)y + 79,
        13,
        (Color){120, 124, 133, 255}
    );

    DrawText(
        TextFormat("%02d MHz", fita->frequencia),
        (int)x + 18,
        (int)y + 112,
        18,
        SKYBLUE
    );

    if (inserida) {
        DrawText("INSERIDA", (int)x + 230, (int)y + 112, 16, COR_VERDE);
    } else if (selecionada) {
        DrawText("SELECIONADA", (int)x + 190, (int)y + 112, 16, COR_DESTAQUE);
    }
}

static void desenharMensagem(const InterfaceVHS *interface) {
    if (!interface->mensagem[0])
        return;

    Color cor = interface->sequenciaIncorreta
        ? COR_VERMELHO
        : COR_TEXTO;

    if (interface->transmissaoReconstruida)
        cor = COR_VERDE;

    DrawRectangle(
        270,
        755,
        1125,
        43,
        (Color){20, 15, 20, 245}
    );

    DrawRectangleLinesEx(
        (Rectangle){270, 755, 1125, 43},
        1.0f,
        cor
    );

    int larguraTexto = MeasureText(interface->mensagem, 19);

    DrawText(
        interface->mensagem,
        270 + (1125 - larguraTexto) / 2,
        767,
        19,
        cor
    );
}

static void desenharTelaInsercao(
    const InterfaceVHS *interface,
    const Inventario *inventario
) {
    DrawText(
        "INSIRA AS FITAS NA ORDEM CORRETA",
        270,
        185,
        20,
        COR_SECUNDARIA
    );

    DrawText(
        "SEQUENCIA DE REPRODUCAO",
        270,
        215,
        22,
        COR_TEXTO
    );

    for (int i = 0; i < QUANTIDADE_FITAS; i++)
        desenharSlot(interface, inventario, i);

    DrawText(
        "SUAS FITAS",
        270,
        520,
        22,
        COR_TEXTO
    );

    int total = inventarioContarFitas(inventario);

    for (int i = 0; i < QUANTIDADE_FITAS; i++) {
        if (i < total)
            desenharFitaDisponivel(interface, inventario, i);
    }

    if (total < QUANTIDADE_FITAS) {
        DrawText(
            TextFormat("FITAS RECUPERADAS: %d/3 - RECUPERE TODAS PARA CONTINUAR.", total),
            270,
            730,
            17,
            COR_DESTAQUE
        );
    } else if (!interface->sequenciaIncorreta &&
               !interface->transmissaoReconstruida) {
        DrawText(
            TextFormat("ESCOLHA A FITA %d DE 3", interface->etapaInsercao + 1),
            270,
            490,
            19,
            COR_DESTAQUE
        );
    }

    desenharMensagem(interface);

    if (interface->sequenciaIncorreta) {
        DrawText(
            "ENTER: TENTAR NOVAMENTE",
            270,
            820,
            18,
            COR_VERMELHO
        );
    } else {
        DrawText(
            "A/D OU SETAS: ESCOLHER FITA  |  ENTER: INSERIR  |  ESC: SAIR",
            270,
            820,
            17,
            COR_SECUNDARIA
        );
    }
}

static void desenharTelaSucesso(const InterfaceVHS *interface) {
    DrawText(
        "TRANSMISSAO RECONSTRUIDA",
        270,
        170,
        35,
        COR_VERDE
    );

    DrawRectangle(270, 225, 1125, 2, COR_VERDE);

    DrawText(
        "AS TRES FITAS ESTAO NA ORDEM CORRETA.",
        270,
        290,
        23,
        COR_TEXTO
    );

    DrawText(
        "CODIGO DE ACESSO DA PORTA",
        270,
        390,
        21,
        COR_SECUNDARIA
    );

    DrawRectangleRec(
        (Rectangle){470, 455, 720, 150},
        (Color){12, 24, 19, 255}
    );

    DrawRectangleLinesEx(
        (Rectangle){470, 455, 720, 150},
        3.0f,
        COR_VERDE
    );

    char codigoFormatado[16] = {
        interface->codigo[0], ' ', '-', ' ',
        interface->codigo[1], ' ', '-', ' ',
        interface->codigo[2], '\0'
    };

    int larguraTexto = MeasureText(codigoFormatado, 72);

    DrawText(
        codigoFormatado,
        (1672 - larguraTexto) / 2,
        492,
        72,
        COR_VERDE
    );

    DrawText(
        "PRESSIONE ENTER PARA FECHAR",
        270,
        820,
        18,
        COR_SECUNDARIA
    );
}

void desenharInterfaceVHS(
    const InterfaceVHS *interface,
    const Inventario *inventario
) {
    if (!interface || !interface->aberto || !inventario)
        return;

    desenharMoldura();

    if (interface->transmissaoReconstruida) {
        desenharTelaSucesso(interface);
        return;
    }

    desenharTelaInsercao(interface, inventario);
}

bool interfaceVHSEstaAberta(const InterfaceVHS *interface) {
    return interface && interface->aberto;
}

bool interfaceVHSReconstruida(const InterfaceVHS *interface) {
    return interface && interface->transmissaoReconstruida;
}

const char *interfaceVHSCodigo(const InterfaceVHS *interface) {
    if (!interface || !interface->transmissaoReconstruida)
        return "";

    return interface->codigo;
}