#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"

typedef struct CellNode {
	Vector2* data;
	struct CellNode* next;
} CellNode;

typedef struct {
	CellNode* head;
	CellNode* tail;
	int length;
	Vector2 direction;
} Snake;

typedef struct {
	int gridSize;	
	int cellSize;

	float baseSpeed;
	float speedFactor;
	float currentSpeed;

	Snake snake;
	Vector2 food;

	float inc;

} GameState;

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
    if (!snake->head) return;

    CellNode* cell = snake->tail;

    while (cell != NULL) {
        CellNode* next = cell->next;
        if (next == NULL) break; 

        cell->data->x = next->data->x;
        cell->data->y = next->data->y;

        cell = next;
    }

		snake->head->data->x += snake->direction.x;
		snake->head->data->y += snake->direction.y;
}

void growSnake(Snake *snake, Vector2 tail) {
	CellNode* newTail = malloc(sizeof(CellNode));
	if (!newTail) {
		printf("ERROR: couldn't allocate CellNode in growing snake");
		exit(1);
	}

	Vector2* pos = malloc(sizeof(Vector2));
	if (!pos) {
		free(newTail);
		printf("ERROR: couldn't allocate Vector2 in growing snake");
		exit(1);
	}

	*pos = tail;

	newTail->data = pos;
	newTail->next = snake->tail;

	snake->tail = newTail;
	snake->length += 1;
}

void randomizeFood(Vector2 *food, int gridSize) {
	food->x = rand() % gridSize;
	food->y = rand() % gridSize;
}

void updateGame(GameState *state) {
	float dt = GetFrameTime();
	state->inc += dt;

	Vector2 tail;
	while (state->inc > state->currentSpeed) {
		tail = *state->snake.tail->data;
		moveSnake(&state->snake);
		if (state->snake.head->data->x == state->food.x && state->snake.head->data->y == state->food.y) {
			growSnake(&state->snake, tail);
			randomizeFood(&state->food, state->gridSize);
			state->currentSpeed = state->baseSpeed * powf(state->speedFactor, state->snake.length-1);
		}
		state->inc -= state->currentSpeed;
	};

	if (IsKeyPressed(KEY_W)) state->snake.direction = (Vector2){ 0,-1};
	if (IsKeyPressed(KEY_S)) state->snake.direction = (Vector2){ 0, 1};
	if (IsKeyPressed(KEY_A)) state->snake.direction = (Vector2){-1, 0};
	if (IsKeyPressed(KEY_D)) state->snake.direction = (Vector2){ 1, 0};
	if (IsKeyPressed(KEY_SPACE)) TakeScreenshot("screenshot.png");
} 

int main() {
	const int windowSize = 1024;
	const int targetFPS = 60;

	const int gridSize = 32;
	const int cellSize = windowSize/gridSize;

	const int gridOffset = (windowSize - gridSize * cellSize)/2;

	Snake snake;
	snake.length = 2;
	snake.direction = (Vector2){ 1, 0 };

	growSnake(&snake, (Vector2){ 1, 0 });
	snake.head = snake.tail;
	growSnake(&snake, (Vector2){ 0, 0 });

	Vector2 food;
	randomizeFood(&food, gridSize);

	GameState state;
	state.gridSize = gridSize;
	state.cellSize = cellSize;

	state.baseSpeed = 0.25;
	state.speedFactor = 0.95;
	state.currentSpeed = state.baseSpeed;
	state.snake = snake;
	state.food = food;
	state.inc = 0;

	InitWindow(windowSize, windowSize, "C Snake");
	SetTargetFPS(targetFPS);

	while (!WindowShouldClose()) {

		updateGame(&state);

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

		Vector2 screenPos = gridToScreen(state.food, cellSize);
		DrawRectangle(
			gridOffset + screenPos.x, gridOffset + screenPos.y,
			cellSize, cellSize,
			RED
		);

		CellNode* cell = state.snake.tail;
		while (cell != NULL) {
			Vector2 screenPos = gridToScreen(*cell->data, cellSize);
			DrawRectangle(
					gridOffset + screenPos.x, gridOffset + screenPos.y,
					cellSize, cellSize,
					GREEN
			);

			cell = cell->next;
		}

		Vector2 pos = gridToScreen(*snake.head->data, cellSize);
		DrawRectangle(gridOffset+pos.x, gridOffset+pos.y, cellSize, cellSize, BLUE);
		EndDrawing();
	}

	return 0;
}
