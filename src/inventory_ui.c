#include "inventory_ui.h"

static void EmptySlot(Rectangle slot) {
    DrawRectangleRec(slot, Fade((Color){5,7,13,255}, .90f));
    DrawRectangleLinesEx(slot, 2, (Color){91,51,63,255});
    DrawLine((int)(slot.x+12),(int)(slot.y+12),(int)(slot.x+28),(int)(slot.y+12),(Color){158,52,63,255});
    DrawLine((int)(slot.x+12),(int)(slot.y+12),(int)(slot.x+12),(int)(slot.y+28),(Color){158,52,63,255});
    const char *text = "VAZIO";
    DrawText(text, (int)(slot.x+(slot.width-MeasureText(text,15))/2), (int)(slot.y+slot.height/2-7), 15, (Color){100,101,116,255});
}

void InventoryDraw(const Assets *assets) {
    Texture2D background = assets->inventoryBackground;
    DrawTexturePro(background, (Rectangle){0,0,(float)background.width,(float)background.height},
                   (Rectangle){0,0,VHS_WIDTH,VHS_HEIGHT}, (Vector2){0}, 0, WHITE);
    DrawText("INVENTÁRIO", 194, 35, 34, (Color){237,229,221,255});
    DrawText("VHS / A ÚLTIMA SESSÃO", 815, 46, 18, (Color){177,118,128,255});
    DrawText("FITAS VHS", 90, 130, 22, (Color){230,224,220,255});
    for (int i = 0; i < 3; i++) EmptySlot((Rectangle){90+i*226,178,204,148});
    DrawText("OBJETOS", 90, 372, 22, (Color){230,224,220,255});
    for (int i = 0; i < 4; i++) EmptySlot((Rectangle){90+i*169,416,148,140});
    DrawText("MOCHILA", 900, 132, 24, (Color){230,224,220,255});
    DrawRectangle(899,177,280,2,(Color){116,44,57,255});
    DrawText("Nenhum item", 900, 216, 22, (Color){174,167,173,255});
    DrawText("Os espaços estão vazios.", 900, 254, 16, (Color){125,122,137,255});
    DrawText("O jogo está pausado.", 900, 540, 17, (Color){174,167,173,255});
    DrawText("[TAB] FECHAR", 90, 642, 20, (Color){237,229,221,255});
    DrawText("[ESC] VOLTAR AO JOGO", 870, 643, 17, (Color){177,118,128,255});
}
