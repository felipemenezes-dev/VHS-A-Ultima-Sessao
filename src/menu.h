#ifndef VHS_MENU_H
#define VHS_MENU_H

#include <stdbool.h>

typedef enum {
    ESTADO_MENU_PRINCIPAL = 0,
    ESTADO_MENU_COMO_JOGAR,
    ESTADO_MENU_CREDITOS,
    ESTADO_MENU_INICIAR_JOGO,
    ESTADO_MENU_SAIR
} EstadoSubMenu;

typedef enum {
    ACAO_PAUSA_NENHUMA = 0,
    ACAO_PAUSA_CONTINUAR,
    ACAO_PAUSA_MENU_PRINCIPAL,
    ACAO_PAUSA_SAIR
} AcaoPausa;

bool carregarMenu(void);
void descarregarMenu(void);
EstadoSubMenu atualizarMenu(void);
void desenharMenu(void);

void abrirMenuPausa(void);
bool menuPausaEstaAberto(void);
AcaoPausa atualizarMenuPausa(void);
void desenharMenuPausa(void);

#endif
