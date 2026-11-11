
#include "cutscenes.h"
#include "mundo.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

enum ImagemCutscene {
    IMG_CUT1_1,
    IMG_CUT1_2,
    IMG_CUT1_3,
    IMG_CUT1_4,
    IMG_CUT2_1,
    IMG_CUT2_2,
    IMG_CUT2_3,
    IMG_CUT2_4,
    IMG_CUT2_5,
    IMG_CUT2_6,
    IMG_CUT2_7,
    IMG_CUT2_8,
    IMG_CUT2_9,
    IMG_CUT3_1,
    IMG_CUT3_2,
    IMG_CUT3_3,
    IMG_CUT4_1,
    IMG_CUT4_2,
    IMG_CUT4_3,
    IMG_CUT4_4,
    IMG_CUT4_5,
    IMG_CUT4_6,
    IMG_CUT5_1,
    IMG_CUT5_2,
    IMG_CUT5_3,
    IMG_CUT5_4,
    IMG_CUT5_5,
    IMG_CUT5_6,
    IMG_CUT5_7,
    IMG_CUT5_8,
    IMG_CUT5_9,
    IMG_CUT5_10,
    IMG_CUT6_INTERFERENCIA,
    IMG_CUT6_ABRE_SAIDA,
    IMG_CUT6_LUZES_APAGADAS,
    IMG_CUT6_ESCAPOU,
    IMG_CUT6_INDO_EMBORA,
    IMG_CUT6_TV_FINAL1,
    IMG_CUT6_TV_FINAL2,
    IMG_CUT7_SINAL_PERDIDO
};

typedef enum {
    ETAPA_IMAGEM,
    ETAPA_CARTELA,
    ETAPA_DESPERTAR_ESCURO,
    ETAPA_DESPERTAR_INTERFERENCIA,
    ETAPA_DESPERTAR_PRESENCA
} TipoEtapa;

typedef struct {
    int imagem;
    float duracao;
    float zoomInicial;
    float zoomFinal;
    TipoEtapa tipo;
    const char *titulo;
    const char *texto;
    const char *legenda;
} Etapa;

#define IMG(id, tempo, zoom1, zoom2, legendaTexto) \
    {id, tempo, zoom1, zoom2, ETAPA_IMAGEM, NULL, NULL, legendaTexto}

#define CARTELA(tempo, tituloTexto, corpoTexto) \
    {-1, tempo, 1.0f, 1.0f, ETAPA_CARTELA, tituloTexto, corpoTexto, NULL}

#define FX(tempo, tipoEtapa, tituloTexto, corpoTexto) \
    {-1, tempo, 1.0f, 1.0f, tipoEtapa, tituloTexto, corpoTexto, NULL}

static const char *const prefixos[CUTSCENE_TOTAL_IMAGENS] = {
    "CUT1.1_",
    "CUT1.2_",
    "CUT1.3_",
    "CUT1.4_",
    "CUT2.1_",
    "CUT2.2_",
    "CUT2.3_",
    "CUT2.4",
    "CUT2.5_",
    "CUT2.6",
    "CUT2.7_",
    "CUT2.8_",
    "CUT2.9_",
    "CUT3.1_",
    "CUT3.2_",
    "CUT3.3_",
    "CUT4.1_",
    "CUT4.2_",
    "CUT4.3_",
    "CUT4.4_",
    "CUT4.5_",
    "CUT4.6_",
    "CUT5.1_",
    "CUT5.2_",
    "CUT5.3_",
    "CUT5.4_",
    "CUT5.5_",
    "CUT5.6_",
    "CUT5.7_",
    "CUT5.8_",
    "CUT5.9_",
    "CUT5.10_",
    "CUT6_interferencia_encerramento",
    "CUT6_john_abre_saida",
    "CUT6_john_escapou_luzes_apagadas",
    "CUT6_john_escapou.png",
    "CUT6_john_indo_embora",
    "CUT6_televisao_religa_final1",
    "CUT6_televisao_religa_final2",
    "CUT7_sinal_perdido"
};

