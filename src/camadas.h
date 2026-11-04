#ifndef VHS_CAMADAS_H
#define VHS_CAMADAS_H
#include "mundo.h"


bool iniciarCamadas(void);
void liberarCamadas(void);
void desenharJohnComCamadas(Texture2D fundo, Texture2D john, int sala, Vector2 posicaoPes);
#endif
