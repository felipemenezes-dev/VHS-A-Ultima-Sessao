#ifndef VHS_INTERFACE_INVENTARIO_H
#define VHS_INTERFACE_INVENTARIO_H

#include "raylib.h"
#include "inventario.h"

typedef struct {
    Inventario dados;
    int selecionado;
} TelaInventario;

bool iniciarTelaInventario(TelaInventario *inventario);
void liberarTelaInventario(TelaInventario *inventario);

void selecionarItemInventario(
    TelaInventario *inventario,
    int passo
);

const ObjetoInventario *inventarioObjetoSelecionado(
    const TelaInventario *inventario
);

void desenharInventario(
    const TelaInventario *inventario,
    int largura,
    int altura
);

#endif
