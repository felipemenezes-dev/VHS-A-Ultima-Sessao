#ifndef VHS_INTERFACE_INVENTARIO_H
#define VHS_INTERFACE_INVENTARIO_H
#include "raylib.h"
#include "inventario.h"

typedef struct {
    Texture2D fundo;
    bool aberto;
    Inventario dados;
    int selecionado;
} TelaInventario;

bool iniciarTelaInventario(TelaInventario *inventario);
void liberarTelaInventario(TelaInventario *inventario);
bool atualizarTelaInventario(TelaInventario *inventario, bool tab, bool esc);
void selecionarEspacoInventario(TelaInventario *inventario, int passoX, int passoY, bool clique,
                                Vector2 cursor, int largura, int altura);
void desenharInventario(const TelaInventario *inventario, int largura, int altura);
#endif
