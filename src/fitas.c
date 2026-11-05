#include "fitas.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const Fita *buscarFita(const Fita *lista, int id) {
    for (const Fita *atual = lista; atual; atual = atual->proxima) {
        if (atual->id == id)
            return atual;
    }

    return NULL;
}

const Fita *fitaNaPosicao(const Fita *lista, int indice) {
    if (indice < 0)
        return NULL;

    while (lista && indice > 0) {
        lista = lista->proxima;
        indice--;
    }

    return lista;
}

int contarFitas(const Fita *lista) {
    int total = 0;

    for (const Fita *atual = lista; atual; atual = atual->proxima)
        total++;

    return total;
}

bool inserirFita(Fita **lista, int id, const char *nome, int frequencia, int digito) {
    if (!lista || id <= 0 || !nome || !*nome ||
        strlen(nome) >= sizeof(((Fita *)0)->nome) ||
        frequencia <= 0 || digito < 0 || digito > 9 ||
        buscarFita(*lista, id)) {
        return false;
    }

    Fita *nova = malloc(sizeof *nova);

    if (!nova)
        return false;

    *nova = (Fita){0};
    nova->id = id;
    nova->frequencia = frequencia;
    nova->digito = digito;
    snprintf(nova->nome, sizeof nova->nome, "%s", nome);

    if (!*lista) {
        *lista = nova;
        return true;
    }

    Fita *ultima = *lista;

    while (ultima->proxima)
        ultima = ultima->proxima;

    ultima->proxima = nova;
    return true;
}

bool removerFita(Fita **lista, int id) {
    if (!lista)
        return false;

    Fita *atual = *lista;
    Fita *anterior = NULL;

    while (atual && atual->id != id) {
        anterior = atual;
        atual = atual->proxima;
    }

    if (!atual)
        return false;

    if (!anterior)
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
