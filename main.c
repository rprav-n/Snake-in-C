#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <math.h>

#define GAME_NAME "Snake"
#define GAME_WIDTH 360
#define GAME_HEIGHT 360
#define FPS 60
#define GRID_SIZE 10
#define GRID_COUNT GAME_WIDTH / GRID_SIZE
#define BG_COLOR GetColor(0xaecc61ff)
#define SNAKE_COLOR GetColor(0x2d341bff)


typedef struct
{
    Vector2 pos;
    Vector2 dir;
}Segment;


typedef struct
{
    Vector2 size;
    int length;
    Segment body[GRID_COUNT * GRID_COUNT]
}Snake;

typedef struct
{
    Vector2 pos;
    Vector2 size;
}Food;


void handle_snake_direction(Snake* snake);
void update_snake_movement(Snake* snake);
void handle_snake_food_collision(Snake* snake, Food* food);
void draw_snake(Snake snake);

int main()
{

    int screen_width = 720;
    int screen_height = 720;
    InitWindow(screen_width, screen_height, GAME_NAME);
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetConfigFlags(FLAG_WINDOW_HIGHDPI);

    RenderTexture2D render_texture = LoadRenderTexture(GAME_WIDTH, GAME_HEIGHT);
    SetTextureFilter(render_texture.texture, TEXTURE_FILTER_POINT);

    Segment head = { 0 };
    head.pos.x = GAME_WIDTH / 2.f;
    head.pos.y = GAME_HEIGHT / 2.f;
    head.dir = (Vector2){ 0 };

    Snake snake = { 0 };
    snake.length = 1;
    snake.body[0] = head;
    snake.size = (Vector2){ GRID_SIZE, GRID_SIZE };

    Food food = { 0 };
    food.pos = (Vector2){ GRID_SIZE, GRID_SIZE };
    food.size = (Vector2){ GRID_SIZE, GRID_SIZE };

    float snake_update_timer = 1 / 10.f;
    float current_snake_update_timer = 0.f;


    SetTargetFPS(FPS);
    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();
        float screen_width = GetScreenWidth();
        float screen_height = GetScreenHeight();
        float scale = fminf(screen_width / (float)GAME_WIDTH, screen_height / (float)GAME_HEIGHT);

        // Updates
        {
            handle_snake_direction(&snake);
            current_snake_update_timer += dt;
            if (current_snake_update_timer >= snake_update_timer)
            {
                update_snake_movement(&snake);
                current_snake_update_timer = 0.f;
            }

            // snake and food collision
            handle_snake_food_collision(&snake, &food);

        }

        // Draw here
        {
            BeginTextureMode(render_texture);
            ClearBackground(BG_COLOR);

            // draw line
            /* for (size_t i = 0; i < GRID_COUNT; i++)
            {
                DrawLine(0, i * GRID_SIZE, GAME_WIDTH, i * GRID_SIZE, LIME);
            }
            for (size_t i = 0; i < GRID_COUNT; i++)
            {
                DrawLine(i * GRID_SIZE, 0, i * GRID_SIZE, GAME_HEIGHT, LIME);
            } */

            // draw snake
            draw_snake(snake);

            // draw food
            // DrawRectangleV(food.pos, food.size, RED);
            Rectangle food_rect = (Rectangle){ food.pos.x, food.pos.y, food.size.x, food.size.y };
            DrawRectangleRounded(food_rect, 0.4f, 4, RED);

            EndTextureMode();
        }

        // Draw render texture
        {
            Vector2 screen_center = (Vector2){ screen_width / 2, screen_height / 2 };
            Vector2 game_center = (Vector2){ GAME_WIDTH / 2, GAME_HEIGHT / 2 };

            Rectangle src_rect = (Rectangle){ 0, 0, render_texture.texture.width, -render_texture.texture.height };
            Rectangle dest_rect = (Rectangle){ screen_center.x - game_center.x * scale, screen_center.y - game_center.y * scale, GAME_WIDTH * scale, GAME_HEIGHT * scale };

            BeginDrawing();
            ClearBackground(RED);
            DrawTexturePro(render_texture.texture, src_rect, dest_rect, (Vector2) { 0 }, 0.f, WHITE);
            EndDrawing();
        }

    }

    CloseWindow();
}


void handle_snake_direction(Snake* snake)
{
    Segment head = snake->body[0];
    if (IsKeyPressed(KEY_LEFT))
    {
        head.dir.x = -1;
        head.dir.y = 0;
    }
    if
        (IsKeyPressed(KEY_RIGHT))
    {
        head.dir.x = 1;
        head.dir.y = 0;
    }
    if
        (IsKeyPressed(KEY_UP))
    {
        head.dir.y = -1;
        head.dir.x = 0;
    }
    if
        (IsKeyPressed(KEY_DOWN))
    {
        head.dir.y = 1;
        head.dir.x = 0;
    }
    snake->body[0] = head;

}


void update_snake_movement(Snake* snake)
{

    for (int i = snake->length - 1; i >= 0; i--)
    {
        if (i == 0)
        {
            Segment seg = snake->body[i];
            Vector2 movement = Vector2Scale(seg.dir, GRID_SIZE);
            seg.pos = Vector2Add(seg.pos, movement);
            snake->body[i] = seg;
        }
        else
        {
            Segment p_seg = snake->body[i - 1];
            snake->body[i].dir = p_seg.dir;
            snake->body[i].pos = p_seg.pos;
        }
    }

}


void handle_snake_food_collision(Snake* snake, Food* food)
{
    Segment head = snake->body[0];
    Rectangle snake_head_rect = (Rectangle){ head.pos.x, head.pos.y, snake->size.x, snake->size.y };
    Rectangle food_rect = (Rectangle){ food->pos.x, food->pos.y, food->size.x, food->size.y };

    if (CheckCollisionRecs(snake_head_rect, food_rect))
    {
        food->pos.x = round(GetRandomValue(0, GAME_WIDTH) / GRID_SIZE) * GRID_SIZE;
        food->pos.y = round(GetRandomValue(0, GAME_WIDTH) / GRID_SIZE) * GRID_SIZE;

        // append a new body to snake
        Segment new_seg = { 0 };
        new_seg.dir = head.dir;
        new_seg.pos = Vector2Subtract(head.pos, Vector2Scale(head.dir, GRID_SIZE));
        snake->body[snake->length] = new_seg;
        snake->length++;
    }

}


void draw_snake(Snake snake)
{
    for (size_t i = 0; i < snake.length; i++)
    {
        Rectangle snake_rect = (Rectangle){ snake.body[i].pos.x, snake.body[i].pos.y, snake.size.x, snake.size.y };
        DrawRectangleRounded(snake_rect, 0.4f, 4, SNAKE_COLOR);
    }
}









