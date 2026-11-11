#ifndef VHS_CUTSCENES_H
#define VHS_CUTSCENES_H

#include "raylib.h"
#include <stdbool.h>

typedef enum {
    CUTSCENE_NENHUMA = 0,
    CUTSCENE_INTRODUCAO,
    CUTSCENE_MISSAO,
    CUTSCENE_DESPERTAR,
    CUTSCENE_VITORIA,
    CUTSCENE_DERROTA
} TipoCutscene;

#define CUTSCENE_TOTAL_IMAGENS 40
#define CUTSCENE_TAMANHO_CAMINHO 512

typedef struct {
    char caminhos[CUTSCENE_TOTAL_IMAGENS][CUTSCENE_TAMANHO_CAMINHO];
    Texture2D texturaAtual;
    int imagemAtual;
    int indiceEtapa;
    float tempoEtapa;
    bool ativa;
    TipoCutscene tipo;
    TipoCutscene concluida;
} SistemaCutscenes;

bool carregarCutscenes(SistemaCutscenes *sistema);
bool iniciarCutscene(SistemaCutscenes *sistema, TipoCutscene tipo);
void atualizarCutscene(SistemaCutscenes *sistema, float delta);
void desenharCutscene(const SistemaCutscenes *sistema);
bool cutsceneEstaAtiva(const SistemaCutscenes *sistema);
TipoCutscene cutsceneTipoAtual(const SistemaCutscenes *sistema);
TipoCutscene consumirCutsceneConcluida(SistemaCutscenes *sistema);
void liberarCutscenes(SistemaCutscenes *sistema);

#endif
