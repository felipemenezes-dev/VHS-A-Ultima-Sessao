#ifndef VHS_INVENTARIO_H
#define VHS_INVENTARIO_H

#include <stdbool.h>

#define INVENTARIO_MAX_OBJETOS 4

typedef enum {
    OBJETO_VAZIO,
    OBJETO_GUARDADO,
    OBJETO_USADO
} EstadoObjeto;

typedef enum {
    INVENTARIO_OK,
    INVENTARIO_DUPLICADO,
    INVENTARIO_CHEIO,
    INVENTARIO_INVALIDO
} ResultadoInventario;

typedef struct {
    int id;
    char nome[40];
    char descricao[160];
    bool consumivel;
    EstadoObjeto estado;
} ObjetoInventario;

typedef struct {
    ObjetoInventario objetos[INVENTARIO_MAX_OBJETOS];
} Inventario;

bool inventarioPossui(const Inventario *inventario, int id);
ResultadoInventario inventarioAdicionarObjeto(
    Inventario *inventario,
    int id,
    const char *nome,
    const char *descricao,
    bool consumivel
);
const ObjetoInventario *inventarioBuscarObjeto(const Inventario *inventario, int id);
int inventarioContarObjetos(const Inventario *inventario);
bool inventarioConsumirObjeto(Inventario *inventario, int id);
bool inventarioRemoverObjeto(Inventario *inventario, int id);
void inventarioLimpar(Inventario *inventario);

#endif
