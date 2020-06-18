/**
 * Copyright (c) 2020 kpishere
 *
 * with credits to :  Copyright (c) 2014 rxi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

#ifndef MAPSK_H
#define MAPSK_H

#include <string.h>
#include "map.h"

#define map_get_sk(m, key, ksize)\
  ( (m)->ref = map_get_sk_(&(m)->base, (void*)key, ksize) )


#define map_set_sk(m, key, ksize, value)\
  ( (m)->tmp = (value),\
    map_set_sk_(&(m)->base, (void*)key, ksize, &(m)->tmp, sizeof((m)->tmp)) )


#define map_remove_sk(m, key, ksize)\
  map_remove_sk_(&(m)->base, key, ksize)


void *map_get_sk_(map_base_t *m, const void *key, int ksize);
int map_set_sk_(map_base_t *m, const void *key, int ksize, void *value, int vsize);
void map_remove_sk_(map_base_t *m, const void *key, int ksize);

#endif 
