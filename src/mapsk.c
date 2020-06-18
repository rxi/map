/** 
 * Copyright (c) 2020 kpishere
 *
 * with credits to :  Copyright (c) 2014 rxi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

#include <stdlib.h>
#include <string.h>
#include "mapsk.h"

struct map_node_t {
  unsigned hash;
  void *value;
  map_node_t *next;
  /* char key[]; */
  /* char value[]; */
};

static unsigned map_hash_sk(const void *key, int ksize) {
  unsigned hash = 5381;
  for(int i=0; i<ksize;i++) {
    uint8_t v = ((uint8_t *)(key))[i];
    hash = ((hash << 5) + hash) ^ (v!=0?v:i);
  }
  return hash;
}

static map_node_t *map_newnode_sk(const void *key, int ksize, void *value, int vsize) {
  map_node_t *node;
  int voffset = ksize + ((sizeof(void*) - ksize) % sizeof(void*));
  node = malloc(sizeof(*node) + voffset + vsize);
  if (!node) return NULL;
  memcpy(node + 1, key, ksize);
  node->hash = map_hash_sk(key, ksize);
  node->value = ((char*) (node + 1)) + voffset;
  memcpy(node->value, value, vsize);
  return node;
}

static int map_bucketidx(map_base_t *m, unsigned hash) {
  /* If the implementation is changed to allow a non-power-of-2 bucket count,
   * the line below should be changed to use mod instead of AND */
  return hash & (m->nbuckets - 1);
}

static void map_addnode(map_base_t *m, map_node_t *node) {
  int n = map_bucketidx(m, node->hash);
  node->next = m->buckets[n];
  m->buckets[n] = node;
}

static int map_resize(map_base_t *m, int nbuckets) {
  map_node_t *nodes, *node, *next;
  map_node_t **buckets;
  int i; 
  /* Chain all nodes together */
  nodes = NULL;
  i = m->nbuckets;
  while (i--) {
    node = (m->buckets)[i];
    while (node) {
      next = node->next;
      node->next = nodes;
      nodes = node;
      node = next;
    }
  }
  /* Reset buckets */
  buckets = realloc(m->buckets, sizeof(*m->buckets) * nbuckets);
  if (buckets != NULL) {
    m->buckets = buckets;
    m->nbuckets = nbuckets;
  }
  if (m->buckets) {
    memset(m->buckets, 0, sizeof(*m->buckets) * m->nbuckets);
    /* Re-add nodes to buckets */
    node = nodes;
    while (node) {
      next = node->next;
      map_addnode(m, node);
      node = next;
    }
  }
  /* Return error code if realloc() failed */
  return (buckets == NULL) ? -1 : 0;
}

static map_node_t **map_getref_sk(map_base_t *m, const void *key, int ksize) {
  unsigned hash = map_hash_sk(key, ksize);
  map_node_t **next;
  if (m->nbuckets > 0) {
    next = &m->buckets[map_bucketidx(m, hash)];
    while (*next) {
        if ((*next)->hash == hash && !memcmp((void*) (*next + 1), key, ksize)) {
        return next;
      }
      next = &(*next)->next;
    }
  }
  return NULL;
}

void *map_get_sk_(map_base_t *m, const void *key, int ksize) {
  map_node_t **next = map_getref_sk(m, key, ksize);
  return next ? (*next)->value : NULL;
}

int map_set_sk_(map_base_t *m, const void *key, int ksize, void *value, int vsize) {
  int n, err;
  map_node_t **next, *node;
  /* Find & replace existing node */
  next = map_getref_sk(m, key, ksize);
  if (next) {
    memcpy((*next)->value, value, vsize);
    return 0;
  }
  /* Add new node */
  node = map_newnode_sk(key, ksize, value, vsize);
  if (node == NULL) goto fail;
  if (m->nnodes >= m->nbuckets) {
    n = (m->nbuckets > 0) ? (m->nbuckets << 1) : 1;
    err = map_resize(m, n);
    if (err) goto fail;
  }
  map_addnode(m, node);
  m->nnodes++;
  return 0;
  fail:
  if (node) free(node);
  return -1;
}

void map_remove_sk_(map_base_t *m, const void *key, int ksize) {
  map_node_t *node;
  map_node_t **next = map_getref_sk(m, key, ksize);
  if (next) {
    node = *next;
    *next = (*next)->next;
    free(node);
    m->nnodes--;
  }
}

