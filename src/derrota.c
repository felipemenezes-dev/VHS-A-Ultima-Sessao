#include "derrota.h"
#include "mundo.h"

static Texture2D fundo;
static int opcaoDerrotaSelecionada = 0;

static const Rectangle botoesDerrota[] = {
    {480, 505, 710, 68}, {480, 590, 710, 68}, {480, 675, 710, 68}
};

static Vector2 mouseNoMundo(void){
    return telaParaMundo(GetMousePosition(), areaExibicao(GetScreenWidth(), GetScreenHeight()));
}

static AcaoDerrota acaoDaOpcaoDerrota(int opcao){
    switch(opcao){
        case 0:
            return ACAO_DERROTA_TENTAR_NOVAMENTE;
        case 1:
            return ACAO_DERROTA_MENU_PRINCIPAL;
        case 2:
            return ACAO_DERROTA_SAIR;
        default:
            return ACAO_DERROTA_NENHUMA;
    }
}

bool carregarDerrota(void){
    if(fundo.id)
        UnloadTexture(fundo);
    fundo = LoadTexture("assets/ui/game_over.png");
    opcaoDerrotaSelecionada = 0;
    return fundo.id != 0;
}

void descarregarDerrota(void){
    if(fundo.id)
        UnloadTexture(fundo);
    fundo = (Texture2D){0};
}

void reiniciarMenuDerrota(void){
    opcaoDerrotaSelecionada = 0;
}

AcaoDerrota atualizarMenuDerrota(void){
    if(IsKeyPressed(KEY_ESCAPE))
        return ACAO_DERROTA_MENU_PRINCIPAL;
    if(IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)){
        opcaoDerrotaSelecionada = (opcaoDerrotaSelecionada - 1 + 3) % 3;
    }
    if(IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)){
        opcaoDerrotaSelecionada = (opcaoDerrotaSelecionada + 1) % 3;
    }
    Vector2 mouse = mouseNoMundo();
    for(int i = 0; i < 3; i++){
        if(CheckCollisionPointRec(mouse, botoesDerrota[i])){
            opcaoDerrotaSelecionada = i;
            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                return acaoDaOpcaoDerrota(i);
        }
    }
    if(IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER) || IsKeyPressed(KEY_SPACE)){
        return acaoDaOpcaoDerrota(opcaoDerrotaSelecionada);
    }
    return ACAO_DERROTA_NENHUMA;
}

void desenharDerrota(void){
    if(fundo.id){
        DrawTexturePro(fundo, (Rectangle){0, 0, (float)fundo.width, (float)fundo.height}, (Rectangle){0, 0, LARGURA_MUNDO, ALTURA_MUNDO}, (Vector2){0, 0}, 0, WHITE);
    } 
    else{
        ClearBackground(BLACK);
    }
    DrawRectangle(0, 0, LARGURA_MUNDO, ALTURA_MUNDO, (Color){0, 0, 0, 185});
    DrawRectangle(330, 170, 1012, 650, (Color){5, 6, 12, 245});
    DrawRectangleLinesEx((Rectangle){330, 170, 1012, 650}, 4, RED);
    const char *titulo = "SINAL PERDIDO";
    const char *mensagem = "JOHN FOI ABSORVIDO PELA FITA.";
    const char *descricao = "A sessao terminou antes que a transmissao pudesse ser interrompida.";
    DrawText(titulo, (LARGURA_MUNDO - MeasureText(titulo, 62)) / 2, 220, 62, RED);
    DrawText(mensagem, (LARGURA_MUNDO - MeasureText(mensagem, 28)) / 2, 315, 28, RAYWHITE);
    DrawText(descricao, (LARGURA_MUNDO - MeasureText(descricao, 20)) / 2, 375, 20, LIGHTGRAY);
    const char *opcoes[] = {"TENTAR NOVAMENTE", "MENU PRINCIPAL", "SAIR"};
    Vector2 mouse = mouseNoMundo();
    
    for(int i = 0; i < 3; i++){
        Rectangle area = botoesDerrota[i];
        bool mouseSobre = CheckCollisionPointRec(mouse, area);
        bool selecionado = i == opcaoDerrotaSelecionada || mouseSobre;
        DrawRectangleRec(area, selecionado ? (Color){95, 12, 26, 250} : (Color){30, 12, 18, 250});
        DrawRectangleLinesEx(area, selecionado ? 3.0f : 1.5f, selecionado ? GOLD : RED);
        DrawText(opcoes[i], (int)(area.x + (area.width - MeasureText(opcoes[i], 25)) / 2), (int)area.y + 21, 25, selecionado ? GOLD : RAYWHITE);
    }
    DrawText("SETAS: NAVEGAR     ENTER: SELECIONAR", 590, 775, 19, LIGHTGRAY);
}