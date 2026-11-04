#include "fitas.h"
#include <stdio.h>
#include <stdlib.h>

const Fita *buscarFita(const Fita *lista, int id) {
    for (const Fita *fita = lista; fita; fita = fita->proxima)
        if (fita->id == id)
            return fita;
    return NULL;
}

int contarFitas(const Fita *lista) {
    int total = 0;
    for (const Fita *fita = lista; fita; fita = fita->proxima)
        total++;
    return total;
}

const Fita *fitaNaPosicao(const Fita *lista, int indice) {
    if (indice < 0)
        return NULL;
    while (lista && indice-- > 0)
        lista = lista->proxima;
    return lista;
}

bool inserirFita(Fita **lista, int id, const char *nome, int frequencia, int digito) {
    if (!lista || !nome || buscarFita(*lista, id))
        return false;
    Fita *nova = malloc(sizeof *nova);
    if (!nova)
        return false;
    *nova = (Fita){.id = id, .frequencia = frequencia, .digito = digito};
    snprintf(nova->nome, sizeof nova->nome, "%s", nome);

    if (*lista == NULL) {
        *lista = nova;
    } else {
        Fita *ultima = *lista;
        while (ultima->proxima != NULL)
            ultima = ultima->proxima;
        ultima->proxima = nova;
    }
    return true;
}

bool removerFita(Fita **lista, int id) {
    if (!lista)
        return false;
    Fita *atual = *lista;
    Fita *anterior = NULL;
    while (atual != NULL && atual->id != id) {
        anterior = atual;
        atual = atual->proxima;
    }
    if (atual == NULL)
        return false;
    if (anterior == NULL)
        *lista = atual->proxima;
    else
        anterior->proxima = atual->proxima;
    free(atual);
    return true;
}

void ordenarFitasPorInsercao(Fita **lista) {
    if (!lista)
        return;
    Fita *ordenada = NULL;
    while (*lista) {
        Fita *atual = *lista;
        *lista = atual->proxima;
        Fita **posicao = &ordenada;
        while (*posicao && (*posicao)->frequencia <= atual->frequencia)
            posicao = &(*posicao)->proxima;
        atual->proxima = *posicao;
        *posicao = atual;
    }
    *lista = ordenada;
}

void liberarLista(Fita **lista) {
    if (!lista)
        return;
    while (*lista) {
        Fita *atual = *lista;
        *lista = atual->proxima;
        free(atual);
    }
}
