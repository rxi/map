#include "map.h"
#include <stdio.h>
#include <stdlib.h>

typedef map_t(int) map_int_t;

int main() {
	map_int_t *map = malloc(sizeof(*map));
	if (map == NULL)
		return -1;
	
	const int value = 12;
	const char key[] = "mykey";
	
	map_init(map, 16);
	map_set(map, key, value);
	int *value_get = map_get(map, key); 
	
	if (value_get != NULL)
		printf("%i\n", *value_get);
	else
		printf("Error\n");	
	
	free(map);
	return 0;
}