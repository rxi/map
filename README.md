# map
A type-safe generic hashmap implementation for C.
Forked from [rxi/map](https://github.com/rxi/map), it adds the possibility to
reserve an arbitrary (power of 2) number of buckets for performance
improvements.

## Installation 
The [map.c](src/map.c?raw=1) and [map.h](src/map.h?raw=1) files can be dropped
into an existing C project and compiled along with it.


## Usage
Before using a map, it should first be initialised using the `map_init_reserve()`
macro.
The second argument is an integer with the number of buckets to be pre-allocated.
If is not a power of 2 or 0, no pre-allocation is done. It can improve the speed but,
on the other hand, could allocate useless space.
```c
map_int_t m;
unsigned initial_nbuckets = 128;
map_init_reserve(&m, initial_nbuckets);
```

It pre-allocation is not needed, the macro `map_init()` can be used as well.
```c
map_int_t m;
map_init(&m);
```

Values can added to a map using the `map_set()` macro.
```c
map_set(&m, "testkey", 123);
```

To retrieve a value from a map, the `map_get()` macro can be used.
`map_get()` will return a pointer to the key's value, or `NULL` if no mapping
for that key exists.
```c
int *val = map_get(&m, "testkey");
if (val) {
  printf("value: %d\n", *val);
} else {
  printf("value not found\n");
}
```

When you are done with a map the `map_deinit()` macro should be called on
it. This will free any memory the map allocated during use.
```c
map_deinit(&m);
```


## Types
To define a new map type the `map_t()` macro should be used:
```c
/* Creates the type uint_map_t for storing unsigned ints */
typedef map_t(unsigned int) uint_map_t;
```

## Functions
All map functions are macro functions. The parameter `m` in each function
should be a pointer to the map struct which the operation is to be performed
on. The `key` parameter should always be a string value.

### map\_t(T)
Creates a map struct for containing values of type `T`.
```c
/* Typedefs the struct `fp_map_t` as a container for type FILE* */
typedef map_t(FILE*) fp_map_t;
```

### map\_init(m, initial_nbuckets)
Initialises the map, this must be called before the map can be used. The parameter
`initial_nbuckets` sets the number of buckets to be pre-allocated.

### map\_deinit(m)
Deinitialises the map, freeing the memory the map allocated during use;
this should be called when we're finished with a map.

### map\_get(m, key)
Returns a pointer to the value of the given `key`. If no mapping for the `key`
exists then `NULL` will be returned.

### map\_set(m, key, value)
Sets the given `key` to the given `value`. Returns `0` on success, otherwise
`-1` is returned and the map remains unchanged.

### map\_remove(m, key)
Removes the mapping of the given `key` from the map. If the `key` does not
exist in the map then the function has no effect.

### map\_iter(m)
Returns a `map_iter_t` which can be used with `map_next()` to iterate all the
keys in the map.

### map\_next(m, iter)
Uses the `map_iter_t` returned by `map_iter()` to iterate all the keys in the
map. `map_next()` returns a key with each call and returns `NULL` when there
are no more keys.
```c
const char *key;
map_iter_t iter = map_iter(&m);

while ((key = map_next(&m, &iter))) {
  printf("%s -> %d", key, *map_get(&m, key));
}
```

## License
This library is free software; you can redistribute it and/or modify it under
the terms of the MIT license. See [LICENSE](LICENSE) for details.
