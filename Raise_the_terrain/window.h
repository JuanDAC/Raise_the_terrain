#pragma warning(disable : 4013)
#pragma warning(disable : 4996)
#define _CRT_NONSTDC_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS_GLOBALS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<stdio.h>
#include<fcntl.h>
#include<errno.h>
#include <SDL.h>
#include <windows.h>
#include <math.h>

#define STDIN_FILENO    0       /* Standard input.  */
#define STDOUT_FILENO   1       /* Standard output.  */
#define STDERR_FILENO   2       /* Standard error output.  */

typedef unsigned char bool;

#define false (0)
#define true (!false)

typedef struct _Instance
{
	SDL_Window *window;
	SDL_Renderer *renderer;
} SDL_Instance;

int poll_events(void);
int init_instance(SDL_Instance *instance);
void draw_stuff(SDL_Instance *instance);

#define SCREEN_WIDTH (400)
#define SCREEN_HEIGHT (400)
#define SIZE_READ_LINES (1024)


typedef struct axis_s
{
	float x;
	float y;
	float z;
	float z_raw;
	float x_next;
	float y_next;
} axis_t;


typedef struct bounds_s
{
	int deep;
	float angle;
	int width;
} bounds_t;
bounds_t *get_bounds(char *data_raw);


char *read_file(char *file_direction);
void info_error(char *error_message);
axis_t **get_directions(char *data_raw, bounds_t *bounds);
void info_error(char *message_error);
char *read_file(char *file_direction);
bounds_t *get_bounds(char *data_raw);
axis_t **get_directions(char *data_raw, bounds_t *bounds);
DWORD get_windows_bound(void);
int init(axis_t **directions, bounds_t * bounds);
