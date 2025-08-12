#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"

typedef struct {
	Vector2* cells;
	Vector2 direction;
	int length, capacity;
} Snake;

Vector2 screenToGrid(Vector2 pos, int gridSize) {
	Vector2 gridPos = {
		floor(pos.x / gridSize),
		floor(pos.y / gridSize),
	};
	return gridPos;
}

Vector2 gridToScreen(Vector2 pos, int cellSize) {
	Vector2 screenPos = {
		pos.x * cellSize,
		pos.y * cellSize, 
	};
	return screenPos;
}

void moveSnake(Snake *snake) {
	if (snake->length == 0) return;

	for (int i = snake->length-1; i >= 1; i--) {
		snake->cells[i].x = snake->cells[i-1].x;
		snake->cells[i].y = snake->cells[i-1].y;
	}

	snake->cells[0].x += snake->direction.x;
	snake->cells[0].y += snake->direction.y;
}

void growSnake(Snake *snake, Vector2 tail) {
	if (snake->length >= snake->capacity) {
		snake->capacity *= 2;
		snake->cells = realloc(snake->cells, sizeof(Vector2) * snake->capacity);
		if (snake->cells == NULL) {
			printf("Failed to realloc snake cells\n");
			exit(1);
		}
	}

	snake->cells[snake->length] = tail;
	snake->length++;
}

void randomizeFood(Vector2 *food, int gridSize) {
	food->x = rand() % gridSize;
	food->y = rand() % gridSize;
}

int main() {
	const int windowSize = 1024;
	const int targetFPS = 60;

	const int gridSize = 32;
	const int cellSize = windowSize/gridSize;

	const int gridOffset = (windowSize - gridSize * cellSize)/2;

	const float speedFactor = 0.95;
	const float baseSpeed   = 0.25;
	float moveSpeed = baseSpeed;

	Snake snake;
	snake.capacity = 4;
	snake.length = 2;
	snake.cells = malloc(sizeof(Vector2) * snake.capacity);

	snake.cells[0] = (Vector2){ 1, 0 };
	snake.cells[1] = (Vector2){ 0, 0 };


	snake.direction = (Vector2){ 1, 0 };

	Vector2 food;
	randomizeFood(&food, gridSize);

	InitWindow(windowSize, windowSize, "C Snake");
	SetTargetFPS(targetFPS);

	float inc = 0;
	while (!WindowShouldClose()) {
		float dt = GetFrameTime();
		inc += dt;

		Vector2 tail;
		while (inc > moveSpeed) {
			tail = snake.cells[snake.length-1];
			moveSnake(&snake);
			if (snake.cells[0].x == food.x && snake.cells[0].y == food.y) {
				growSnake(&snake, tail);
				randomizeFood(&food, gridSize);
				moveSpeed = baseSpeed * powf(speedFactor, snake.length-1);
			}
			inc -= moveSpeed;
		};

		if (IsKeyPressed(KEY_W)) snake.direction = (Vector2){ 0,-1};
		if (IsKeyPressed(KEY_S)) snake.direction = (Vector2){ 0, 1};
		if (IsKeyPressed(KEY_A)) snake.direction = (Vector2){-1, 0};
		if (IsKeyPressed(KEY_D)) snake.direction = (Vector2){ 1, 0};
		if (IsKeyPressed(KEY_SPACE)) TakeScreenshot("screenshot.png");

		BeginDrawing();
		ClearBackground(RAYWHITE);

		for (int y = 0; y < gridSize; y++) {
			for (int x = 0; x < gridSize; x++) {
				Color color;
				if ((x + y) % 2 == 0) { color = GRAY; } 
				else 								{ color = LIGHTGRAY; };

				Vector2 screenPos = gridToScreen((Vector2){x,y}, cellSize);
				DrawRectangle(
						gridOffset + screenPos.x, gridOffset + screenPos.y,
						cellSize, cellSize,
						color
				);
			}
		}

		Vector2 screenPos = gridToScreen(food, cellSize);
		DrawRectangle(
			gridOffset + screenPos.x, gridOffset + screenPos.y,
			cellSize, cellSize,
			RED
		);

		for (int i = 0; i < snake.length; i++) {
			Vector2 screenPos = gridToScreen(snake.cells[i], cellSize);
			DrawRectangle(
					gridOffset + screenPos.x, gridOffset + screenPos.y,
					cellSize, cellSize,
					GREEN
			);
		}

		EndDrawing();
	}

	return 0;
}
