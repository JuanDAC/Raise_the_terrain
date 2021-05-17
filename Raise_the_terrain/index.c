#include "window.h"

/**
 * main - Entry point
 *
 * @argc: conunter of argimens of the program
 * @argv: in first position the direction of file
*/
int main(int argc, char *argv[])
{
	SDL_Instance instance;
	int first = 1;
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

/**
 * poll_events - Entry point
 *
 * @argc: conunter of argimens of the program
 * @argv: in first position the direction of file
*/
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
				for (i = 0; i < bounds->axis_x * bounds->axis_y; i++)
					directions[i]->z = 0;
			break;
		case SDL_KEYUP:
			key = event.key;
			break;
		}
	}
	return (0);
}

/**
 * init - update data in frame
 *
 * @directions: Array of directions 
 * @bounds: limits of grid 
 * 
*/
void init(axis_t **directions, bounds_t * bounds)
{
	int i, j, k;

	DWORD tile_row = (DWORD)(bounds->max_bound / (bounds->axis_x + 3));
	DWORD tile_column = (DWORD)(bounds->max_bound / (bounds->axis_y + 3));
	int row_displacement = tile_row * 3;
	int column_displacement = tile_column * 3;

	k = 0;
	for (i = 0; i < bounds->axis_x; i++)
		for (j = 0; j < bounds->axis_y; j++, k++)
		{
			directions[k]->x = (float)tile_row * i + row_displacement;
			directions[k]->y = (float)tile_column * j + column_displacement;
		}
}

/**
 * update - update data in frame
 *
 * @directions: Array of directions 
 * @bounds: limits of grid 
 * 
*/
void update(axis_t **directions, bounds_t *bounds)
{
	int k;
	float change = 0.5;
	float X, Y;

	bounds->angle = 0;
	float a = (float)(bounds->angle * M_PI / 180);
	for (k = 0; k < bounds->axis_x * bounds->axis_y; k++)
	{
		if (directions[k]->z < directions[k]->z_raw - (change * 2) || directions[k]->z > directions[k]->z_raw + (change * 2))
			directions[k]->z += directions[k]->z_raw < 0 ? -change : change;
		else 
			directions[k]->z = directions[k]->z_raw;

		X = directions[k]->x;
		Y = directions[k]->y;
		
		directions[k]->x = (float)get_pos_angle_x(X, Y, bounds->max_bound / 2, a);
		directions[k]->y = (float)get_pos_angle_y(X, Y, bounds->max_bound / 2, a);
	}
}

/**
 * color_handler - Color handler  
 *  
 * @instance: - instance of render object
 * 
*/
void color_handler(SDL_Instance *instance)
{
	Uint8 red, blue, green;
	time_t t;
	srand((unsigned)time(&t));

	red = (rand() % 55) + 200;
	blue = rand() % 210;
	green = rand() % 100 + 154;

	SDL_SetRenderDrawColor(instance->renderer, red, green, blue, SDL_ALPHA_OPAQUE);
}

/**
 * color_handler - Color handler  
 *  
 * @instance: - instance of render object
 * @bounds: limits of grid 
 * @directions: storage of directions
 * 
*/
void draw_stuff(SDL_Instance *instance, axis_t **directions, bounds_t *bounds)
{
	int i, j, k;
	float inc = 0.75;

	SDL_RenderClear(instance->renderer); 
	color_handler(instance);

	k = 0;
	for (i = 0; i < bounds->axis_x; i++)
		for (j = 0; j < bounds->axis_y; j++, k++)
		{
			if (i < bounds->axis_x - 1)
				SDL_RenderDrawLine(instance->renderer,
					(int)WX(directions[k]->x, directions[k]->y, bounds->width, inc),
					(int)WY(directions[k]->x, directions[k]->y, directions[k]->z, inc),
					(int)WX(directions[k + (int)bounds->axis_x]->x, directions[k]->y, bounds->width, inc),
					(int)WY(directions[k + (int)bounds->axis_x]->x, directions[k]->y, directions[k + (int)bounds->axis_x]->z, inc)
				);
			if (j < bounds->axis_y - 1)
				SDL_RenderDrawLine(instance->renderer,
					(int)WX(directions[k]->x, directions[k]->y, bounds->width, inc),
					(int)WY(directions[k]->x, directions[k]->y, directions[k]->z, inc),
					(int)WX(directions[k]->x, directions[k + 1]->y, bounds->width, inc),
					(int)WY(directions[k]->x, directions[k + 1]->y, directions[k + 1]->z , inc)
				);
	}
}
/**
 * init_instance - init the instance of window
 *
 * @instance: - instance of render object
 * Return: integer to validate error 
 */
int init_instance(SDL_Instance *instance)
{
	DWORD windows_bound, width, height;

	if (SDL_Init(SDL_INIT_VIDEO) != 0)
		return (EXIT_FAILURE);
	get_windows_bound(&width, &height, &windows_bound);

	instance->window = SDL_CreateWindow(
		"Raising the ground",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		width,
		height,
		SDL_WINDOW_SHOWN
		| SDL_WINDOW_INPUT_FOCUS 
		| SDL_WINDOW_MAXIMIZED
	);

	if (!instance->window)
	{
		SDL_Quit();
		return (EXIT_FAILURE);
	}

	instance->renderer = SDL_CreateRenderer(
		instance->window, -1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);

	if (instance->renderer == NULL)
	{
		SDL_DestroyWindow(instance->window);
		SDL_Quit();
		return (EXIT_FAILURE);
	}

	return (EXIT_SUCCESS);
}

