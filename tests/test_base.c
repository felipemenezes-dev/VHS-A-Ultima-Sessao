#include "game.h"
#include "world.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#define CELL 8
#define COLS (VHS_WIDTH/CELL)
#define ROWS (VHS_HEIGHT/CELL)
#define CELLS (COLS*ROWS)
static int previous[CELLS], queue[CELLS];
static Vector2 path[CELLS];

static bool Near(Vector2 a, Vector2 b) { return fabsf(a.x-b.x)<.001f && fabsf(a.y-b.y)<.001f; }
static Vector2 Center(int index) { return (Vector2){(index%COLS)*CELL+CELL/2.0f,(index/COLS)*CELL+CELL/2.0f}; }

static bool CellFree(const World *world, RoomId room, Vector2 point, const Door *target) {
    if (WorldBlocked(world,room,point)) return false;
    const Room *definition=RoomsGet(room);
    for (size_t i=0;i<definition->doorCount;i++) {
        if (&definition->doors[i]!=target && CheckCollisionPointRec(point,definition->doors[i].trigger)) return false;
    }
    return true;
}

static int FindPath(const World *world, RoomId room, Vector2 start, const Door *target) {
    for (int i=0;i<CELLS;i++) previous[i]=-1;
    int first=(int)(start.y/CELL)*COLS+(int)(start.x/CELL);
    int head=0,tail=0,goal=-1;
    previous[first]=-2; queue[tail++]=first;
    while(head<tail) {
        int at=queue[head++];
        if (CheckCollisionPointRec(Center(at),target->trigger)) {goal=at;break;}
        int x=at%COLS,y=at/COLS;
        const int dx[]={-1,1,0,0},dy[]={0,0,-1,1};
        for (int d=0;d<4;d++) {
            int nx=x+dx[d],ny=y+dy[d];
            if(nx<0||nx>=COLS||ny<0||ny>=ROWS)continue;
            int next=ny*COLS+nx;
            if(previous[next]!=-1||!CellFree(world,room,Center(next),target))continue;
            previous[next]=at;queue[tail++]=next;
        }
    }
    if(goal<0)return 0;
    int count=0;
    for(int at=goal;at>=0;at=previous[at])path[count++]=Center(at);
    for(int i=0;i<count/2;i++){Vector2 t=path[i];path[i]=path[count-1-i];path[count-1-i]=t;}
    return count;
}

static bool Travel(Game *game, RoomId destination) {
    RoomId origin=game->room;
    const Room *definition=RoomsGet(origin);
    const Door *door=NULL;
    for(size_t i=0;i<definition->doorCount;i++)if(definition->doors[i].destination==destination)door=&definition->doors[i];
    assert(door);
    int length=FindPath(&game->world,origin,game->player.position,door);
    if(!length){fprintf(stderr,"SEM CAMINHO: %s -> %s\n",definition->name,RoomsGet(destination)->name);return false;}
    int point=0;
    for(int frame=0;frame<6000;frame++){
        if(game->room!=origin){assert(game->room==destination);assert(!WorldBlocked(&game->world,game->room,game->player.position));return true;}
        if(point>=length){
            Vector2 through={0};
            if(door->movementDirection==LEFT)through.x=-1;
            if(door->movementDirection==RIGHT)through.x=1;
            if(door->movementDirection==UP)through.y=-1;
            if(door->movementDirection==DOWN)through.y=1;
            GameStep(game,(GameInput){.movement=through},1.0f/60.0f);
            continue;
        }
        Vector2 delta={path[point].x-game->player.position.x,path[point].y-game->player.position.y};
        if(fabsf(delta.x)<.05f&&fabsf(delta.y)<.05f){point++;continue;}
        float distance=sqrtf(delta.x*delta.x+delta.y*delta.y);
        GameStep(game,(GameInput){.movement=delta},fminf(1.0f/60.0f,distance/252.0f));
        assert(!WorldBlocked(&game->world,game->room,game->player.position));
    }
    fprintf(stderr,"TRANSICAO NAO ALCANCADA: %s -> %s em %.2f,%.2f ponto=%d/%d\n",definition->name,RoomsGet(destination)->name,game->player.position.x,game->player.position.y,point,length);
    return false;
}

static void SaveCanvas(Game *game,const char *pathName){
    GameDraw(game);
    Image image=LoadImageFromTexture(game->canvas.texture);
    ImageFlipVertical(&image);
    assert(ExportImage(image,pathName));
    UnloadImage(image);
}