static const Etapa introducao[] = {
    IMG(IMG_CUT1_1, 3.0f, 1.00f, 1.00f, NULL),
    IMG(IMG_CUT1_2, 3.0f, 1.00f, 1.09f, NULL),
    IMG(IMG_CUT1_3, 4.0f, 1.00f, 1.12f, NULL),
    IMG(IMG_CUT1_4, 3.0f, 1.00f, 1.00f, NULL),
    IMG(IMG_CUT2_1, 0.2f, 1.00f, 1.00f, NULL),
    IMG(IMG_CUT2_2, 0.2f, 1.00f, 1.00f, NULL),
    IMG(IMG_CUT2_3, 0.2f, 1.00f, 1.00f, NULL),
    IMG(IMG_CUT2_1, 0.2f, 1.00f, 1.00f, NULL),
    IMG(IMG_CUT2_2, 0.2f, 1.00f, 1.00f, NULL),
    IMG(IMG_CUT2_4, 3.0f, 1.00f, 1.04f, NULL),
    IMG(IMG_CUT2_5, 3.0f, 1.00f, 1.04f, NULL),
    IMG(IMG_CUT2_6, 2.5f, 1.00f, 1.06f, NULL),
    IMG(IMG_CUT2_7, 2.5f, 1.00f, 1.06f, NULL),
    IMG(IMG_CUT2_8, 3.0f, 1.00f, 1.08f, NULL),
    IMG(IMG_CUT2_9, 1.5f, 1.00f, 1.00f, NULL),
    IMG(IMG_CUT3_1, 3.0f, 1.00f, 1.04f, NULL),
    IMG(IMG_CUT3_2, 3.0f, 1.00f, 1.04f, NULL),
    CARTELA(2.5f, "23:47 - VIDEOLOCADORA AURORA", NULL),
    CARTELA(3.5f, NULL, "Naquela mesma noite, John foi ate a locadora\npara descobrir o que havia acontecido."),
    IMG(IMG_CUT3_3, 3.0f, 1.00f, 1.08f, NULL),
    IMG(IMG_CUT4_1, 3.0f, 1.00f, 1.05f, NULL),
    IMG(IMG_CUT4_2, 0.2f, 1.00f, 1.00f, NULL),
    IMG(IMG_CUT4_3, 0.2f, 1.00f, 1.00f, NULL),
    IMG(IMG_CUT4_2, 0.2f, 1.00f, 1.00f, NULL),
    IMG(IMG_CUT4_3, 0.2f, 1.00f, 1.00f, NULL),
    IMG(IMG_CUT4_2, 0.2f, 1.00f, 1.00f, NULL),
    IMG(IMG_CUT4_3, 0.2f, 1.00f, 1.00f, NULL),
    IMG(IMG_CUT4_4, 3.0f, 1.00f, 1.04f, "ENERGIA INSTAVEL."),
    IMG(IMG_CUT4_5, 3.0f, 1.00f, 1.04f, "FECHADURA BLOQUEADA."),
    IMG(IMG_CUT4_6, 3.0f, 1.00f, 1.04f, "SAIDA INDISPONIVEL.")
};

static const Etapa missao[] = {
    IMG(IMG_CUT5_1, 2.0f, 1.00f, 1.03f, NULL),
    IMG(IMG_CUT5_2, 2.5f, 1.00f, 1.05f, "[VOZ DISTORCIDA] Tres fragmentos. Tres frequencias."),
    IMG(IMG_CUT5_3, 2.5f, 1.00f, 1.05f, "[VOZ DISTORCIDA] A ordem em que voce os encontrar nao importa."),
    IMG(IMG_CUT5_4, 2.5f, 1.00f, 1.05f, "[VOZ DISTORCIDA] Reconstrua a transmissao. So entao a saida sera liberada."),
    IMG(IMG_CUT5_5, 1.0f, 1.00f, 1.00f, NULL),
    IMG(IMG_CUT5_6, 1.5f, 1.00f, 1.00f, NULL),
    IMG(IMG_CUT5_7, 4.0f, 1.00f, 1.00f, NULL),
    IMG(IMG_CUT5_8, 0.2f, 1.00f, 1.00f, NULL),
    IMG(IMG_CUT5_9, 0.2f, 1.00f, 1.00f, NULL),
    IMG(IMG_CUT5_10, 3.0f, 1.00f, 1.00f, NULL)
};

