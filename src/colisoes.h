    #ifndef VHS_COLISOES_H
    #define VHS_COLISOES_H
    #include "mundo.h"
    #include <stdint.h>
    typedef struct {
        uint32_t *somas;
        int largura, altura, sala;
        bool pronto;
    } MapaColisao;
    bool carregarMapaColisao(MapaColisao *mapa, const char *caminho);
    bool carregarColisoesSala(MapaColisao *mapa, int sala);
    void liberarColisoes(MapaColisao *mapa);
    bool retanguloBloqueado(const MapaColisao *mapa, Rectangle r);
    bool pesBloqueados(const MapaColisao *mapa, Vector2 p);
    Vector2 moverComColisao(const MapaColisao *mapa, Vector2 p, Vector2 deslocamento);
    #endif
