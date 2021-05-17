#include "window.h"
/**
 * info_error - handler error 
 *
 * @message_error: message error
 *
*/
void info_error(char *message_error)
{
	size_t bytes_readed = 0;
	bytes_readed = write(2, message_error, strlen(message_error));
	if (bytes_readed == EOF)
		exit(EOF);
}

/**
 * is_map - valid if it's a valid map the content of the file 
 *
 * @data_raw: data without processing
 *
 * Return: true if it's a valid map else return false
*/
bool is_map(char *data_raw)
{
	if (data_raw == NULL)
		return (false);
	if (*data_raw == '\0')
		return (true);
	if (*data_raw == '\n' || *data_raw == ' ' || *data_raw == '-')
		return (is_map(data_raw + 1));
	if (*data_raw >= '0' && *data_raw <= '9')
		return (is_map(data_raw + 1));
	return (false);
}

/**
 * read_file - read content of file
 *
 * @file_direction: direction of the file to read
 *
 * Return: data without processing
*/
char *read_file(char *file_direction)
{
	size_t file_pointer = 0;
	char line[BUFSIZ];
	char *content = NULL;
	size_t line_size = 0;

	file_pointer = open(file_direction, O_RDONLY | O_CREAT);
	if (file_pointer == -1)
		return (NULL);

	line_size = read(file_pointer, line, BUFSIZ);
	if (line_size == EOF)
		return (NULL);
	line[line_size] = '\0';
	close(file_pointer);
	if (!is_map(line))
		return (NULL);
	content = malloc(sizeof(char) * strlen(line));
	if (!content)
		return (NULL);
	strcpy(content, line);
	return (content);
}

/**
 * get_bounds - read the bounds and storage in the bounds struct
 *
 * @data_raw: data without processing
 *
 * Return: structure that contain the limits if gid
*/
bounds_t *get_bounds(char *data_raw)
{
	int i;
	bounds_t *bounds;

	bounds = malloc(sizeof(bounds_t));
	if (!bounds)
		return (NULL);
	bounds->axis_y = 0;
	bounds->axis_x = 0;
	for (i = 0; data_raw[i] != '\0'; i++)
	{
		bounds->axis_x += !bounds->axis_y && data_raw[i] == ' ';
		bounds->axis_y += data_raw[i] == '\n'; 
	}
	bounds->axis_x += 1;
	bounds->axis_y += 1;
	bounds->angle = 0;

	get_windows_bound(&bounds->width, &bounds->height, &bounds->min_bound);
	bounds->max_bound = bounds->width == bounds->min_bound ? bounds->height : bounds->width;
	return (bounds);
}

/**
 * get_directions - read the directions and return the struct storage
 *
 * @data_raw: data without processing
 * @bounds: limits of grid 
 *
 * Return: array of strctures storege the directions
*/
axis_t **get_directions(char *data_raw, bounds_t *bounds)
{
	int i, lenght = 0;
	axis_t *local_directions[BUFSIZ];
	axis_t **directions;
	char *current = data_raw;

	lenght = (int)(bounds->axis_x * bounds->axis_y);
	for (i = 0; current; i++)
	{
		current = strtok(i ? NULL : data_raw, " \n");
		if (current != NULL)
		{
			local_directions[i] = malloc(sizeof(axis_t));
			if (!local_directions[i])
			{
				i-= 1;
				for (i--; 0 < i; i--)
					free(local_directions[i]);
				return (NULL);
			}
			local_directions[i]->z_raw = (float)atoi(current);
			local_directions[i]->z = 0;
			local_directions[i]->x = 0;
			local_directions[i]->y = 0;
		}
		else
		{ 
			local_directions[i] = NULL;
		}
	}
	directions = calloc(i, sizeof(axis_t *));
	if (!directions)
		return (NULL);

	for (i = 0; local_directions[i]; i++)
		directions[i] = local_directions[i];
	directions[i] = NULL;

	return (directions);
}


/**
 * get_windows_bound - get the windows screem bounds
 *
 * @width: width size of screen
 * @height: height size of screen
 * @min_size: min_size size of screen
 *
*/
void get_windows_bound(DWORD *width, DWORD *height, DWORD *min_size)
{
	DWORD local_width = 0;
	DWORD local_height = 0;

	local_width = GetSystemMetrics(SM_CXSCREEN);
	local_height = GetSystemMetrics(SM_CYSCREEN) - GetSystemMetrics(SM_CYSIZE)
		- GetSystemMetrics(SM_CYMENU) - GetSystemMetrics(SM_CYCAPTION);
	if (width)
		*width = local_width;
	if (height)
		*height = local_height;
	if (min_size)
		*min_size = ((local_width < local_height ? local_width : local_height));
}
