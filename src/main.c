            ? animacoes[direcao][quadroAnimacao]
            : personagemParado[direcao];


        BeginTextureMode(quadro);
        ClearBackground(BLACK);


        DrawTexture(cenarios[salaAtual], 0, 0, WHITE);


        desenharJohnComCamadas(
            cenarios[salaAtual],
            personagemAtual,
            salaAtual,
            personagem
        );


        desenharObjetosMapa(salaAtual);


        if (escurecimento > 0) {
            DrawRectangle(
                0,
                0,
                LARGURA_MUNDO,
                ALTURA_MUNDO,
                Fade(BLACK, escurecimento * 2)
            );
        }


        if (mostrarColisoes)
            desenharDepuracao(salaAtual, personagem, texturasMascaras[salaAtual]);


        DrawRectangle(18, 16, 320, 64, (Color){6, 8, 14, 195});
        DrawText(salas[salaAtual].nome, 30, 24, 20, RAYWHITE);
        DrawText(
            TextFormat("FITAS: %d / 3", inventarioContarFitas(&inventario.dados)),
            30,
            52,
            18,
            GOLD
        );


        if (!inventarioAberto)
            desenharIndicacaoColeta(salaAtual, personagem);


        desenharInventario(&inventario, LARGURA_MUNDO, ALTURA_MUNDO);

        // As legendas são desenhadas sobre o cenário, dentro do quadro do mundo.
        desenharLegendas();

        EndTextureMode();


        BeginDrawing();
        ClearBackground(BLACK);


        DrawTexturePro(
            quadro.texture,
            (Rectangle){0, 0, LARGURA_MUNDO, -ALTURA_MUNDO},
            areaExibicao(GetScreenWidth(), GetScreenHeight()),
            (Vector2){0, 0},
            0,
            WHITE
        );


        EndDrawing();
    }


    if (quadro.id)
        UnloadRenderTexture(quadro);


    for (int sala = 0; sala < NUM_CENARIOS; sala++) {
        if (cenarios[sala].id)
            UnloadTexture(cenarios[sala]);
        if (texturasMascaras[sala].id)
            UnloadTexture(texturasMascaras[sala]);
        liberarColisoes(&mascaras[sala]);
    }


    for (int direcaoSprite = 0; direcaoSprite < NUM_DIRECOES; direcaoSprite++) {
        if (personagemParado[direcaoSprite].id)
            UnloadTexture(personagemParado[direcaoSprite]);


        for (int quadroSprite = 0; quadroSprite < NUM_QUADROS; quadroSprite++) {
            if (animacoes[direcaoSprite][quadroSprite].id)
                UnloadTexture(animacoes[direcaoSprite][quadroSprite]);
        }
    }


    liberarObjetosMapa();
    liberarTelaInventario(&inventario);
    liberarCamadas();
    liberarLegendas();
    CloseWindow();


    return erro ? 1 : 0;
}
