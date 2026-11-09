#ifndef VHS_ESTADO_JOGO_H
#define VHS_ESTADO_JOGO_H


#include <stdbool.h>
#include "inventario.h"


typedef struct {
    bool chaveEstoqueObtida;
    bool fusivelInstalado;
    int vida;
    int cargasFlash;
} EstadoJogo;


void reiniciarEstadoJogo(EstadoJogo *estado);
void registrarObjetoColetado(EstadoJogo *estado, int idObjeto);
bool estadoJogoPodeEntrarSala(const EstadoJogo *estado, int salaDestino);
bool estadoJogoInstalarFusivel(EstadoJogo *estado, Inventario *inventario);


#endif
