#include "inventario.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

const ObjetoInventario *inventarioBuscarObjeto(const Inventario *inventario, int id) {
    if (!inventario || id <= 0)
        return NULL;
    for (int i = 0; i < INVENTARIO_MAX_OBJETOS; i++)
        if (inventario->objetos[i].estado != OBJETO_VAZIO && inventario->objetos[i].id == id)
            return &inventario->objetos[i];
    return NULL;
}

bool inventarioPossui(const Inventario *inventario, int id) {
    return inventario && id > 0 &&
           (buscarFita(inventario->fitas, id) || inventarioBuscarObjeto(inventario, id));
}

ResultadoInventario inventarioAdicionarFita(Inventario *inventario, int id, const char *nome,
                                            int frequencia, int digito) {
    if (!inventario || id <= 0 || !nome || !*nome || strlen(nome) >= sizeof((Fita *)0)->nome ||
        frequencia <= 0 || digito < 0 || digito > 9)
        return INV_INVALIDO;
    if (inventarioPossui(inventario, id))
        return INV_DUPLICADO;
    if (contarFitas(inventario->fitas) >= INVENTARIO_MAX_FITAS)
        return INV_CHEIO;
    return inserirFita(&inventario->fitas, id, nome, frequencia, digito) ? INV_OK : INV_SEM_MEMORIA;
}

ResultadoInventario inventarioAdicionarObjeto(Inventario *inventario, int id, const char *nome,
                                              const char *descricao, bool consumivel) {
    if (!inventario || id <= 0 || !nome || !*nome || !descricao ||
        strlen(nome) >= sizeof inventario->objetos[0].nome ||
        strlen(descricao) >= sizeof inventario->objetos[0].descricao)
        return INV_INVALIDO;
    if (inventarioPossui(inventario, id))
        return INV_DUPLICADO;
    for (int i = 0; i < INVENTARIO_MAX_OBJETOS; i++) {
        if (inventario->objetos[i].estado != OBJETO_VAZIO)
            continue;
        ObjetoInventario *objeto = &inventario->objetos[i];
        *objeto = (ObjetoInventario){.id = id, .consumivel = consumivel, .estado = OBJETO_GUARDADO};
        snprintf(objeto->nome, sizeof objeto->nome, "%s", nome);
        snprintf(objeto->descricao, sizeof objeto->descricao, "%s", descricao);
        return INV_OK;
    }
    return INV_CHEIO;
}

int inventarioContarObjetos(const Inventario *inventario) {
    int total = 0;
    if (inventario)
        for (int i = 0; i < INVENTARIO_MAX_OBJETOS; i++)
            total += inventario->objetos[i].estado == OBJETO_GUARDADO;
    return total;
}

bool inventarioConsumirObjeto(Inventario *inventario, int id) {
    if (!inventario)
        return false;
    for (int i = 0; i < INVENTARIO_MAX_OBJETOS; i++) {
        ObjetoInventario *objeto = &inventario->objetos[i];
        if (objeto->id != id || objeto->estado != OBJETO_GUARDADO || !objeto->consumivel)
            continue;
        objeto->estado = OBJETO_USADO;
        return true;
    }
    return false;
}

bool inventarioRemoverObjeto(Inventario *inventario, int id) {
    if (!inventario || id <= 0)
        return false;
    for (int i = 0; i < INVENTARIO_MAX_OBJETOS; i++)
        if (inventario->objetos[i].estado != OBJETO_VAZIO && inventario->objetos[i].id == id) {
            inventario->objetos[i] = (ObjetoInventario){0};
            return true;
        }
    return false;
}

void inventarioLimpar(Inventario *inventario) {
    if (!inventario)
        return;
    liberarLista(&inventario->fitas);
    *inventario = (Inventario){0};
}