static const Etapa despertar[] = {
    FX(0.5f, ETAPA_DESPERTAR_ESCURO, "SINAL DETECTADO.", NULL),
    FX(0.25f, ETAPA_DESPERTAR_INTERFERENCIA, NULL, NULL),
    FX(2.0f, ETAPA_DESPERTAR_PRESENCA, "[TRANSMISSAO]", "VOCE NAO DEVERIA TER REBOBINADO ISSO.")
};

static const Etapa vitoria[] = {
    IMG(IMG_CUT6_ABRE_SAIDA, 3.0f, 1.00f, 1.04f, NULL),
    IMG(IMG_CUT6_ESCAPOU, 2.0f, 1.00f, 1.00f, NULL),
    IMG(IMG_CUT6_LUZES_APAGADAS, 2.5f, 1.00f, 1.00f, NULL),
    IMG(IMG_CUT6_INDO_EMBORA, 3.0f, 1.00f, 1.06f, NULL),
    CARTELA(2.0f, "SESSAO ENCERRADA", NULL),
    CARTELA(2.0f, NULL, "John conseguiu escapar da locadora. A transmissao foi\ninterrompida e os ruidos desapareceram."),
    CARTELA(3.5f, NULL, "Mas, antes do amanhecer, um dos televisores da loja\nvoltou a ligar sozinho."),
    IMG(IMG_CUT6_TV_FINAL1, 2.0f, 1.00f, 1.00f, NULL),
    IMG(IMG_CUT6_TV_FINAL2, 2.0f, 1.00f, 1.00f, NULL),
    IMG(IMG_CUT6_INTERFERENCIA, 1.5f, 1.00f, 1.00f, NULL)
};

static const Etapa derrota[] = {
    IMG(IMG_CUT7_SINAL_PERDIDO, 1.0f, 1.00f, 1.00f, NULL),
    CARTELA(0.9f, "SINAL PERDIDO", NULL),
    CARTELA(1.8f, "JOHN FOI ABSORVIDO PELA FITA.", "A sessao terminou antes que a transmissao pudesse ser interrompida.")
};

static const Etapa *obterEtapas(TipoCutscene tipo, int *quantidade) {
    if (!quantidade)
        return NULL;

    switch (tipo) {
        case CUTSCENE_INTRODUCAO:
            *quantidade = (int)(sizeof introducao / sizeof introducao[0]);
            return introducao;

        case CUTSCENE_MISSAO:
            *quantidade = (int)(sizeof missao / sizeof missao[0]);
            return missao;

        case CUTSCENE_DESPERTAR:
            *quantidade = (int)(sizeof despertar / sizeof despertar[0]);
            return despertar;

        case CUTSCENE_VITORIA:
            *quantidade = (int)(sizeof vitoria / sizeof vitoria[0]);
            return vitoria;

        case CUTSCENE_DERROTA:
            *quantidade = (int)(sizeof derrota / sizeof derrota[0]);
            return derrota;

        default:
            *quantidade = 0;
            return NULL;
    }
}

static void descarregarTexturaAtual(SistemaCutscenes *sistema) {
    if (!sistema)
        return;

    if (sistema->texturaAtual.id)
        UnloadTexture(sistema->texturaAtual);

    sistema->texturaAtual = (Texture2D){0};
    sistema->imagemAtual = -1;
}

static bool carregarTexturaEtapa(SistemaCutscenes *sistema, int imagem) {
    if (!sistema || imagem < 0 || imagem >= CUTSCENE_TOTAL_IMAGENS)
        return false;

    if (sistema->imagemAtual == imagem && sistema->texturaAtual.id)
        return true;

    descarregarTexturaAtual(sistema);

    if (sistema->caminhos[imagem][0] == '\0')
        return false;

    sistema->texturaAtual = LoadTexture(sistema->caminhos[imagem]);

    if (!sistema->texturaAtual.id) {
        TraceLog(LOG_ERROR, "Nao foi possivel carregar a cutscene: %s", sistema->caminhos[imagem]);
        return false;
    }

    SetTextureFilter(sistema->texturaAtual, TEXTURE_FILTER_POINT);
    sistema->imagemAtual = imagem;
    return true;
}

