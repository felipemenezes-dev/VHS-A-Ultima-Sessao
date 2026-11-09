#ifndef VHS_INTERFACE_TECLADO_H
#define VHS_INTERFACE_TECLADO_H


#include <stdbool.h>


typedef struct {
    bool aberto;
    bool saidaLiberada;
    bool transmissaoCompleta;
    char codigoEsperado[4];
    char entrada[4];
    char mensagem[128];
    float tempoMensagem;
} TecladoSaida;


void iniciarTecladoSaida(TecladoSaida *teclado);
void abrirTecladoSaida(TecladoSaida *teclado, const char *codigo);
void atualizarTecladoSaida(TecladoSaida *teclado, float delta);
void desenharTecladoSaida(const TecladoSaida *teclado);
bool tecladoSaidaEstaAberto(const TecladoSaida *teclado);
bool tecladoSaidaEstaLiberado(const TecladoSaida *teclado);


#endif