int main(void){
    SetTraceLogLevel(LOG_WARNING);
    SetConfigFlags(FLAG_WINDOW_HIDDEN|FLAG_WINDOW_RESIZABLE);
    InitWindow(VHS_WIDTH,VHS_HEIGHT,"Teste VHS");
    assert(IsWindowReady());
    SetTargetFPS(0);
    Game game;
    assert(GameInit(&game));
    assert(!game.inventoryOpen);

    for(int r=0;r<ROOM_COUNT;r++){
        game.room=(RoomId)r;
        PlayerInit(&game.player,RoomsGet((RoomId)r)->spawn);
        game.debugCollisions=true;
        char file[80];snprintf(file,sizeof(file),"build/qa_sala_%d.png",r);
        SaveCanvas(&game,file);
    }

    // Every spawn/arrival is clear and every doorway has at least one reachable grid cell.
    for(int r=0;r<ROOM_COUNT;r++){
        const Room *room=RoomsGet((RoomId)r);
        assert(room&&(r==ROOM_HALL||room->obstacleCount>0));
        assert(!WorldBlocked(&game.world,(RoomId)r,room->spawn));
        for(size_t d=0;d<room->doorCount;d++){
            assert(!WorldBlocked(&game.world,room->doors[d].destination,room->doors[d].arrival));
            if(FindPath(&game.world,(RoomId)r,room->spawn,&room->doors[d])<=0){
                fprintf(stderr,"PORTA INACESSIVEL: %s -> %s\n",room->name,RoomsGet(room->doors[d].destination)->name);
                return 1;
            }
        }
        game.room=(RoomId)r;
        PlayerInit(&game.player,room->spawn);
        game.debugCollisions=true;
        char file[80];snprintf(file,sizeof(file),"build/qa_sala_%d.png",r);
        SaveCanvas(&game,file);
    }
    game.debugCollisions=false;
    game.room=ROOM_HALL;
    PlayerInit(&game.player,RoomsGet(ROOM_HALL)->spawn);

    const RoomId visits[]={ROOM_BATHROOM,ROOM_HALL,ROOM_OFFICE,ROOM_HALL,ROOM_STORAGE,ROOM_PROJECTION,ROOM_STORAGE,ROOM_HALL};
    for(size_t i=0;i<sizeof(visits)/sizeof(visits[0]);i++){
        assert(Travel(&game,visits[i]));
        printf("TESTE: entrou em %s sem atravessar obstaculos\n",RoomsGet(game.room)->name);
    }

    // A shelf boundary in the upstream alpha mask blocks X but allows sliding.
    Vector2 start={420,330};
    assert(!WorldBlocked(&game.world,ROOM_HALL,start));
    Vector2 stopped=WorldMove(&game.world,ROOM_HALL,start,(Vector2){1000,0});
    assert(stopped.x<440&&stopped.x>=start.x);
    Vector2 slid=WorldMove(&game.world,ROOM_HALL,start,(Vector2){20,20});
    assert(slid.x<440&&slid.y>start.y+19);

    // Tab toggles; movement and animation are frozen; Escape closes before requesting exit.
    PlayerInit(&game.player,RoomsGet(ROOM_HALL)->spawn);
    GameStep(&game,(GameInput){.toggleInventory=true},1.0f/60);
    assert(game.inventoryOpen);
    Player saved=game.player;
    for(int i=0;i<60;i++)GameStep(&game,(GameInput){.movement={1,1}},1.0f/60);
    assert(Near(saved.position,game.player.position));
    assert(saved.frame==game.player.frame&&saved.animationTime==game.player.animationTime);
    SaveCanvas(&game,"build/inventario_no_jogo.png");
    GameStep(&game,(GameInput){.toggleInventory=true,.movement={1,0}},1.0f/60);
    assert(!game.inventoryOpen&&Near(saved.position,game.player.position));
    GameStep(&game,(GameInput){.toggleInventory=true},1.0f/60);
    GameStep(&game,(GameInput){.escape=true},1.0f/60);
    assert(!game.inventoryOpen&&!game.exitRequested);

    // Window size affects presentation only.
    Vector2 before=game.player.position;
    SetWindowSize(960,540);
    GameDraw(&game);
    assert(Near(before,game.player.position));
    GameStep(&game,(GameInput){.escape=true},1.0f/60);
    assert(game.exitRequested);

    FilePathList images=LoadDirectoryFilesEx("assets",".png",true);
    unsigned int count=images.count;
    for(unsigned int i=0;i<count;i++){
        Image im=LoadImage(images.paths[i]);assert(im.data&&im.width>0&&im.height>0);UnloadImage(im);
    }
    UnloadDirectoryFiles(images);
    GameUnload(&game);GameUnload(&game);
    CloseWindow();
    printf("APROVADO: cinco salas, portas acessiveis, colisoes, deslizamento, Tab/Esc, pausa, %u imagens.\n",count);
    return 0;
}
