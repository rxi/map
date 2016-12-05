#include <map.h>
#include <stdio.h>

int main(void)
{
	map_int_t m;
	map_init(&m);
    map_set(&m, "testkey", 123);
	int *val = (int*)(map_get(&m, "testkey"));
	if (val) {
	  printf("value: %d\n", *val);
	} else {
	  printf("value not found\n");
	}
	return 0;
}
