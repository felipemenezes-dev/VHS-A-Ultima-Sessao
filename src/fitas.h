#ifndef VHS_FITAS_H
#define VHS_FITAS_H
#include <stdbool.h>

typedef struct Fita {
    int id;
    char nome[40];
    int frequencia;
    int digito;
    struct Fita *proxima;
} Fita;

bool inserirFita(Fita **lista, int id, const char *nome, int frequencia, int digito);
const Fita *buscarFita(const Fita *lista, int id);
const Fita *fitaNaPosicao(const Fita *lista, int indice);
int contarFitas(const Fita *lista);
bool removerFita(Fita **lista, int id);
void ordenarFitasPorInsercao(Fita **lista);
void liberarLista(Fita **lista);
#endif