bool carregarCutscenes(SistemaCutscenes *sistema) {
    if (!sistema)
        return false;

    memset(sistema, 0, sizeof *sistema);
    sistema->imagemAtual = -1;

    FilePathList arquivos = LoadDirectoryFiles("assets/cutscenes");

    if (arquivos.count == 0) {
        TraceLog(LOG_ERROR, "A pasta assets/cutscenes esta vazia ou nao foi encontrada.");
        UnloadDirectoryFiles(arquivos);
        return false;
    }

    bool sucesso = true;

    for (int imagem = 0; imagem < CUTSCENE_TOTAL_IMAGENS; imagem++) {
        bool encontrada = false;
        size_t tamanhoPrefixo = strlen(prefixos[imagem]);

        for (unsigned int indice = 0; indice < arquivos.count; indice++) {
            const char *caminho = arquivos.paths[indice];
            const char *nome = strrchr(caminho, '/');
            nome = nome ? nome + 1 : caminho;

            if (strncmp(nome, prefixos[imagem], tamanhoPrefixo) == 0) {
                snprintf(
                    sistema->caminhos[imagem],
                    sizeof sistema->caminhos[imagem],
                    "%s",
                    caminho
                );

                encontrada = true;
                break;
            }
        }

        if (!encontrada) {
            TraceLog(LOG_ERROR, "Imagem de cutscene nao encontrada. Prefixo esperado: %s",
                     prefixos[imagem]);
            sucesso = false;
        }
    }

    UnloadDirectoryFiles(arquivos);

    if (!sucesso)
        memset(sistema->caminhos, 0, sizeof sistema->caminhos);

    return sucesso;
}

bool iniciarCutscene(SistemaCutscenes *sistema, TipoCutscene tipo) {
    if (!sistema)
        return false;

    int quantidade = 0;
    const Etapa *etapas = obterEtapas(tipo, &quantidade);

    if (!etapas || quantidade <= 0)
        return false;

    descarregarTexturaAtual(sistema);

    sistema->tipo = tipo;
    sistema->indiceEtapa = 0;
    sistema->tempoEtapa = 0.0f;
    sistema->ativa = true;
    sistema->concluida = CUTSCENE_NENHUMA;

    if (etapas[0].tipo == ETAPA_IMAGEM &&
        !carregarTexturaEtapa(sistema, etapas[0].imagem)) {
        sistema->ativa = false;
        sistema->tipo = CUTSCENE_NENHUMA;
        return false;
    }

    return true;
}

void atualizarCutscene(SistemaCutscenes *sistema, float delta) {
    if (!sistema || !sistema->ativa || delta <= 0.0f)
        return;

    int quantidade = 0;
    const Etapa *etapas = obterEtapas(sistema->tipo, &quantidade);

    if (!etapas || quantidade <= 0) {
        sistema->ativa = false;
        sistema->tipo = CUTSCENE_NENHUMA;
        return;
    }

    delta = fminf(delta, 0.25f);
    sistema->tempoEtapa += delta;

    while (sistema->ativa &&
           sistema->tempoEtapa >= etapas[sistema->indiceEtapa].duracao) {
        sistema->tempoEtapa -= etapas[sistema->indiceEtapa].duracao;
        sistema->indiceEtapa++;

        if (sistema->indiceEtapa >= quantidade) {
            sistema->ativa = false;
            sistema->concluida = sistema->tipo;
            sistema->tipo = CUTSCENE_NENHUMA;
            descarregarTexturaAtual(sistema);
            return;
        }

        if (etapas[sistema->indiceEtapa].tipo == ETAPA_IMAGEM &&
            !carregarTexturaEtapa(sistema, etapas[sistema->indiceEtapa].imagem)) {
            sistema->ativa = false;
            sistema->concluida = sistema->tipo;
            sistema->tipo = CUTSCENE_NENHUMA;
            descarregarTexturaAtual(sistema);
            return;
        }

        if (etapas[sistema->indiceEtapa].tipo != ETAPA_IMAGEM)
            descarregarTexturaAtual(sistema);
    }
}

