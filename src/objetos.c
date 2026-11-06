#include "objetos.h"
#include <math.h>
#include <string.h>

#define ALCANCE_COLETA 82.0f
#define TAMANHO_OBJETO 76.0f
#define TAMANHO_BILHETE 76.0f

typedef struct {
    int id;
    int sala;
    const char *nome;
    const char *descricao;
    const char *caminho;
    Vector2 posicao;
    float tamanho;
    bool coletavel;
    bool coletado;
    Texture2D textura;
} ObjetoMapa;

static ObjetoMapa objetos[] = {
    {
        ID_KIT_MEDICO,
        SALAO,
        "Kit medico",
        "Recupera 30 pontos de vida quando usado com Q.",
        "assets/itens/kit_medico.png",
        {470, 640},
        TAMANHO_OBJETO,
        true,
        false,
        {0}
    },
    {
        ID_FITA_17,
        BANHEIRO,
        "A Chamada",
        "Fita VHS com frequencia 17. Primeiro fragmento da transmissao.",
        "assets/itens/fita_17.png",
        {1318, 433},
        TAMANHO_OBJETO,
        true,
        false,
        {0}
    },
    {
        ID_CHAVE_ESTOQUE,
        ESCRITORIO,
        "Chave do estoque",
        "Chave permanente que libera a porta do estoque.",
        "assets/itens/chave_estoque.png",
        {1008, 445},
        TAMANHO_OBJETO,
        true,
        false,
        {0}
    },
    {
        ID_FITA_42,
        ESTOQUE,
        "O Corredor",
        "Fita VHS com frequencia 42. Segundo fragmento da transmissao.",
        "assets/itens/fita_42.png",
        {1306, 586},
        TAMANHO_OBJETO,
        true,
        false,
        {0}
    },
    {
        ID_FUSIVEL,
        ESTOQUE,
        "Fusivel",
        "Componente necessario para restaurar a energia da sala de projecao.",
        "assets/itens/fusivel.png",
        {1333, 743},
        TAMANHO_OBJETO,
        true,
        false,
        {0}
    },
    {
        ID_FITA_89,
        SALA_PROJECAO,
        "Sala Vazia",
        "Fita VHS com frequencia 89. Terceiro fragmento da transmissao.",
        "assets/itens/fita_89.png",
        {1172, 326},
        TAMANHO_OBJETO,
        true,
        false,
        {0}
    },
    {
        ID_BATERIA_FLASH,
        SALA_PROJECAO,
        "Bateria do flash",
        "Adiciona duas cargas ao flash.",
        "assets/itens/bateria_flash.png",
        {458, 594},
        TAMANHO_OBJETO,
        true,
        false,
        {0}
    },
    {
        0,
        ESCRITORIO,
        "Bilhete do gerente",
        "Bilhete que sera examinado em uma etapa posterior.",
        "assets/itens/bilhete_gerente.png",
        {708, 445},
        TAMANHO_BILHETE,
        false,
        false,
        {0}
    }
};

static int quantidadeObjetos(void) {
    return (int)(sizeof objetos / sizeof objetos[0]);
}

static ObjetoMapa *encontrarObjetoProximo(int sala, Vector2 personagem) {
    ObjetoMapa *maisProximo = NULL;
    float menorDistancia = ALCANCE_COLETA;

    for (int i = 0; i < quantidadeObjetos(); i++) {
        ObjetoMapa *objeto = &objetos[i];

        if (!objeto->coletavel || objeto->coletado || objeto->sala != sala) {
            continue;
        }

        float distancia = hypotf(
            personagem.x - objeto->posicao.x,
            personagem.y - objeto->posicao.y
        );

        if (distancia <= menorDistancia) {
            menorDistancia = distancia;
            maisProximo = objeto;
        }
    }

    return maisProximo;
}

bool carregarObjetosMapa(void) {
    bool sucesso = true;

    for (int i = 0; i < quantidadeObjetos(); i++) {
        objetos[i].textura = LoadTexture(objetos[i].caminho);

        if (!objetos[i].textura.id) {
            TraceLog(
                LOG_ERROR,
                "Nao foi possivel carregar o objeto: %s",
                objetos[i].caminho
            );

            sucesso = false;
        } else {
            SetTextureFilter(objetos[i].textura, TEXTURE_FILTER_POINT);
        }
    }

    if (!sucesso) {
        liberarObjetosMapa();
    }

    reiniciarObjetosMapa();

    return sucesso;
}

