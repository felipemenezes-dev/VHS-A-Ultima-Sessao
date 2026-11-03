#ifndef VHS_INVENTARIO_H
#define VHS_INVENTARIO_H
#include "fitas.h"

#define INVENTARIO_MAX_FITAS 3
#define INVENTARIO_MAX_OBJETOS 4
#define INVENTARIO_SLOTS (INVENTARIO_MAX_FITAS + INVENTARIO_MAX_OBJETOS)

typedef enum { OBJETO_VAZIO, OBJETO_GUARDADO, OBJETO_USADO } EstadoObjeto;
typedef enum {
    INV_OK,
    INV_DUPLICADO,
    INV_CHEIO,
    INV_SEM_MEMORIA,
    INV_INVALIDO
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
ResultadoInventario inventarioAdicionarFita(Inventario *inventario, int id, const char *nome,
                                            int frequencia, int digito);
ResultadoInventario inventarioAdicionarObjeto(Inventario *inventario, int id, const char *nome,
                                              const char *descricao, bool consumivel);
const ObjetoInventario *inventarioBuscarObjeto(const Inventario *inventario, int id);
int inventarioContarObjetos(const Inventario *inventario);

bool inventarioConsumirObjeto(Inventario *inventario, int id);
bool inventarioRemoverObjeto(Inventario *inventario, int id);
void inventarioLimpar(Inventario *inventario);
#endif