static float limitar(float valor, float minimo, float maximo) {
    if (valor < minimo)
        return minimo;

    if (valor > maximo)
        return maximo;

    return valor;
}

static void desenharTextoCentralizado(
    const char *texto,
    int y,
    int tamanho,
    Color cor
) {
    if (!texto)
        return;

    const char *inicio = texto;
    char linha[512];

    while (*inicio) {
        const char *fim = strchr(inicio, '\n');
        size_t quantidade = fim ? (size_t)(fim - inicio) : strlen(inicio);

        if (quantidade >= sizeof linha)
            quantidade = sizeof linha - 1;

        memcpy(linha, inicio, quantidade);
        linha[quantidade] = '\0';

        int largura = MeasureText(linha, tamanho);
        DrawText(linha, (LARGURA_MUNDO - largura) / 2, y, tamanho, cor);

        y += tamanho + 16;

        if (!fim)
            break;

        inicio = fim + 1;
    }
}

static void desenharCartela(const Etapa *etapa) {
    DrawRectangle(0, 0, LARGURA_MUNDO, ALTURA_MUNDO, BLACK);
    DrawRectangle(0, 0, LARGURA_MUNDO, 7, (Color){110, 12, 24, 255});
    DrawRectangle(0, ALTURA_MUNDO - 7, LARGURA_MUNDO, 7, (Color){110, 12, 24, 255});

    if (etapa->titulo) {
        desenharTextoCentralizado(
            etapa->titulo,
            ALTURA_MUNDO / 2 - 75,
            42,
            (Color){230, 45, 55, 255}
        );
    }

    if (etapa->texto) {
        int quantidadeLinhas = 1;

        for (const char *p = etapa->texto; *p; p++)
            if (*p == '\n')
                quantidadeLinhas++;

        int altura = quantidadeLinhas * 42;

        desenharTextoCentralizado(
            etapa->texto,
            (ALTURA_MUNDO - altura) / 2,
            28,
            RAYWHITE
        );
    }
}

static void desenharLegendaCutscene(const char *texto) {
    if (!texto || !texto[0])
        return;

    int altura = 112;
    int y = ALTURA_MUNDO - altura - 32;

    DrawRectangle(0, y, LARGURA_MUNDO, altura, (Color){5, 5, 9, 225});
    DrawRectangle(0, y, LARGURA_MUNDO, 3, (Color){170, 28, 39, 240});

    int tamanho = 25;
    int quantidadeLinhas = 1;

    for (const char *p = texto; *p; p++)
        if (*p == '\n')
            quantidadeLinhas++;

    int alturaTexto = quantidadeLinhas * (tamanho + 12);

    desenharTextoCentralizado(
        texto,
        y + (altura - alturaTexto) / 2,
        tamanho,
        RAYWHITE
    );
}

static void desenharInterferencia(void) {
    DrawRectangle(
        0,
        0,
        LARGURA_MUNDO,
        ALTURA_MUNDO,
        (Color){38, 0, 10, 235}
    );

    for (int i = 0; i < 40; i++) {
        int y = GetRandomValue(0, ALTURA_MUNDO - 3);
        int altura = GetRandomValue(1, 5);
        int x = GetRandomValue(0, LARGURA_MUNDO / 3);
        int largura = GetRandomValue(LARGURA_MUNDO / 3, LARGURA_MUNDO);

        Color cor = (i % 3 == 0)
            ? (Color){255, 20, 35, 75}
            : (Color){220, 220, 240, 35};

        DrawRectangle(x, y, largura, altura, cor);
    }

    DrawRectangle(
        0,
        GetRandomValue(0, ALTURA_MUNDO - 18),
        LARGURA_MUNDO,
        GetRandomValue(5, 18),
        (Color){180, 0, 20, 60}
    );
}

