#include<stdio.h>
#include<raylib.h>
#include<time.h>
#include<stdlib.h>

#define MAX_LASERS 30
#define SCREENWIDTH 1200
#define SCREENHEIGHT 800
#define SCREENCINNAMONBORDER 75
#define ENEMYFALLVELOCITY 2
#define ENEMYZIGZAG 50

#define DEFINE_ARRAY_STRUCT(Type, MaxSize)                  \
    typedef struct Type##Array {                            \
        Type elements[MaxSize];                             \
        size_t size;                                        \
    } Type##Array;

typedef struct Player
{
    Vector2 pos;
    Vector2 vel;
}Player;

typedef struct LaserArray
{
    Vector2 lasers[MAX_LASERS];
    size_t size;
}LaserArray;

typedef struct Enemy
{
    Vector2 pos;
    Vector2 new_pos;
}Enemy;

#define MAX_ENEMIES 5
DEFINE_ARRAY_STRUCT(Enemy, MAX_ENEMIES)

Enemy
enemyCreate(Vector2 pos)
{
    Enemy enemy;
    enemy.pos = pos;
    enemy.new_pos = pos;
    return enemy;
}

void
enemyDraw(Enemy* enemy, Texture2D* textures)
{
    DrawTexture(textures[1], enemy->pos.x, enemy->pos.y, WHITE);
}

Player
playerCreate(Vector2 pos, Vector2 vel)
{
    Player player;
    player.pos = pos;
    player.vel = vel;
    return player;
}

void
playerDraw(Player* player, Texture2D* textures)
{
    DrawTexture(textures[0], player->pos.x, player->pos.y, WHITE);
}

void
loadSprites(Texture2D* textures){
    Image image = LoadImage("sprites/ship.png");
    if (image.data == NULL) {
        printf("Error: Unable to load player sprite.\n");
        return;
    }

    ImageResize(&image, 100,100);
    textures[0] = LoadTextureFromImage(image);

    image = LoadImage("sprites/enemy.png");
    if (image.data == NULL) {
        printf("Error: Unable to load player sprite.\n");
        return;
    }

    ImageResize(&image, 86,66);
    textures[1] = LoadTextureFromImage(image);

    UnloadImage(image);
}

void
enemyUpdate(Enemy* enemy)
{
    // printf("pos: %f newPos: %f\n", enemy->pos.x, enemy->new_pos.x);

    if(enemy->pos.x == enemy->new_pos.x)
    {
        int r = rand();
        if(r%2 == 0)
        {
            if(enemy->pos.x < SCREENWIDTH - SCREENCINNAMONBORDER - ENEMYZIGZAG)
            {
                enemy->new_pos.x = enemy->pos.x + ENEMYZIGZAG;
            }
        }
        else
        {
            if(enemy->pos.x > ENEMYZIGZAG)
            {
                enemy->new_pos.x = enemy->pos.x + -ENEMYZIGZAG;
            }
        }
    }
    else
    {
        if(enemy->new_pos.x < enemy->pos.x)
        {
            enemy->pos.x += -ENEMYZIGZAG/10;
        }
        else
        {
            enemy->pos.x += ENEMYZIGZAG/10;
        }
    }

    if(enemy->pos.y < 725 - ENEMYFALLVELOCITY) // Minus velocity, because we are not checking the new_position
    {
        enemy->pos.y += ENEMYFALLVELOCITY;
    }

}

void
playerUpdate(Player* player)
{
    float new_x = player->pos.x + player->vel.x;
    float new_y = player->pos.y + player->vel.y;

    if(new_x > 0 && new_x < SCREENWIDTH - SCREENCINNAMONBORDER)
    {
        player->pos.x += player->vel.x;
    }
    if(new_y > 0 && new_y < SCREENHEIGHT - SCREENCINNAMONBORDER)
    {
        player->pos.y += player->vel.y;
    }
    player->vel.x = 0;
    player->vel.y = 0;
}

void
laserCreate(Player* player, LaserArray* laserarr)
{
    Vector2 laser = {player->pos.x + 49, player->pos.y};
    laserarr->lasers[laserarr->size] = laser;
    laserarr->size++;
}

void
laserUpdate(LaserArray* laserarr)
{
    for (size_t i = 0; i < laserarr->size; )
    {
        laserarr->lasers[i].y += -5;
        if (laserarr->lasers[i].y < 0)
        {
            laserarr->lasers[i] = laserarr->lasers[laserarr->size - 1];
            laserarr->size--;
        }
        else
        {
            i++;
        }
    }
}

void
laserCollide(LaserArray* laserarr, Enemy* enemyarr)
{
    for (size_t i = 0; i < laserarr->size;)
    {

        // if (laserarr->lasers[i]){}

    }
}

void
laserDraw(LaserArray* laserarr)
{
    for (size_t i = 0; i < laserarr->size; i++)
    {
        Vector2 end_line = {laserarr->lasers[i].x, laserarr->lasers[i].y - 15};
        DrawLineV(laserarr->lasers[i], end_line, RED);
    }
}

int main()
{
    InitWindow(SCREENWIDTH, SCREENHEIGHT, "Raylib");
    SetTargetFPS(60);

    srand(time(NULL));

    Texture2D textures[2];
    LaserArray laserarr;
    laserarr.size = 0;

    Camera2D camera;
    camera.target = (Vector2){ SCREENWIDTH/2.0f, SCREENHEIGHT/2.0f };
    camera.offset = (Vector2){ SCREENWIDTH/2.0f, SCREENHEIGHT/2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    Player player = playerCreate((Vector2){220,220}, (Vector2){0,0});
    Enemy enemy = enemyCreate((Vector2){200,100});

    loadSprites(textures);

    while (!WindowShouldClose())
    {
        // Input

        if(IsKeyDown(KEY_D))
        {
            player.vel.x = 5;
        }
        if(IsKeyDown(KEY_S))
        {
            player.vel.y = 5;
        }
        if(IsKeyDown(KEY_W))
        {
            player.vel.y = -5;
        }
        if(IsKeyDown(KEY_A))
        {
            player.vel.x = -5;
        }
        if(IsKeyPressed(KEY_SPACE))
        {
            laserCreate(&player, &laserarr);
        }

        // Update

        playerUpdate(&player);
        laserUpdate(&laserarr);
        enemyUpdate(&enemy);

        // Render

        BeginDrawing();
            ClearBackground(BLACK);
            DrawFPS(10,10);
            BeginMode2D(camera);

                playerDraw(&player, textures);
                laserDraw(&laserarr);
                enemyDraw(&enemy, textures);

                // char position[40];
                // sprintf(position, "X: %f Y: %f\n", player.pos.x, player.pos.y);
                // DrawText(position, 10, 40, 30, GREEN);
                // printf("%d", laserarr.size);

            EndMode2D();
        EndDrawing();
    }

    CloseWindow();



    return 0;
}