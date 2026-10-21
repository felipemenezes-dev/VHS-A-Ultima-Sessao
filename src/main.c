
#include "raylib.h"
#include <math.h>

#define VELOCIDADE_JOHN 220.0f
#define ESCALA_JOHN 0.22f

int main(void)
{
    SetConfigFlags(FLAG_FULLSCREEN_MODE);

    int monitor = GetCurrentMonitor();

    InitWindow(
        GetMonitorWidth(monitor),
        GetMonitorHeight(monitor),
        "VHS - A Ultima Sessao"
    );

    SetTargetFPS(60);

    Texture2D fundo = LoadTexture("assets/cenarios/salao_principal.png");
    Texture2D john = LoadTexture("assets/personagens/john_frente.png");

    if (fundo.id == 0 || john.id == 0)
    {
        if (fundo.id != 0)
        {
            UnloadTexture(fundo);
        }

        if (john.id != 0)
        {
            UnloadTexture(john);
        }

        CloseWindow();
        return 1;
    }

    SetTextureFilter(fundo, TEXTURE_FILTER_POINT);
    SetTextureFilter(john, TEXTURE_FILTER_POINT);

    Vector2 posicaoJohn = {
        GetScreenWidth() * 0.50f,
        GetScreenHeight() * 0.78f
    };

    float larguraJohn = john.width * ESCALA_JOHN;
    float alturaJohn = john.height * ESCALA_JOHN;

    while (!WindowShouldClose())
    {
        float delta = GetFrameTime();

        Vector2 movimento = {0.0f, 0.0f};

        if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))
        {
            movimento.y -= 1.0f;
        }

        if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))
        {
            movimento.y += 1.0f;
        }

        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
        {
            movimento.x -= 1.0f;
        }

        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
        {
            movimento.x += 1.0f;
        }

        if (movimento.x != 0.0f || movimento.y != 0.0f)
        {
            float comprimento = sqrtf(
                movimento.x * movimento.x +
                movimento.y * movimento.y
            );

            movimento.x /= comprimento;
            movimento.y /= comprimento;

            posicaoJohn.x += movimento.x * VELOCIDADE_JOHN * delta;
            posicaoJohn.y += movimento.y * VELOCIDADE_JOHN * delta;
        }

        int larguraTela = GetScreenWidth();
        int alturaTela = GetScreenHeight();

        if (posicaoJohn.x < larguraJohn / 2.0f)
        {
            posicaoJohn.x = larguraJohn / 2.0f;
        }

        if (posicaoJohn.x > larguraTela - larguraJohn / 2.0f)
        {
            posicaoJohn.x = larguraTela - larguraJohn / 2.0f;
        }

        if (posicaoJohn.y < alturaJohn)
        {
            posicaoJohn.y = alturaJohn;
        }

        if (posicaoJohn.y > alturaTela)
        {
            posicaoJohn.y = alturaTela;
        }

        BeginDrawing();

        ClearBackground(BLACK);

        DrawTexturePro(
            fundo,
            (Rectangle){
                0.0f,
                0.0f,
                (float)fundo.width,
                (float)fundo.height
            },
            (Rectangle){
                0.0f,
                0.0f,
                (float)larguraTela,
                (float)alturaTela
            },
            (Vector2){0.0f, 0.0f},
            0.0f,
            WHITE
        );

        DrawTexturePro(
            john,
            (Rectangle){
                0.0f,
                0.0f,
                (float)john.width,
                (float)john.height
            },
            (Rectangle){
                posicaoJohn.x - larguraJohn / 2.0f,
                posicaoJohn.y - alturaJohn,
                larguraJohn,
                alturaJohn
            },
            (Vector2){0.0f, 0.0f},
            0.0f,
            WHITE
        );

        EndDrawing();
    }

    UnloadTexture(fundo);
    UnloadTexture(john);

    CloseWindow();

    return 0;
}
