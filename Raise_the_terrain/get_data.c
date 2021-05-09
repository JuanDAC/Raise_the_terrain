#include "window.h"

void info_error(char *message_error)
{
	size_t bytes_readed = 0;
	bytes_readed = write(2, message_error, strlen(message_error));
	if (bytes_readed == EOF)
		exit(EOF);
}

char *read_file(char *file_direction)
{
	size_t file_pointer = 0;
	char lines[BUFSIZ];
	char *content = NULL;
	size_t line_size = 0;

	file_pointer = open(file_direction, O_RDONLY | O_CREAT);
	if (file_pointer == -1)
		return (NULL);

	line_size = read(file_pointer, lines, BUFSIZ);
	if (line_size == EOF)
		return (NULL);
	lines[line_size] = '\0';
	close(file_pointer);
	content = malloc(sizeof(char) * strlen(lines));
	if (!content)
		return (NULL);
	strcpy(content, lines);
	return (content);
}

bounds_t *get_bounds(char *data_raw)
{
	int i;
	bounds_t *bounds;

	bounds = malloc(sizeof(bounds_t));
	if (!bounds)
		return (NULL);
	bounds->deep = 0;
	bounds->width = 0;
	for (i = 0; data_raw[i] != '\0'; i++)
	{
		bounds->width += !bounds->deep && data_raw[i] == ' ';
		bounds->deep += data_raw[i] == '\n';
	}
	bounds->width += 1;
	bounds->deep += 1;
	bounds->angle = 0;
	return (bounds);
}


axis_t **get_directions(char *data_raw, bounds_t *bounds)
{
	int i, max = 0;
	axis_t *local_directions[BUFSIZ];
	axis_t **directions;
	char *current = data_raw;

	max = (bounds->deep * bounds->width);
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
			local_directions[i]->z_raw = atoi(current);
			local_directions[i]->z = 0;
			local_directions[i]->x = 0;
			local_directions[i]->y = 0;
			local_directions[i]->x_next = 0;
			local_directions[i]->y_next = 0;
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

	return (directions);
}


DWORD get_windows_bound(void)
{
	DWORD width = 0;
	DWORD height = 0;

	width = GetSystemMetrics(SM_CXSCREEN) - GetSystemMetrics(SM_CXSIZE);
	height = GetSystemMetrics(SM_CYSCREEN) - GetSystemMetrics(SM_CYSIZE) 
		- GetSystemMetrics(SM_CYMENU) - GetSystemMetrics(SM_CYCAPTION);

	return ((width < height ? width : height) - 10);
}
