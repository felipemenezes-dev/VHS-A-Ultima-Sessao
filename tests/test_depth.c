#include "scene_draw.h"
#include "collision.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Visual integration checks: use the same assets, crop and renderer as the game.
// DEPTH_BASELINE links the saved old renderer to record comparable before images.
#ifdef DEPTH_BASELINE
bool sceneDrawInit(void) { return true; }
void sceneDrawUnload(void) {}
#endif

typedef struct { int room; Vector2 p; const char *name; } Sample;
static const Sample samples[] = {
    {ESCRITORIO, {330, 400}, "escritorio_esquerda"},
    {ESCRITORIO, {1361, 434}, "escritorio_direita"},
    {ESCRITORIO, {640, 385}, "escritorio_mesa_atras"},
    {ESCRITORIO, {820, 485}, "escritorio_mesa_frente"},
    {SALAO, {430, 390}, "salao_estante_atras"},
    {SALAO, {380, 565}, "salao_estante_frente"},
    {SALAO, {1020, 610}, "salao_expositor"},
    {BANHEIRO, {535, 460}, "banheiro_divisoria"},
    {BANHEIRO, {1045, 325}, "banheiro_pia_atras"},
    {BANHEIRO, {840, 455}, "banheiro_pia_frente"},
    {ESTOQUE, {610, 465}, "estoque_estante"},
    {ESTOQUE, {1225, 605}, "estoque_bancada"},
    {ESTOQUE, {840, 720}, "estoque_chao"},
    {SALA_PROJECAO, {660, 355}, "projecao_mesa_atras"},
    {SALA_PROJECAO, {810, 495}, "projecao_mesa_frente"},
    {SALA_PROJECAO, {1100, 475}, "projecao_bancada"},
    {SALAO, {500, 270}, "porta_escritorio"},
    {SALAO, {1174, 275}, "porta_estoque"},
    {SALAO, {165, 520}, "porta_banheiro"},
    {BANHEIRO, {1360, 620}, "banheiro_saida"},
    {ESCRITORIO, {835, 775}, "escritorio_saida"},
    {ESTOQUE, {845, 800}, "estoque_saida"},
    {ESTOQUE, {845, 260}, "estoque_projecao"},
    {SALA_PROJECAO, {825, 785}, "projecao_saida"},
    {ESCRITORIO, {835, 650}, "escritorio_chao"},
};
#define SAMPLE_COUNT ((int)(sizeof samples / sizeof *samples))
static int failures;
static void check(bool condition, const char *message) {
    if (!condition) { fprintf(stderr, "FAIL %s\n", message); failures++; }
}
static Texture2D loadJohn(const char *path) {
    Image im = LoadImage(path);
    if (!im.data) return (Texture2D){0};
    ImageAlphaCrop(&im, .05f);
    Texture2D t = LoadTextureFromImage(im);
    UnloadImage(im);
    SetTextureFilter(t, TEXTURE_FILTER_POINT);
    return t;
}
static void render(RenderTexture2D rt, Texture2D bg, Texture2D john, const Sample *s) {
    BeginTextureMode(rt);
    ClearBackground(BLACK);
    DrawTexture(bg, 0, 0, WHITE);
    sceneDrawJohn(bg, john, s->room, s->p);
    EndTextureMode();
}
static Image readRender(RenderTexture2D rt) {
    Image im = LoadImageFromTexture(rt.texture);
    ImageFlipVertical(&im);
    return im;
}
static bool same(Color a, Color b) { return a.r==b.r && a.g==b.g && a.b==b.b; }
#ifndef DEPTH_BASELINE
static void checkOfficeOcclusion(RenderTexture2D rt, Texture2D background, Texture2D *john,
                                 const Color *backgroundPixels) {
    for(int sample=0;sample<2;sample++)for(int f=0;f<20;f++) {
        const Sample *s=&samples[sample];
        render(rt,background,john[f],s);
        Image layered=readRender(rt);Color *actual=LoadImageColors(layered);
        BeginTextureMode(rt);ClearBackground(BLACK);DrawTexture(background,0,0,WHITE);
        float width=JOHN_HEIGHT*john[f].width/john[f].height;
        DrawTexturePro(john[f],(Rectangle){0,0,john[f].width,john[f].height},
            (Rectangle){s->p.x-width/2,s->p.y-JOHN_HEIGHT,width,JOHN_HEIGHT},(Vector2){0},0,WHITE);
        EndTextureMode();
        Image raw=readRender(rt);Color *expected=LoadImageColors(raw);
        int hidden=0,headVisible=0,wrongColor=0;
        for(int y=0;y<WORLD_H;y++)for(int x=0;x<WORLD_W;x++) {
            int p=y*WORLD_W+x;
            if(same(expected[p],backgroundPixels[p]))continue;
            if(same(actual[p],backgroundPixels[p]))hidden++;
            else {
                if(!same(actual[p],expected[p]))wrongColor++;
                if(y<s->p.y-JOHN_HEIGHT+40)headVisible++;
            }
        }
        check(hidden>200,"reported office shelving hides body pixels");
        check(headVisible>100,"reported office shelving leaves the head visible");
        check(wrongColor==0,"occlusion restores original art without colored or rectangular overlays");
        UnloadImageColors(actual);UnloadImageColors(expected);UnloadImage(layered);UnloadImage(raw);
    }
    puts("OFFICE REGRESSION: 40 poses checked for partial body occlusion and visible head");
}
#endif
int main(int argc, char **argv) {
    if (argc != 2) { fprintf(stderr,"Usage: test_depth existing-output-directory\n"); return 2; }
    SetTraceLogLevel(LOG_WARNING);
    SetConfigFlags(FLAG_WINDOW_HIDDEN);
    InitWindow(1280, 720, "Depth visual verification");
    if (!IsWindowReady() || !sceneDrawInit()) return 2;
    Texture2D backgrounds[5] = {0}, john[20] = {0};
    Color *pixels[5] = {0};
    CollisionMap maps[5] = {0};
    for (int r=0;r<5;r++) {
        Image im=LoadImage(rooms[r].image);
        pixels[r]=LoadImageColors(im);
        backgrounds[r]=LoadTextureFromImage(im);
        UnloadImage(im);
        check(backgrounds[r].id != 0 && collisionLoadRoom(&maps[r],r), "assets and masks load");
    }
    const char *dirs[]={"baixo","cima","direita","esquerda"};
    for(int d=0;d<4;d++) for(int f=0;f<5;f++) {
        char path[256];
        if(f==0) snprintf(path,sizeof path,"assets/personagens/john/%s/john_parado_%s.png",dirs[d],dirs[d]);
        else snprintf(path,sizeof path,"assets/personagens/john/%s/john_andando_%s%d.png",dirs[d],dirs[d],f);
        john[d*5+f]=loadJohn(path);
        check(john[d*5+f].id!=0,"all four idle and sixteen walk frames load");
    }
    RenderTexture2D rt=LoadRenderTexture(WORLD_W,WORLD_H);
#ifndef DEPTH_BASELINE
    checkOfficeOcclusion(rt,backgrounds[ESCRITORIO],john,pixels[ESCRITORIO]);
#endif
    RenderTexture2D sheet=LoadRenderTexture(1280,7*240);
    BeginTextureMode(sheet);ClearBackground((Color){12,14,20,255});EndTextureMode();
    RenderTexture2D frames=LoadRenderTexture(4*320,5*240);
    BeginTextureMode(frames);ClearBackground(BLACK);EndTextureMode();
    int poses=0;
    for(int i=0;i<SAMPLE_COUNT;i++) {
        const Sample *s=&samples[i];
        int minimumVisible=WORLD_W*WORLD_H, maximumVisible=0;
        for(int f=0;f<20;f++) {
            render(rt,backgrounds[s->room],john[f],s);
            Image im=readRender(rt);
            Color *actual=LoadImageColors(im);
            float width=JOHN_HEIGHT*john[f].width/john[f].height;
            Rectangle sprite={s->p.x-width/2-1,s->p.y-JOHN_HEIGHT-1,width+2,JOHN_HEIGHT+2};
            int visible=0,outside=0;
            for(int y=0;y<WORLD_H;y++)for(int x=0;x<WORLD_W;x++) {
                if(same(actual[y*WORLD_W+x],pixels[s->room][y*WORLD_W+x])) continue;
                visible++;
                if(x<sprite.x || x>sprite.x+sprite.width || y<sprite.y || y>sprite.y+sprite.height) outside++;
            }
            check(outside==0,"background outside John is pixel-identical");
            check(visible>25,"John never disappears completely at representative positions");
            if(visible<minimumVisible)minimumVisible=visible;
            if(visible>maximumVisible)maximumVisible=visible;
            UnloadImageColors(actual);
            if(f==0) {
                char path[512];snprintf(path,sizeof path,"%s/%02d-%s.png",argv[1],i,s->name);
                ExportImage(im,path);
                BeginTextureMode(sheet);
                int tx=(i%4)*320,ty=(i/4)*240;
                DrawTexturePro(rt.texture,(Rectangle){s->p.x-160,WORLD_H-(s->p.y+40),320,-200},
                    (Rectangle){tx,ty+32,320,200},(Vector2){0},0,WHITE);
                DrawText(s->name,tx+6,ty+6,15,RAYWHITE);
                EndTextureMode();
            }
            if(i==1) {
                BeginTextureMode(frames);
                int tx=(f/5)*320,ty=(f%5)*240;
                DrawTexturePro(rt.texture,(Rectangle){s->p.x-160,WORLD_H-(s->p.y+40),320,-200},
                    (Rectangle){tx,ty+32,320,200},(Vector2){0},0,WHITE);
                DrawText(TextFormat("%s %s %d",dirs[f/5],f%5?"andando":"parado",f%5),tx+6,ty+6,17,RAYWHITE);
                EndTextureMode();
            }
            UnloadImage(im);poses++;
        }
        printf("POSE %-28s visible=%d..%d feet=%s\n",s->name,minimumVisible,maximumVisible,
            collisionFeet(&maps[s->room],s->p)?"blocked (visual stress)":"free");
    }
    Image contact=readRender(sheet); char path[512];
    snprintf(path,sizeof path,"%s/contato.png",argv[1]); ExportImage(contact,path);UnloadImage(contact);
    contact=readRender(frames);snprintf(path,sizeof path,"%s/direcoes.png",argv[1]); ExportImage(contact,path);UnloadImage(contact);
    // All five arrival areas are clear floor: no part of any animation may be
    // erased there by a foreground's transparent area or its bounding rectangle.
    for(int r=0;r<5;r++) for(int f=0;f<20;f++) {
        Sample s={r,rooms[r].entry,"clear floor"};
        render(rt,backgrounds[r],john[f],&s);
        Image actual=readRender(rt);Color *a=LoadImageColors(actual);
        BeginTextureMode(rt);ClearBackground(BLACK);DrawTexture(backgrounds[r],0,0,WHITE);
        float width=JOHN_HEIGHT*john[f].width/john[f].height;
        DrawTexturePro(john[f],(Rectangle){0,0,john[f].width,john[f].height},
            (Rectangle){s.p.x-width/2,s.p.y-JOHN_HEIGHT,width,JOHN_HEIGHT},(Vector2){0},0,WHITE);
        EndTextureMode();
        Image expected=readRender(rt);Color *e=LoadImageColors(expected);int differences=0;
        for(int n=0;n<WORLD_W*WORLD_H;n++)differences+=!same(a[n],e[n]);
        check(differences==0,"all sprite pixels remain visible on clear floor");
        UnloadImageColors(a);UnloadImageColors(e);UnloadImage(actual);UnloadImage(expected);
    }
    // An invisible actor must not change even one background pixel anywhere.
    Image transparent=GenImageColor(64,128,BLANK);
    Texture2D empty=LoadTextureFromImage(transparent);UnloadImage(transparent);
    for(int r=0;r<5;r++) {
        Sample s={r,rooms[r].entry,"empty"};render(rt,backgrounds[r],empty,&s);
        Image im=readRender(rt);Color *p=LoadImageColors(im);int differences=0;
        for(int n=0;n<WORLD_W*WORLD_H;n++) differences+=!same(p[n],pixels[r][n]);
        check(differences==0,"transparent silhouette regions preserve original scene");
        UnloadImageColors(p);UnloadImage(im);
    }
    UnloadTexture(empty);UnloadRenderTexture(sheet);UnloadRenderTexture(frames);UnloadRenderTexture(rt);
    for(int r=0;r<5;r++){UnloadTexture(backgrounds[r]);UnloadImageColors(pixels[r]);collisionFree(&maps[r]);}
    for(int f=0;f<20;f++)UnloadTexture(john[f]);
    sceneDrawUnload();sceneDrawUnload();CloseWindow();
    printf("DEPTH: %d sprite poses, 100 clear-floor poses, 5 untouched backgrounds, %d failures\n",poses,failures);
    return failures?1:0;
}
