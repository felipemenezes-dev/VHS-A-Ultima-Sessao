#include "estado_jogo.h"
#include "objetos.h"
#include "mundo.h"


void reiniciarEstadoJogo(EstadoJogo *estado) {
    if (!estado)
        return;


    *estado = (EstadoJogo){
        .vida = 100,
        .cargasFlash = 3
    };
}


void registrarObjetoColetado(EstadoJogo *estado, int idObjeto) {
    if (!estado)
        return;


    if (idObjeto == ID_CHAVE_ESTOQUE)
        estado->chaveEstoqueObtida = true;
}


bool estadoJogoPodeEntrarSala(const EstadoJogo *estado, int salaDestino) {
    if (!estado)
        return false;


    if (salaDestino == ESTOQUE && !estado->chaveEstoqueObtida)
        return false;


    if (salaDestino == SALA_PROJECAO && !estado->fusivelInstalado)
        return false;


    return true;
}


bool estadoJogoInstalarFusivel(EstadoJogo *estado, Inventario *inventario) {
    if (!estado || !inventario || estado->fusivelInstalado)
        return false;


    if (!inventarioMarcarInstalado(inventario, ID_FUSIVEL))
        return false;


    estado->fusivelInstalado = true;
    return true;
}
