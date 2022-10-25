#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "consoleEngine.h"

#define ASTEROID_VERT_COUNT		20
#define ASTEROID_COUNT			32
#define MAX_BULLETS				64

struct space_object_t
{
	float x;
	float y;
	float dx;
	float dy;
	int32_t size;
	float angle;
} player, asteroids[ASTEROID_COUNT], bullets[MAX_BULLETS];

struct xypair_t
{
	float x;
	float y;
} asteroidmodel[ASTEROID_VERT_COUNT], asteroidtransformed[ASTEROID_VERT_COUNT], stars[256];

float vertx[3] = { 0.0f, -2.5f, +2.5f };
float verty[3] = { -5.5f, +2.5f, +2.5f };
float shipx[3], shipy[3];

uint32_t asteroid_count = 0;
uint32_t bullet_count = 0;
uint32_t score = 0;

bool is_point_inside_circle(float cx, float cy, float r, float x, float y)
{
	return sqrt((x - cx)*(x - cx) + (y - cy)*(y - cy)) < r;
}

inline void add_asteroid(float x, float y, float dx, float dy, int32_t size, float angle)
{
	if(asteroid_count < ASTEROID_COUNT)
	{
		struct space_object_t tmp = { x, y, dx, dy, size, angle };
		asteroids[asteroid_count++] = tmp;
	}
}

inline void remove_asteroid(int32_t index)
{
	if(asteroid_count > 0)
	{
		asteroids[index] = asteroids[asteroid_count - 1];
		asteroid_count--;
	}
}

inline void handle_bullet(console_t console, float elapsedTime)
{
	if(keys[SPACE].bReleased)
		if(bullet_count < MAX_BULLETS)
		{
			struct space_object_t tmp = { player.x, player.y, 100.0f * sinf(player.angle), -100.0f * cosf(player.angle), 100.0f };
			bullets[bullet_count++] = tmp;
		}

	int32_t i, j;

	for(i = 0; i < bullet_count; i++)
	{
		bullets[i].x += bullets[i].dx * elapsedTime;
		bullets[i].y += bullets[i].dy * elapsedTime;
		bullets[i].angle -= 1.0f * elapsedTime;

		ConsoleEngine_drawcircle(console, bullets[i].x, bullets[i].y, 2, DARK_MAGENTA);
	}

	for(i = 0; i < bullet_count; i++)
	{
		for(j = 0; j < asteroid_count; j++)
			if(is_point_inside_circle(asteroids[j].x, asteroids[j].y, asteroids[j].size, bullets[i].x, bullets[i].y))
			{
				bullets[i].x = -100;
				score += asteroids[j].size * 10;

				if(asteroids[j].size > 8)
				{
					float angle1 = ((float)rand() / (float)RAND_MAX) * 6.283185f;
					float angle2 = ((float)rand() / (float)RAND_MAX) * 6.283185f;

					add_asteroid(asteroids[j].x, asteroids[j].y, 10.0f * sinf(angle1), 10.0f * cosf(angle1), asteroids[j].size >> 1, 0.0f);
					add_asteroid(asteroids[j].x, asteroids[j].y, 10.0f * sinf(angle2), 10.0f * cosf(angle2), asteroids[j].size >> 1, 0.0f);
				}

				remove_asteroid(j);
			}

		if(bullets[i].x < 1 ||
			bullets[i].y < 1 ||
			bullets[i].x > ConsoleEngine_get_width(console) - 1 ||
			bullets[i].y > ConsoleEngine_get_height(console) - 1)
		{
			if(bullet_count > 1)
				bullets[i--] = bullets[--bullet_count];
			else bullet_count--;
		}
	}
}

inline void draw_player(console_t console)
{
	int32_t i, j;

	for(i = 0; i < 3; i++)
	{
		shipx[i] = vertx[i] * cosf(player.angle) - verty[i] * sinf(player.angle);
		shipy[i] = vertx[i] * sinf(player.angle) + verty[i] * cosf(player.angle);
	}

	for(i = 0; i < 3; i++)
	{
		shipx[i] = shipx[i] * player.size;
		shipy[i] = shipy[i] * player.size;
	}

	for(i = 0; i < 3; i++)
	{
		shipx[i] = shipx[i] + player.x;
		shipy[i] = shipy[i] + player.y;
	}

	for(i = 0, j = 1; i < 4; i++, j++)
		ConsoleEngine_drawline(console, shipx[i % 3], shipy[i % 3], shipx[j % 3], shipy[j % 3], GREEN);
}

inline void init_asteroid_model(void)
{
	int32_t i;

	for(i = 0; i < ASTEROID_VERT_COUNT; i++)
	{
		float noise = (float)rand() / (float)RAND_MAX * 0.4f + 0.8f;

		asteroidmodel[i].x = noise * sinf(((float)i / (float)ASTEROID_VERT_COUNT) * 6.28318f);
		asteroidmodel[i].y = noise * cosf(((float)i / (float)ASTEROID_VERT_COUNT) * 6.28318f);
	}
}

