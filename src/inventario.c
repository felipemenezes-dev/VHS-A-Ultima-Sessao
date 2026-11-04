#include "inventario.h"
#include <stdio.h>
#include <string.h>

const ObjetoInventario *inventarioBuscarObjeto(const Inventario *inventario, int id) {
    if (!inventario || id <= 0)
        return NULL;

    for (int i = 0; i < INVENTARIO_MAX_OBJETOS; i++) {
        const ObjetoInventario *objeto = &inventario->objetos[i];

        if (objeto->estado != OBJETO_VAZIO && objeto->id == id)
            return objeto;
    }

    return NULL;
}

bool inventarioPossui(const Inventario *inventario, int id) {
    return inventarioBuscarObjeto(inventario, id) != NULL;
}

ResultadoInventario inventarioAdicionarObjeto(
    Inventario *inventario,
    int id,
    const char *nome,
    const char *descricao,
    bool consumivel
) {
    if (
        !inventario ||
        id <= 0 ||
        !nome ||
        !*nome ||
        !descricao ||
        strlen(nome) >= sizeof inventario->objetos[0].nome ||
        strlen(descricao) >= sizeof inventario->objetos[0].descricao
    )
        return INVENTARIO_INVALIDO;

    if (inventarioPossui(inventario, id))
        return INVENTARIO_DUPLICADO;

    for (int i = 0; i < INVENTARIO_MAX_OBJETOS; i++) {
        ObjetoInventario *objeto = &inventario->objetos[i];

        if (objeto->estado != OBJETO_VAZIO)
            continue;

        *objeto = (ObjetoInventario){
            .id = id,
            .consumivel = consumivel,
            .estado = OBJETO_GUARDADO
        };

        snprintf(objeto->nome, sizeof objeto->nome, "%s", nome);
        snprintf(objeto->descricao, sizeof objeto->descricao, "%s", descricao);

        return INVENTARIO_OK;
    }

    return INVENTARIO_CHEIO;
}

int inventarioContarObjetos(const Inventario *inventario) {
    if (!inventario)
        return 0;

    int total = 0;

    for (int i = 0; i < INVENTARIO_MAX_OBJETOS; i++)
        total += inventario->objetos[i].estado == OBJETO_GUARDADO;

    return total;
}

bool inventarioConsumirObjeto(Inventario *inventario, int id) {
    if (!inventario)
        return false;

    for (int i = 0; i < INVENTARIO_MAX_OBJETOS; i++) {
        ObjetoInventario *objeto = &inventario->objetos[i];

        if (
            objeto->id == id &&
            objeto->estado == OBJETO_GUARDADO &&
            objeto->consumivel
        ) {
            objeto->estado = OBJETO_USADO;
            return true;
        }
    }

    return false;
}

bool inventarioRemoverObjeto(Inventario *inventario, int id) {
    if (!inventario || id <= 0)
        return false;

    for (int i = 0; i < INVENTARIO_MAX_OBJETOS; i++) {
        ObjetoInventario *objeto = &inventario->objetos[i];

        if (objeto->estado != OBJETO_VAZIO && objeto->id == id) {
            *objeto = (ObjetoInventario){0};
            return true;
        }
    }

    return false;
}

void inventarioLimpar(Inventario *inventario) {
    if (inventario)
        *inventario = (Inventario){0};
}
