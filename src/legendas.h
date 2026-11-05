
#ifndef VHS_LEGENDAS_H
#define VHS_LEGENDAS_H

#include "raylib.h"
#include <stdbool.h>

void iniciarLegendas(void);
void mostrarLegenda(const char *texto, float duracao);
void atualizarLegendas(float delta);
void desenharLegendas(void);
void liberarLegendas(void);

#endif
