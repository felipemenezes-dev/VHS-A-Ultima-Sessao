#ifndef VHS_DERROTA_H
#define VHS_DERROTA_H

#include "raylib.h"
#include <stdbool.h>

typedef enum {
    ACAO_DERROTA_NENHUMA = -1,
    ACAO_DERROTA_TENTAR_NOVAMENTE,
    ACAO_DERROTA_MENU_PRINCIPAL,
    ACAO_DERROTA_SAIR
} AcaoDerrota;

bool carregarDerrota(void);
void descarregarDerrota(void);
void reiniciarMenuDerrota(void);
AcaoDerrota atualizarMenuDerrota(void);
void desenharDerrota(void);

#endif
