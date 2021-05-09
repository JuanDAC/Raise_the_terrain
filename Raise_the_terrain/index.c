#include "window.h"

int main(int argc, char *argv[])
{
	SDL_Instance instance;
	int first = 1, i;
	char *data_raw = NULL;
	bounds_t *bounds = NULL;
	axis_t **directions = NULL;

	if (argc != 2)
	{
		info_error("WARNING: Your need spesifi a file");
		return (EXIT_FAILURE);
	}

	data_raw = read_file(argv[first]);
	if (!data_raw)
		return (EXIT_FAILURE);
	bounds = get_bounds(data_raw);
	if (!bounds)
	{
		free(data_raw);
		return (EXIT_FAILURE);
	}
	directions = get_directions(data_raw, bounds);
	if (!directions)
	{
		free(data_raw);
		free(bounds);
		return (EXIT_FAILURE);
	}

	if (init_instance(&instance))
		return (1);

	init(directions, bounds);
	while (true)
	{
		SDL_SetRenderDrawColor(instance.renderer, 0, 0, 0, 0);
		SDL_RenderClear(instance.renderer);
		if (poll_events(directions, bounds))
			break;
		update(directions, bounds);
		draw_stuff(&instance, directions, bounds);
		SDL_RenderPresent(instance.renderer);
	}

	SDL_DestroyRenderer(instance.renderer);
	SDL_DestroyWindow(instance.window);
	SDL_Quit();
	return (0);
}

int poll_events(axis_t **directions, bounds_t * bounds)
{
	SDL_Event event;
	SDL_KeyboardEvent key;
	int i;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			return (1);
		case SDL_KEYDOWN:
			key = event.key;
			if (key.keysym.scancode == 0x29)
				return (1);
			if (key.keysym.scancode == SDL_SCANCODE_P)
				for (i = 0; i < bounds->width * bounds->deep; i++)
					directions[i]->z = 0;
			break;
		case SDL_KEYUP:
			key = event.key;
			break;
		}
	}
	return (0);
}

// numeros aleatoriso 
#define WX(X, Y, WB, INC) (((INC) * (X) - (INC) * (Y)) + ((WB) / 2))
#define WY(X, Y, Z, INC) (((1 - (INC)) * (X)) + ((1 - (INC)) * (Y)) - (Z)) + 200	

// numeros aleatoriso 
#define get_pos_angle_x(X, Y, ZS, A) (((X) - (ZS)) * cos(A) - ((Y) - (ZS)) * sin(A) + (ZS))
#define get_pos_angle_y(X, Y, ZS, A) (((X) - (ZS)) * sin(A) + ((Y) - (ZS)) * cos(A) + (ZS))

int init(axis_t **directions, bounds_t * bounds)
{
	int i, j, k;

	DWORD windows_bound = get_windows_bound();
	DWORD tile_row = windows_bound / (bounds->width + 3);
	DWORD tile_column = windows_bound / (bounds->deep + 3);
	int row_displacement = tile_row * 3;
	int column_displacement = tile_column * 3;
	windows_bound += row_displacement;

	k = 0;
	for (i = 0; i < bounds->width; i++)
		for (j = 0; j < bounds->deep; j++, k++)
		{
			directions[k]->x_next = tile_row * (i + 1) + row_displacement;
			directions[k]->y_next = tile_column * (j + 1) + column_displacement;
			directions[k]->x = tile_row * i + row_displacement;
			directions[k]->y = tile_column * j + column_displacement;
		}
}

int update(axis_t **directions, bounds_t * bounds)
{
	int i, j, k;

	float change = 0.5;
	DWORD windows_bound = get_windows_bound();
	DWORD tile_row = windows_bound / (bounds->width + 3);
	DWORD tile_column = windows_bound / (bounds->deep + 3);
	int row_displacement = tile_row * 3;
	int column_displacement = tile_column * 3;
	windows_bound += row_displacement;
	float radio = windows_bound / 2, X, Y, XN, YN;

	bounds->angle = 0;
	float a = bounds->angle * M_PI / 180;
	k = 0;
	for (i = 0; i < bounds->width; i++)
		for (j = 0; j < bounds->deep; j++, k++)
		{
			if (directions[k]->z < directions[k]->z_raw - (change * 2) || directions[k]->z > directions[k]->z_raw + (change * 2))
				directions[k]->z += directions[k]->z_raw < 0 ? -change: change;
			else 
				directions[k]->z = directions[k]->z_raw;

			X = directions[k]->x;
			Y = directions[k]->y;
			XN = directions[k]->x_next;
			YN = directions[k]->y_next;
			
			directions[k]->x = get_pos_angle_x(X, Y, radio, a);
			directions[k]->y = get_pos_angle_y(X, Y, radio, a);
			directions[k]->x_next = get_pos_angle_x(XN, YN, radio, a);
			directions[k]->y_next = get_pos_angle_y(XN, YN, radio, a);
		}
}

void draw_stuff(SDL_Instance *instance, axis_t **directions, bounds_t *bounds)
{
	DWORD windows_bound, row, column;
	int i, j, k;
	float inc = 0.75, red, blue, green;
	time_t t;
	srand((unsigned)time(&t));

	red = (rand() % 55) + 200;
	blue = rand() % 210;
	green = rand() % 100 + 154;

	windows_bound = get_windows_bound();

	SDL_RenderClear(instance->renderer); /*    r     g              b          a          */
	SDL_SetRenderDrawColor(instance->renderer, red, green, blue, SDL_ALPHA_OPAQUE);
	k = 0;
	for (i = 0; i < bounds->width; i++)
		for (j = 0; j < bounds->deep; j++, k++)
		{
			if (i < bounds->width - 1)
				SDL_RenderDrawLine(instance->renderer,
					WX(directions[k]->x, directions[k]->y, windows_bound, inc),
					WY(directions[k]->x, directions[k]->y, directions[k]->z, inc),
					WX(directions[k]->x_next, directions[k]->y, windows_bound, inc),
					WY(directions[k]->x_next, directions[k]->y, directions[k + bounds->deep]->z, inc)
				);
			if (j < bounds->deep - 1)
				SDL_RenderDrawLine(instance->renderer,
					WX(directions[k]->x, directions[k]->y, windows_bound, inc),
					WY(directions[k]->x, directions[k]->y, directions[k]->z, inc),
					WX(directions[k]->x, directions[k]->y_next, windows_bound, inc),
					WY(directions[k]->x, directions[k]->y_next, directions[k + 1]->z , inc)
				);
		}
}
/**
 * init_instance - entry point
 * Return: Alwais void
 */
int init_instance(SDL_Instance *instance)
{
	DWORD *windows_bound;

	if (SDL_Init(SDL_INIT_VIDEO) != 0)
		return (1);
	windows_bound = get_windows_bound();

	instance->window = SDL_CreateWindow(
		"Raising the ground",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		windows_bound, /*W*/
		windows_bound, /*H*/
		SDL_WINDOW_SHOWN
	);

	if (instance->window == NULL)
	{
		SDL_Quit();
		return (1);
	}

	instance->renderer = SDL_CreateRenderer(
		instance->window, -1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);

	if (instance->renderer == NULL)
	{
		SDL_DestroyWindow(instance->window);
		SDL_Quit();
		return (1);
	}

	return (EXIT_SUCCESS);
}

