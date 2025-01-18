#include<stdio.h>
#include<raylib.h>
#include<time.h>
#include<stdlib.h>

#define MAX_LASERS 30
#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 800
#define SCREEN_CINNAMON_BORDER 75
#define ENEMY_FALL_VELOCITY 2
#define ENEMY_ZIGZAG 50
#define MAX_ENEMIES 5

#define DEFINE_ARRAY_STRUCT(Type, MaxSize)                  \
    typedef struct Type##Array {                            \
        Type elements[MaxSize];                             \
        size_t size;                                        \
    } Type##Array;


typedef struct Player
{
    Vector2 pos;
    Vector2 vel;
} Player;

typedef Vector2 Laser;
DEFINE_ARRAY_STRUCT(Laser, MAX_LASERS)

typedef struct Enemy
{
    Vector2 pos;
    Vector2 new_pos;
} Enemy;
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
enemiesDraw(EnemyArray* enemyarr, Texture2D* textures)
{
    for(size_t i = 0; i < enemyarr->size; i++)
    {
        DrawTexture(textures[1], enemyarr->elements[i].pos.x, enemyarr->elements[i].pos.y, WHITE);
    }
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
loadTextures(Texture2D* textures)
{
    Image image = LoadImage("sprites/ship.png");
    if (image.data == NULL) {
        printf("Error: Unable to load player sprite.\n");
        return;
    }

    ImageResize(&image, 100, 100);
    textures[0] = LoadTextureFromImage(image);

    image = LoadImage("sprites/enemy.png");
    if (image.data == NULL) {
        printf("Error: Unable to load enemy sprite.\n");
        return;
    }

    ImageResize(&image, 80, 60);
    textures[1] = LoadTextureFromImage(image);

    UnloadImage(image);
}

void
enemiesUpdate(EnemyArray* enemyarr)
{
    for(size_t i = 0; i < enemyarr->size; i++)
    {

        if (enemyarr->elements[i].pos.x == enemyarr->elements[i].new_pos.x)
        {
            int r = rand();
            if (r % 2 == 0)
            {
                if (enemyarr->elements[i].pos.x < SCREEN_WIDTH - SCREEN_CINNAMON_BORDER - ENEMY_ZIGZAG)
                {
                    enemyarr->elements[i].new_pos.x = enemyarr->elements[i].pos.x + ENEMY_ZIGZAG;
                }
            }
            else
            {
                if (enemyarr->elements[i].pos.x > ENEMY_ZIGZAG)
                {
                    enemyarr->elements[i].new_pos.x = enemyarr->elements[i].pos.x - ENEMY_ZIGZAG;
                }
            }
        }
        else
        {
            if (enemyarr->elements[i].new_pos.x < enemyarr->elements[i].pos.x)
            {
                enemyarr->elements[i].pos.x -= ENEMY_ZIGZAG / 10;
            }
            else
            {
                enemyarr->elements[i].pos.x += ENEMY_ZIGZAG / 10;
            }
        }

        if (enemyarr->elements[i].pos.y < 725 - ENEMY_FALL_VELOCITY)
        {
            enemyarr->elements[i].pos.y += ENEMY_FALL_VELOCITY;
        }
    }
}

void
playerUpdate(Player* player)
{
    float new_x = player->pos.x + player->vel.x;
    float new_y = player->pos.y + player->vel.y;

    if (new_x > 0 && new_x < SCREEN_WIDTH - SCREEN_CINNAMON_BORDER)
    {
        player->pos.x += player->vel.x;
    }
    if (new_y > 0 && new_y < SCREEN_HEIGHT - SCREEN_CINNAMON_BORDER)
    {
        player->pos.y += player->vel.y;
    }
    player->vel.x = 0;
    player->vel.y = 0;
}

void
laserCreate(Player* player, LaserArray* laserarr)
{
    Laser laser = {player->pos.x + 49, player->pos.y};
    laserarr->elements[laserarr->size] = laser;
    laserarr->size++;
}

void
laserUpdate(LaserArray* laserarr)
{
    for (size_t i = 0; i < laserarr->size;)
    {
        laserarr->elements[i].y -= 5;
        if (laserarr->elements[i].y < 0)
        {
            laserarr->elements[i] = laserarr->elements[laserarr->size - 1];
            laserarr->size--;
        }
        else
        {
            i++;
        }
    }
}

void
laserCollide(LaserArray* laserarr, EnemyArray* enemyarr)
{
    for (size_t i = 0; i < laserarr->size;)
    {
        bool laser_collided = false;
        for (size_t j = 0; j < enemyarr->size;)
        {
            Rectangle rec_laser = {laserarr->elements[i].x, laserarr->elements[i].y - 15, 5, 15};
            Rectangle rec_enemy = {enemyarr->elements[j].pos.x, enemyarr->elements[j].pos.y, 80, 60};
            // DrawRectangleRec(rec_enemy, RED);
            if(CheckCollisionRecs(rec_laser, rec_enemy))
            {
                laserarr->elements[i] = laserarr->elements[laserarr->size -1];
                laserarr->size--;

                enemyarr->elements[j] = enemyarr->elements[enemyarr->size -1];
                enemyarr->size--;

                laser_collided = true;
            }
            else{j++;}
        }
        if(!laser_collided){i++;}
    }
}

void
laserDraw(LaserArray* laserarr)
{
    for (size_t i = 0; i < laserarr->size; i++)
    {
        Vector2 end_line = {laserarr->elements[i].x, laserarr->elements[i].y - 15};
        DrawLineV(laserarr->elements[i], end_line, RED);
    }
}

int main()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Raylib");
    SetTargetFPS(60);

    srand(time(NULL));

    Texture2D textures[2];

    LaserArray laserarr;
    laserarr.size = 0;

    EnemyArray enemyarr;
    enemyarr.size = 0;

    Camera2D camera;
    camera.target = (Vector2){SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
    camera.offset = (Vector2){SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    Player player = playerCreate((Vector2){220, 220}, (Vector2){0, 0});

    for(size_t i = 0; i < 5; i++)
    {
        Enemy enemy = enemyCreate((Vector2){i * 100, 0});
        enemyarr.elements[i] = enemy;
        enemyarr.size++;
    }

    loadTextures(textures);

    while (!WindowShouldClose())
    {
        // Input
        if (IsKeyDown(KEY_D))
        {
            player.vel.x = 5;
        }
        if (IsKeyDown(KEY_S))
        {
            player.vel.y = 5;
        }
        if (IsKeyDown(KEY_W))
        {
            player.vel.y = -5;
        }
        if (IsKeyDown(KEY_A))
        {
            player.vel.x = -5;
        }
        if (IsKeyPressed(KEY_SPACE))
        {
            laserCreate(&player, &laserarr);
        }

        // Update

        playerUpdate(&player);
        laserUpdate(&laserarr);
        enemiesUpdate(&enemyarr);
        laserCollide(&laserarr, &enemyarr);

        // Render
        BeginDrawing();
            ClearBackground(BLACK);
            DrawFPS(10, 10);
            BeginMode2D(camera);

                playerDraw(&player, textures);
                laserDraw(&laserarr);
                enemiesDraw(&enemyarr, textures);

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
