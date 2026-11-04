#ifndef VHS_INTERFACE_INVENTARIO_H
#define VHS_INTERFACE_INVENTARIO_H

#include "raylib.h"
#include "inventario.h"

typedef struct {
    bool aberto;
    Inventario dados;
    int selecionado;
} TelaInventario;

bool iniciarTelaInventario(TelaInventario *inventario);
void liberarTelaInventario(TelaInventario *inventario);
bool atualizarTelaInventario(TelaInventario *inventario, bool teclaTab, bool teclaEsc);
void selecionarEspacoInventario(
    TelaInventario *inventario,
    int passoHorizontal,
    int passoVertical,
    bool clique,
    Vector2 cursor,
    int largura,
    int altura
);
void desenharInventario(const TelaInventario *inventario, int largura, int altura);

#endif
