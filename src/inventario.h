
#ifndef VHS_INVENTARIO_H
#define VHS_INVENTARIO_H

#include "fitas.h"
#include <stdbool.h>

#define INVENTARIO_MAX_OBJETOS 4
#define INVENTARIO_MAX_FITAS 3

typedef enum {
    OBJETO_VAZIO,
    OBJETO_GUARDADO,
    OBJETO_USADO,
    OBJETO_INSTALADO
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
    Fita *fitas;
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

ResultadoInventario inventarioAdicionarFita(
    Inventario *inventario,
    int id,
    const char *nome,
    int frequencia,
    int digito
);

const ObjetoInventario *inventarioBuscarObjeto(
    const Inventario *inventario,
    int id
);

const Fita *inventarioBuscarFita(
    const Inventario *inventario,
    int id
);

int inventarioContarObjetos(const Inventario *inventario);
int inventarioContarFitas(const Inventario *inventario);

bool inventarioConsumirObjeto(Inventario *inventario, int id);
bool inventarioMarcarInstalado(Inventario *inventario, int id);
bool inventarioRemoverObjeto(Inventario *inventario, int id);

void inventarioLimpar(Inventario *inventario);

#endif
