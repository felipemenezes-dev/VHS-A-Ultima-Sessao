#ifndef VHS_OBJETOS_H
#define VHS_OBJETOS_H


#include "inventario.h"
#include "mundo.h"
#include <stdbool.h>


enum IdentificadorObjeto {
    ID_KIT_MEDICO = 1,
    ID_CHAVE_ESTOQUE = 2,
    ID_FUSIVEL = 3,
    ID_BATERIA_FLASH = 4,
    ID_QUADRO_ELETRICO = 5,
    ID_BILHETE_GERENTE = 6,
    ID_APARELHO_VHS = 7,
    ID_TECLADO_SAIDA = 8,
    ID_FITA_17 = 17,
    ID_FITA_42 = 42,
    ID_FITA_89 = 89
};


bool bilheteGerenteProximo(int sala, Vector2 personagem);
bool quadroEletricoProximo(int sala, Vector2 personagem);
bool aparelhoVhsProximo(int sala, Vector2 personagem);
bool tecladoSaidaProximo(int sala, Vector2 personagem);
bool portaSaidaProxima(int sala, Vector2 personagem);




bool carregarObjetosMapa(void);
void reiniciarObjetosMapa(void);
void marcarObjetoConcluido(int id);
void liberarObjetosMapa(void);
void desenharObjetosMapa(int sala);
void desenharIndicacaoColeta(int sala, Vector2 personagem);
bool coletarObjetoProximo(int sala, Vector2 personagem, Inventario *inventario, int *idColetado);


#endif
