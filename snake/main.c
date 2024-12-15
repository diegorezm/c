#include "raylib.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct snake {
  Vector2 pos;
  int32_t size;
} snake;

struct fruit {
  Vector2 pos;
  int32_t size;
};

Vector2 generate_random_pos(int boundaries_x, int boundaries_y) {
  Vector2 response;

  srand(time(NULL));
  response.x = (float)(rand() % (boundaries_x - 1)) + 1;
  response.y = (float)(rand() % (boundaries_y - 1)) + 1;

  return response;
}

int main() {
  const int screenWidth = 800;
  const int screenHeight = 450;
  const int walkSpeed = 10;

  snake.pos.x = 0;
  snake.pos.y = 50;
  snake.size = 1;

  int score = 0;

  InitWindow(screenWidth, screenHeight, "Snake game");

  while (!WindowShouldClose()) {
    const int key = GetKeyPressed();

    switch (key) {
    case KEY_UP:
      snake.pos.y = snake.pos.y - walkSpeed;
      break;
    case KEY_DOWN:
      snake.pos.y = snake.pos.y + walkSpeed;
      break;
    case KEY_RIGHT:
      snake.pos.x = snake.pos.x + walkSpeed;
      break;
    case KEY_LEFT:
      snake.pos.x = snake.pos.x - walkSpeed;
      break;
    }

    char snakeDots[snake.size + 1];

    for (int i = 0; i < snake.size; i++) {
      snakeDots[i] = '.';
    }

    Vector2 randomPos = generate_random_pos(screenWidth, screenHeight - 5);
    snakeDots[snake.size] = '\0';
    char scoreText[50];
    snprintf(scoreText, sizeof(scoreText), "Score: %d", score);

    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawText(scoreText, 5, 5, 30, BLACK);
    DrawText(snakeDots, snake.pos.x, snake.pos.y, 40, BLACK);
    DrawText("+", randomPos.x, randomPos.y, 40, BLACK);
    EndDrawing();
  }
  CloseWindow();
  return 0;
}
