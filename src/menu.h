#ifndef MENU_H
#define MENU_H

#include "raylib.h"
#include <stdbool.h>

typedef enum {
    ESTADO_MENU_PRINCIPAL,
    ESTADO_MENU_COMO_JOGAR,
    ESTADO_MENU_CREDITOS,
    ESTADO_MENU_SAIR,
    ESTADO_MENU_INICIAR_JOGO
} EstadoSubMenu;

bool carregarMenu(void);
void descarregarMenu(void);
EstadoSubMenu atualizarMenu(void);
void desenharMenu(void);

#endif 