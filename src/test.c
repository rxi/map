/** 
 * Copyright (c) 2019-2022 Giovanni Cerretani
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

#include "map.h"
#include <stdio.h>
#include <stdlib.h>

typedef map_t(int) map_int_t;

int main() {
	int ret = 0;

	map_int_t *map = malloc(sizeof(*map));
	if (map == NULL) {
		ret = 1;
		goto quit;
	}

	const int value = 12;
	const char key[] = "mykey";

	map_init_reserve(map, 16);
	map_set(map, key, value);
	int *value_get = map_get(map, key);

	if (value_get == NULL) {
		ret = 2;
		goto quit;
	}

	if (*value_get != value) {
		ret = 3;
		goto quit;
	}

quit:
	map_deinit(map);
	free(map);
	printf("%d\n", ret);

	return ret;
}