void desenharCutscene(const SistemaCutscenes *sistema) {
    if (!sistema || !sistema->ativa)
        return;

    int quantidade = 0;
    const Etapa *etapas = obterEtapas(sistema->tipo, &quantidade);

    if (!etapas || sistema->indiceEtapa < 0 || sistema->indiceEtapa >= quantidade)
        return;

    if (sistema->tipo != CUTSCENE_DESPERTAR)
        DrawRectangle(0, 0, LARGURA_MUNDO, ALTURA_MUNDO, BLACK);

    const Etapa *etapa = &etapas[sistema->indiceEtapa];

    if (etapa->tipo == ETAPA_DESPERTAR_ESCURO) {
        DrawRectangle(
            0,
            0,
            LARGURA_MUNDO,
            ALTURA_MUNDO,
            (Color){0, 0, 0, 238}
        );

        desenharTextoCentralizado(
            etapa->titulo,
            ALTURA_MUNDO / 2 - 18,
            30,
            (Color){230, 35, 45, 255}
        );

        return;
    }

    if (etapa->tipo == ETAPA_DESPERTAR_INTERFERENCIA) {
        desenharInterferencia();
        return;
    }

    if (etapa->tipo == ETAPA_DESPERTAR_PRESENCA) {
        DrawRectangle(
            0,
            0,
            LARGURA_MUNDO,
            ALTURA_MUNDO,
            (Color){0, 0, 0, 105}
        );

        DrawRectangle(
            0,
            ALTURA_MUNDO - 142,
            LARGURA_MUNDO,
            142,
            (Color){4, 4, 8, 220}
        );

        desenharTextoCentralizado(
            etapa->titulo,
            ALTURA_MUNDO - 122,
            24,
            (Color){235, 35, 45, 255}
        );

        desenharTextoCentralizado(
            etapa->texto,
            ALTURA_MUNDO - 78,
            29,
            RAYWHITE
        );

        return;
    }

    if (etapa->tipo == ETAPA_CARTELA) {
        desenharCartela(etapa);
        return;
    }

    if (!sistema->texturaAtual.id)
        return;

    float progresso = limitar(
        sistema->tempoEtapa / etapa->duracao,
        0.0f,
        1.0f
    );

    float zoom = etapa->zoomInicial +
                 (etapa->zoomFinal - etapa->zoomInicial) * progresso;

    float alpha = 1.0f;

    if (etapa->duracao > 0.65f) {
        float entrada = limitar(
            sistema->tempoEtapa / 0.28f,
            0.0f,
            1.0f
        );

        float saida = limitar(
            (etapa->duracao - sistema->tempoEtapa) / 0.28f,
            0.0f,
            1.0f
        );

        alpha = fminf(entrada, saida);
    }

    float largura = LARGURA_MUNDO * zoom;
    float altura = ALTURA_MUNDO * zoom;

    Rectangle destino = {
        (LARGURA_MUNDO - largura) / 2.0f,
        (ALTURA_MUNDO - altura) / 2.0f,
        largura,
        altura
    };

    DrawTexturePro(
        sistema->texturaAtual,
        (Rectangle){
            0,
            0,
            sistema->texturaAtual.width,
            sistema->texturaAtual.height
        },
        destino,
        (Vector2){0, 0},
        0.0f,
        ColorAlpha(WHITE, alpha)
    );

    if (etapa->legenda)
        desenharLegendaCutscene(etapa->legenda);
}

bool cutsceneEstaAtiva(const SistemaCutscenes *sistema) {
    return sistema && sistema->ativa;
}

TipoCutscene cutsceneTipoAtual(const SistemaCutscenes *sistema) {
    return sistema ? sistema->tipo : CUTSCENE_NENHUMA;
}

TipoCutscene consumirCutsceneConcluida(SistemaCutscenes *sistema) {
    if (!sistema)
        return CUTSCENE_NENHUMA;

    TipoCutscene concluida = sistema->concluida;
    sistema->concluida = CUTSCENE_NENHUMA;

    return concluida;
}

void liberarCutscenes(SistemaCutscenes *sistema) {
    if (!sistema)
        return;

    descarregarTexturaAtual(sistema);
    memset(sistema, 0, sizeof *sistema);
    sistema->imagemAtual = -1;
}