void reiniciarObjetosMapa(void) {
    for (int i = 0; i < quantidadeObjetos(); i++) {
        objetos[i].coletado = false;
    }
}

void liberarObjetosMapa(void) {
    for (int i = 0; i < quantidadeObjetos(); i++) {
        if (objetos[i].textura.id) {
            UnloadTexture(objetos[i].textura);
        }

        objetos[i].textura = (Texture2D){0};
    }
}

void desenharObjetosMapa(int sala) {
    for (int i = 0; i < quantidadeObjetos(); i++) {
        const ObjetoMapa *objeto = &objetos[i];

        if (objeto->sala != sala ||
            (objeto->coletavel && objeto->coletado) ||
            !objeto->textura.id) {
            continue;
        }

        Rectangle origem = {
            0,
            0,
            (float)objeto->textura.width,
            (float)objeto->textura.height
        };

        Rectangle destino = {
            objeto->posicao.x - objeto->tamanho / 2.0f,
            objeto->posicao.y - objeto->tamanho / 2.0f,
            objeto->tamanho,
            objeto->tamanho
        };

        DrawTexturePro(
            objeto->textura,
            origem,
            destino,
            (Vector2){0, 0},
            0,
            WHITE
        );
    }
}

void desenharIndicacaoColeta(int sala, Vector2 personagem) {
    ObjetoMapa *objeto = encontrarObjetoProximo(sala, personagem);

    if (!objeto) {
        return;
    }

    const char *texto = "E - COLETAR";

    int larguraTexto = MeasureText(texto, 18);
    float larguraCaixa = (float)larguraTexto + 24.0f;

    float x = objeto->posicao.x - larguraCaixa / 2.0f;
    float y = objeto->posicao.y - objeto->tamanho / 2.0f - 34.0f;

    if (x < 4) {
        x = 4;
    }

    if (x + larguraCaixa > LARGURA_MUNDO - 4) {
        x = LARGURA_MUNDO - larguraCaixa - 4;
    }

    DrawRectangleRec(
        (Rectangle){x, y, larguraCaixa, 28},
        (Color){8, 8, 12, 225}
    );

    DrawRectangleLinesEx(
        (Rectangle){x, y, larguraCaixa, 28},
        2,
        (Color){220, 190, 125, 255}
    );

    DrawText(
        texto,
        (int)x + 12,
        (int)y + 5,
        18,
        RAYWHITE
    );
}

bool coletarObjetoProximo(
    int sala,
    Vector2 personagem,
    Inventario *inventario,
    int *idColetado
) {
    if (!inventario) {
        return false;
    }

    ObjetoMapa *objeto = encontrarObjetoProximo(sala, personagem);

    if (!objeto) {
        return false;
    }

    ResultadoInventario resultado;

    if (objeto->id == ID_FITA_17) {
        resultado = inventarioAdicionarFita(
            inventario,
            ID_FITA_17,
            "A Chamada",
            17,
            4
        );
    } else if (objeto->id == ID_FITA_42) {
        resultado = inventarioAdicionarFita(
            inventario,
            ID_FITA_42,
            "O Corredor",
            42,
            1
        );
    } else if (objeto->id == ID_FITA_89) {
        resultado = inventarioAdicionarFita(
            inventario,
            ID_FITA_89,
            "Sala Vazia",
            89,
            7
        );
    } else {
        resultado = inventarioAdicionarObjeto(
            inventario,
            objeto->id,
            objeto->nome,
            objeto->descricao,
            objeto->id == ID_KIT_MEDICO ||
            objeto->id == ID_BATERIA_FLASH
        );
    }

    if (resultado != INVENTARIO_OK) {
        TraceLog(
            LOG_WARNING,
            "Nao foi possivel guardar o objeto: %s",
            objeto->nome
        );

        return false;
    }

    objeto->coletado = true;

    if (idColetado) {
        *idColetado = objeto->id;
    }

    TraceLog(LOG_INFO, "Objeto coletado: %s", objeto->nome);

    return true;
}