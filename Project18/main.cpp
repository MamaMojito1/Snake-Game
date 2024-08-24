#include "raylib.h"
#include <iostream>
#include <deque>
#include <raymath.h>

using namespace std;

Color green = { 173, 204, 96, 255 };
Color DarkGreen = { 43, 51, 24, 255 };

int cellSize = 25;
int cellCount = 25;
int offset = 75;

double lastUpdateTime = 0;

bool ElementInDeque(Vector2 element, deque<Vector2> deque)
{
	for (unsigned int i = 0; i < deque.size(); i++)
	{
		if (Vector2Equals(deque[i], element))
		{
			return true;
		}
	}
	return false;
}

bool eventTrigerred(double interval)
{
	double currentTime = GetTime();
	if (currentTime - lastUpdateTime >= interval)
	{
		lastUpdateTime = currentTime;
		return true;
	}
	return false;
}

class Snake
{
public:
	deque<Vector2> body = { Vector2{6.0f, 9.0f}, Vector2{5.0f, 9.0f}, Vector2{4.0f, 9.0f} };

	Vector2 direction = { 1, 0 };
	bool addSegement = false;

	void Draw()
	{
		for (unsigned int i = 0; i < body.size(); i++)
		{
			float x = body[i].x;
			float y = body[i].y;
			Rectangle segment = { offset + x * cellSize, offset + y * cellSize, (float)cellSize, (float)cellSize };
			DrawRectangleRounded(segment, 0.5f, 6, DarkGreen);
		}
	}

	void Update()
	{
		body.push_front(Vector2Add(body[0], direction));
		if (addSegement == true)
		{
			addSegement = false;
		}
		else
		{
			body.pop_back();
		}
	}
	void Reset()
	{
		body = { Vector2{ 6, 9}, Vector2{5, 9}, Vector2{4, 9} };
		direction = { 1, 0 };
	}
};

class Food
{
public:
	Vector2 position;
	Texture2D texture;

	Food(deque<Vector2> snakeBody)
	{
		Image image = LoadImage("graphics/food.png");
		texture = LoadTextureFromImage(image);
		UnloadImage(image);
		position = GenerateRandomPos(snakeBody);
	}

	~Food()
	{
		UnloadTexture(texture);
	}

	void Draw()
	{
		DrawTexture(texture, offset + position.x * cellSize, offset + position.y * cellSize, WHITE);
	}

	Vector2 GenerateRandomCell()
	{
		float x = static_cast<float>(GetRandomValue(0, cellCount - 1));
		float y = static_cast<float>(GetRandomValue(0, cellCount - 1));
		return Vector2{ x, y };
	}

	Vector2 GenerateRandomPos(deque<Vector2> snakeBody)
	{
		Vector2 position = GenerateRandomCell();
		while (ElementInDeque(position, snakeBody))
		{
			position = GenerateRandomCell();
		}
		return position;
	}
};

class Game
{
public:
	Snake snake = Snake();
	Food food = Food(snake.body);
	bool running = true;
	int score = 0;

	Sound eatSound;
	Sound wallSound;

	Game()
	{
		InitAudioDevice();
		eatSound = LoadSound("sound/Sounds_eat.mp3");
		wallSound = LoadSound("sound/Sounds_wall.mp3");
	}

	~Game()
	{
		UnloadSound(eatSound);
		UnloadSound(wallSound);
		CloseAudioDevice();
	}

	void Draw()
	{
		food.Draw();
		snake.Draw();
	}

	void Update()
	{
		if (running)
		{
			snake.Update();
			checkCollisionWithFood();
			checkCollisionWithEdge();
			CheckCollisionWithTail();
		}
	}

	void checkCollisionWithFood()
	{
		if (Vector2Equals(snake.body[0], food.position))
		{
			food.position = food.GenerateRandomPos(snake.body);
			snake.addSegement = true;
			score = score + 1;
			PlaySound(eatSound);
		}
	}
	void GameOver()
	{
		snake.Reset();
		food.position = food.GenerateRandomPos(snake.body);
		running = false;
		score = 0;
		PlaySound(wallSound);
	}
	void checkCollisionWithEdge()
	{
		if (snake.body[0].x == cellCount || snake.body[0].x == -1)
		{
			GameOver();
		}
		if (snake.body[0].y == cellCount || snake.body[0].y == -1)
		{
			GameOver();
		}
	}
	void CheckCollisionWithTail()
	{
		deque<Vector2> HeadLessBody = snake.body;
		HeadLessBody.pop_front();
		if (ElementInDeque(snake.body[0], HeadLessBody))
		{
			GameOver();
		}
	}
};

int main()
{
	InitWindow(2 * offset + cellSize * cellCount, (2 * offset + cellSize * cellCount), "Snake Game");

	SetTargetFPS(60);

	Game game = Game();

	while (!WindowShouldClose())
	{
		BeginDrawing();
		if (eventTrigerred(0.1))
		{
			game.Update();
		}

		if (IsKeyPressed(KEY_W) && game.snake.direction.y != 1)
		{
			game.snake.direction = { 0, -1 };
			game.running = true;
		}

		if (IsKeyPressed(KEY_S) && game.snake.direction.y != -1)
		{
			game.snake.direction = { 0, 1 };
			game.running = true;
		}
		if (IsKeyPressed(KEY_A) && game.snake.direction.x != 1)
		{
			game.snake.direction = { -1, 0 };
			game.running = true;
		}
		if (IsKeyPressed(KEY_D) && game.snake.direction.x != -1)
		{
			game.snake.direction = { 1, 0 };
			game.running = true;
		}

		// drawing
		ClearBackground(green);
		DrawRectangleLinesEx(Rectangle{(float)offset - 5, (float)offset-5, (float)cellSize * cellCount + 10, (float)cellSize * cellCount+10}, 5, DarkGreen);
		DrawText("Snake Game", offset - 5, 27, 40, DarkGreen);
		DrawText("Press Esc to exit", offset-5, 705, 40, DarkGreen);
		DrawText(TextFormat("Score: %i", game.score), 550, 30, 40, DarkGreen);
		game.Draw();

		EndDrawing();
	}

	CloseWindow();
	return 0;
}
