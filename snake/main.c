#include "raylib.h"
#include <math.h>
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
} fruit;

Vector2 generateRandomPos(int boundaries_x, int boundaries_y) {
  Vector2 response;

  srand(time(NULL));
  response.x = (float)(rand() % (boundaries_x - 1)) + 1;
  response.y = (float)(rand() % (boundaries_y - 1)) + 1;

  return response;
}

bool checkCollision(Vector2 p1, Vector2 p2) {
  float x = fabs(p1.x - p2.x);
  float y = fabs(p1.y - p2.y);

  if (x <= 8 && y <= 15)
    return true;
  return false;
}

int main() {
  const int screenWidth = 800;
  const int screenHeight = 450;
  const int walkSpeed = 20;

  snake.pos.x = 0;
  snake.pos.y = 50;
  snake.size = 1;

  Vector2 randomPos = generateRandomPos(screenHeight, screenHeight);

  fruit.pos.x = randomPos.x;
  fruit.pos.y = randomPos.y;

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

    snakeDots[snake.size] = '\0';
    char scoreText[50];
    snprintf(scoreText, sizeof(scoreText), "Score: %d", score);

    if (checkCollision(snake.pos, fruit.pos)) {
      score++;
      snake.size++;
      Vector2 randomPos = generateRandomPos(screenHeight, screenHeight);

      fruit.pos.x = randomPos.x;
      fruit.pos.y = randomPos.y;
    }

    BeginDrawing();
    ClearBackground(RAYWHITE);

    DrawText(scoreText, 5, 5, 30, BLACK);
    DrawText(snakeDots, snake.pos.x, snake.pos.y, 40, BLACK);
    DrawText("+", fruit.pos.x, fruit.pos.y, 30, BLACK);

    EndDrawing();
  }
  CloseWindow();
  return 0;
}
