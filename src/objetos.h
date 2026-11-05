#ifndef VHS_OBJETOS_H
#define VHS_OBJETOS_H

#include "inventario.h"
#include "mundo.h"
#include <stdbool.h>

enum IdentificadorObjeto {
    ID_KIT_MEDICO = 1,
    ID_CHAVE_ESTOQUE = 2,
    ID_FUSIVEL = 3,
    ID_BATERIA_FLASH = 4,
    ID_FITA_17 = 17,
    ID_FITA_42 = 42,
    ID_FITA_89 = 89
};

bool carregarObjetosMapa(void);
void reiniciarObjetosMapa(void);
void liberarObjetosMapa(void);
void desenharObjetosMapa(int sala);
void desenharIndicacaoColeta(int sala, Vector2 personagem);
bool coletarObjetoProximo(int sala, Vector2 personagem, Inventario *inventario, int *idColetado);

#endif