inline void draw_asteroid(console_t console, int32_t ast_index)
{
	int32_t i, j;

	for(i = 0; i < ASTEROID_VERT_COUNT; i++)
	{
		asteroidtransformed[i].x = asteroidmodel[i].x * cosf(asteroids[ast_index].angle) - asteroidmodel[i].y * sinf(asteroids[ast_index].angle);
		asteroidtransformed[i].y = asteroidmodel[i].x * sinf(asteroids[ast_index].angle) + asteroidmodel[i].y * cosf(asteroids[ast_index].angle);
	}

	for(i = 0; i < ASTEROID_VERT_COUNT; i++)
	{
		asteroidtransformed[i].x = asteroidtransformed[i].x * asteroids[ast_index].size;
		asteroidtransformed[i].y = asteroidtransformed[i].y * asteroids[ast_index].size;
	}

	for(i = 0; i < ASTEROID_VERT_COUNT; i++)
	{
		asteroidtransformed[i].x = asteroidtransformed[i].x + asteroids[ast_index].x;
		asteroidtransformed[i].y = asteroidtransformed[i].y + asteroids[ast_index].y;
	}

	for(i = 0, j = 1; i < ASTEROID_VERT_COUNT + 1; i++, j++)
		ConsoleEngine_drawline(console,
			asteroidtransformed[i % ASTEROID_VERT_COUNT].x, asteroidtransformed[i % ASTEROID_VERT_COUNT].y,
			asteroidtransformed[j % ASTEROID_VERT_COUNT].x, asteroidtransformed[j % ASTEROID_VERT_COUNT].y, YELLOW);
}

inline void init_space_bg(console_t console)
{
	int32_t i;

	for(i = 0; i < 256; i++)
	{
		stars[i].x = rand() % ConsoleEngine_get_width(console);
		stars[i].y = rand() % ConsoleEngine_get_height(console);
	}
}

inline void draw_space_bg(console_t console)
{
	int32_t i;

	for(i = 0; i < 256; i++)
		ConsoleEngine_drawpixel(console, stars[i].x, stars[i].y, WHITE);
}

int main(void)
{
	srand(time(NULL));
	console_t con = ConsoleEngine_init(ConsoleEngine_new(), 640, 400, "Console");
	char buff[128] = { 0 };

	player.x = ConsoleEngine_get_width(con) / 2;
	player.y = ConsoleEngine_get_height(con) / 2;
	player.dx = 0.0f;
	player.dy = 0.0f;
	player.size = 2;
	player.angle = 0.0f;
	init_asteroid_model();
	init_space_bg(con);

	int64_t curr_time = ConsoleEngine_get_time_ms();
	int64_t last_time = ConsoleEngine_get_time_ms();
	float timePassed = 0.0f;
	float elapsedTime;
	float fps = 0.0f;

	bool player_dead = false;
	bool player_won = false;

	int32_t i;

	add_asteroid(20.0f, 20.0f, 8.0f, -6.0f, 64, 3.0f);
	add_asteroid(100.0f, 20.0f, -5.0f, 3.0f, 64, 76.0f);

	while(true)
	{
		ConsoleEngine_clear(con, BLACK);
		ConsoleEngine_readinput(con);

		draw_space_bg(con);

		curr_time = ConsoleEngine_get_time_ms();
		elapsedTime = (curr_time - last_time) / 1000.0f;
		last_time = curr_time;
		timePassed += elapsedTime;

		if(keys[ESCAPE].bPressed) break;

		if(keys[UPARROW].bHeld)
		{
			player.dx += sinf(player.angle) * 20.0f * elapsedTime;
			player.dy += -cosf(player.angle) * 20.0f * elapsedTime;
		}

		if(keys[LEFTARROW].bHeld)
			player.angle -= 5.0f * elapsedTime;

		if(keys[RIGHTARROW].bHeld)
			player.angle += 5.0f * elapsedTime;

		player.x += player.dx * elapsedTime;
		player.y += player.dy * elapsedTime;
		ConsoleEngine_wrap_coordinates(con, player.x, player.y, &player.x, &player.y);

		for(i = 0; i < asteroid_count; i++)
		{
			asteroids[i].x += asteroids[i].dx * elapsedTime;
			asteroids[i].y += asteroids[i].dy * elapsedTime;
			asteroids[i].angle += 0.5f * elapsedTime;

			ConsoleEngine_wrap_coordinates(con, asteroids[i].x, asteroids[i].y, &asteroids[i].x, &asteroids[i].y);
			draw_asteroid(con, i);

			if(is_point_inside_circle(asteroids[i].x, asteroids[i].y, asteroids[i].size, player.x, player.y))
				player_dead = true;
		}

		if(player_dead == true) break;

		if(asteroid_count == 0)
		{
			player_won = true;
			break;
		}

		draw_player(con);
		handle_bullet(con, elapsedTime);

		if(timePassed > 0.15f)
		{
			fps = (1.0f / elapsedTime);
			timePassed = 0.0f;
		}

		sprintf(buff, "FPS: %3.2f", fps);
		ConsoleEngine_printf(con, 2, 2, 1, WHITE, "Score: %u", score);
		ConsoleEngine_set_title(con, buff);
		ConsoleEngine_update(con);
	}

	while(true)
	{
		ConsoleEngine_clear(con, BLACK);
		ConsoleEngine_readinput(con);

		if(keys[ENTER].bPressed) break;

		if(player_won)
		{
			ConsoleEngine_printf(con, 2, 2, 1, BLUE, "YOU WON\n\nPress ENTER to close window");
			ConsoleEngine_set_title(con, "YOU WIN");
		}

		if(player_dead)
		{
			ConsoleEngine_printf(con, 2, 2, 1, RED, "YOU DIED\n\nPress ENTER to close window");
			ConsoleEngine_set_title(con, "YOU LOSE");
		}

		if(!player_dead && !player_won)
		{
			ConsoleEngine_printf(con, 2, 2, 1, GREEN, "GAME EXITED\n\nPress ENTER to close window");
			ConsoleEngine_set_title(con, "GAME CLOSED");
		}

		ConsoleEngine_update(con);
	}

	ConsoleEngine_free(con);
	return 0;
}
