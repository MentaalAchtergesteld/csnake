#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"

typedef struct CellNode {
	Vector2 data;
	struct CellNode *next;
} CellNode;

typedef struct {
	CellNode *head;
	CellNode *tail;
	int length;
	Vector2 direction;
	Vector2 desiredDirection;
} Snake;

typedef struct {
	bool lost;

	int gridSize;	
	int cellSize;

	float baseSpeed;
	float speedFactor;
	float currentSpeed;
	float inc;

	Snake snake;
	Vector2 food;
} GameState;

Vector2 screenToGrid(Vector2 pos, int gridSize) {
	return (Vector2){
		floor(pos.x / gridSize),
		floor(pos.y / gridSize),
	};
}

Vector2 gridToScreen(Vector2 pos, int cellSize) {
	return (Vector2){
		pos.x * cellSize,
		pos.y * cellSize, 
	};
}

void moveSnake(Snake *snake) {
    if (!snake->head) return;

    CellNode *cell = snake->tail;

    while (cell != NULL) {
        CellNode *next = cell->next;
        if (next == NULL) break; 

        cell->data = next->data;

        cell = next;
    }

		snake->head->data.x += snake->direction.x;
		snake->head->data.y += snake->direction.y;
}

void growSnake(Snake *snake, Vector2 tail) {
	CellNode *newTail = malloc(sizeof(CellNode));
	newTail->data = tail;
	newTail->next = snake->tail;

	snake->tail = newTail;
	snake->length += 1;
}

void resetSnake(Snake *snake) {
	CellNode* cell = snake->tail;

	while (cell != NULL) {
		CellNode* next = cell->next;
		free(cell);
		cell = next;
	}

	snake->tail = NULL;
	growSnake(snake, (Vector2){ 2, 0 });
	snake->head = snake->tail;
	growSnake(snake, (Vector2){ 1, 0 });
	growSnake(snake, (Vector2){ 0, 0 });

	snake->length = 3;
	snake->direction = (Vector2){ 1, 0 };
	snake->desiredDirection = (Vector2){ 1, 0 };
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
		tail = state->snake.tail->data;
		state->snake.direction = state->snake.desiredDirection;
		moveSnake(&state->snake);

		CellNode* head = state->snake.head;
		CellNode* cell = state->snake.tail;
		while (cell != NULL) {
			CellNode* next = cell->next;
			if ( next == NULL) break;

			if (cell->data.x == head->data.x && cell->data.y == head->data.y) {
				state->lost = true;
				break;
			}

			cell = next;
		}

		if (state->snake.head->data.x < 0 || state->snake.head->data.x >= state->gridSize) {
			state->lost = true;
			return;
		}

		if (state->snake.head->data.y < 0 || state->snake.head->data.y >= state->gridSize) {
			printf("Y\n");
			state->lost = true;
			return;
		}


		if (state->snake.head->data.x == state->food.x && state->snake.head->data.y == state->food.y) {
			growSnake(&state->snake, tail);
			randomizeFood(&state->food, state->gridSize);
			state->currentSpeed = state->baseSpeed * powf(state->speedFactor, state->snake.length-1);
		}
		state->inc -= state->currentSpeed;
	};

	if      (IsKeyPressed(KEY_W) && state->snake.direction.y !=  1) state->snake.desiredDirection = (Vector2){ 0,-1};
	else if (IsKeyPressed(KEY_S) && state->snake.direction.y != -1) state->snake.desiredDirection = (Vector2){ 0, 1};
	else if (IsKeyPressed(KEY_A) && state->snake.direction.x !=  1) state->snake.desiredDirection = (Vector2){-1, 0};
	else if (IsKeyPressed(KEY_D) && state->snake.direction.x != -1) state->snake.desiredDirection = (Vector2){ 1, 0};
	else if (IsKeyPressed(KEY_SPACE)) TakeScreenshot("screenshot.png");
} 

int main() {
	srand(time(NULL));
	const int windowSize = 1024;
	const int targetFPS = 60;

	const int gridSize = 32;
	const int cellSize = windowSize/gridSize;

	const int gridOffset = (windowSize - gridSize * cellSize)/2;

	GameState state = {
		.lost = false,
		.snake = {
			.length = 3,
			.direction = { 1, 0 },
			.desiredDirection = state.snake.direction, 
		},
		.gridSize = gridSize,
		.cellSize = cellSize,
		.baseSpeed = 0.25,
		.speedFactor = 0.95,
		.currentSpeed = state.baseSpeed,
		.food = { 0, 0 },
		.inc = 0
	};

	resetSnake(&state.snake);
	randomizeFood(&state.food, gridSize);

	InitWindow(windowSize, windowSize, "C Snake");
	SetTargetFPS(targetFPS);

	while (!WindowShouldClose()) {
		updateGame(&state);

		while (state.lost) {
			BeginDrawing();
			char *message = "You lost, press space to play again";
			int fontSize = 32;
			unsigned int length = MeasureText(message, fontSize);
			DrawText(
					message,
					windowSize/2 - length / 2, windowSize/2  - fontSize/2,
					fontSize, BLACK
			);
			EndDrawing();
			if (IsKeyPressed(KEY_SPACE)) {
				state.lost = false;
				state.inc = 0;
				state.currentSpeed = state.baseSpeed;
				resetSnake(&state.snake);
				randomizeFood(&state.food, state.gridSize);
			}
			if (WindowShouldClose()) break;
		}

		BeginDrawing();
		ClearBackground(RAYWHITE);

		for (int y = 0; y < gridSize; y++) {
			for (int x = 0; x < gridSize; x++) {
				Color color;
				if ((x + y) % 2 == 0) { color = GRAY; } 
				else 								  { color = LIGHTGRAY; };

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

		CellNode *cell = state.snake.tail;
		while (cell != NULL) {
			Vector2 screenPos = gridToScreen(cell->data, cellSize);
			DrawRectangle(
					gridOffset + screenPos.x, gridOffset + screenPos.y,
					cellSize, cellSize,
					GREEN
			);

			cell = cell->next;
		}

		Vector2 pos = gridToScreen(state.snake.head->data, cellSize);
		DrawRectangle(gridOffset+pos.x, gridOffset+pos.y, cellSize, cellSize, BLUE);
		EndDrawing();
	}

	return 0;
}
