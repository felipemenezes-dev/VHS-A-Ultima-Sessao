#ifndef VHS_INTERFACE_VHS_H
#define VHS_INTERFACE_VHS_H

#include "inventario.h"
#include <stdbool.h>

typedef struct {
    bool aberto;
    bool ordenado;
    bool transmissaoReconstruida;
    bool sequenciaIncorreta;
    int etapaInsercao;
    int fitaSelecionada;
    int fitasInseridas[INVENTARIO_MAX_FITAS];
    float tempoMensagem;
    char mensagem[160];
    char codigo[4];
} InterfaceVHS;

void iniciarInterfaceVHS(InterfaceVHS *interface);
void abrirInterfaceVHS(InterfaceVHS *interface);
void atualizarInterfaceVHS(InterfaceVHS *interface, Inventario *inventario, float delta);
void desenharInterfaceVHS(const InterfaceVHS *interface, const Inventario *inventario);
bool interfaceVHSEstaAberta(const InterfaceVHS *interface);
bool interfaceVHSReconstruida(const InterfaceVHS *interface);
const char *interfaceVHSCodigo(const InterfaceVHS *interface);

#endif
