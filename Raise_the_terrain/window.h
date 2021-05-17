#pragma warning(disable : 4013)
#pragma warning(disable : 4996)
#define _CRT_NONSTDC_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS_GLOBALS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <SDL.h>
#include <windows.h>
#include <math.h>

#define STDIN_FILENO    0       /* Standard input.  */
#define STDOUT_FILENO   1       /* Standard output.  */
#define STDERR_FILENO   2       /* Standard error output.  */

typedef unsigned char bool;

#define false (0)
#define true (!false)

/**
 * @window: windows element
 * @renderer: renderer element
*/
typedef struct _Instance
{
	SDL_Window *window;
	SDL_Renderer *renderer;
} SDL_Instance;

#define WX(X, Y, WB, INC) (((INC) * (Y) - (INC) * (X)) + ((WB / 2)))
#define WY(X, Y, Z, INC) (((1 - (INC)) * (X)) + ((1 - (INC)) * (Y)) - (Z))

#define get_pos_angle_x(X, Y, ZS, A) (((X) - (ZS)) * cos(A) - ((Y) - (ZS)) * sin(A) + (ZS))
#define get_pos_angle_y(X, Y, ZS, A) (((X) - (ZS)) * sin(A) + ((Y) - (ZS)) * cos(A) + (ZS))

/**
 * @x: axis x
 * @y: axis y
 * @z: axis z
 * @z: z with processing
 * @z_raw: z without processing
*/
typedef struct axis_s
{
	float x;
	float y;
	float z;
	float z_raw;
} axis_t;

/**
 * @angle: angle
 * @axis_x: axis x
 * @axis_y: axis y
 * @width: width x
 * @height: height y
 * @min_bound: min_bound
 * @max_bound: max_bound
*/
typedef struct bounds_s
{
	float angle;
	float axis_x;
	float axis_y;
	DWORD width;
	DWORD height;
	DWORD min_bound;
	DWORD max_bound;
} bounds_t;
bounds_t *get_bounds(char *data_raw);

int poll_events(axis_t **directions, bounds_t * bounds);
int init_instance(SDL_Instance *instance);
void draw_stuff(SDL_Instance *instance, axis_t **directions, bounds_t *bounds);
char *read_file(char *file_direction);
void info_error(char *error_message);
axis_t **get_directions(char *data_raw, bounds_t *bounds);
void info_error(char *message_error);
char *read_file(char *file_direction);
bounds_t *get_bounds(char *data_raw);
axis_t **get_directions(char *data_raw, bounds_t *bounds);
void get_windows_bound(DWORD *width, DWORD *height, DWORD *minSize);
void init(axis_t **directions, bounds_t * bounds);
void update(axis_t **directions, bounds_t * bounds);
