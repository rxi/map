/** 
 * Copyright (c) 2014 rxi
 * Copyright (c) 2019 gcerretani
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "map.h"

struct map_node_t {
	unsigned int hash;
	void *value;
	char *key;
	map_node_t *next;
};


static unsigned int map_hash(const char *str) {
	unsigned int hash = 5381;
	while (*str) {
		hash = ((hash << 5) + hash) ^ *str++;
	}
	return hash;
}


static map_node_t *map_newnode(const char *key, const void *value, int vsize) {
	map_node_t *node = malloc(sizeof(*node));
	if (!node)
		return NULL;
	node->hash = map_hash(key);
	node->key = strdup(key);
	if (node->key == NULL) {
		free(node);
		return NULL;
	}
	node->value = malloc(vsize);
	if (node->value == NULL) {
		free(node->key);
		free(node);
		return NULL;
	}
	memcpy(node->value, value, vsize);
	node->next = NULL;
	return node;
}


static void map_deletenode(map_node_t *node) {
	if (node == NULL)
		return;
	free(node->value);
	free(node->key);
	free(node);
}


static int map_bucketidx(map_base_t *m, unsigned int hash) {
	/* If the implementation is changed to allow a non-power-of-2 bucket count,
	 * the line below should be changed to use mod instead of AND */
	return hash & (m->nbuckets - 1);
}


static void map_addnode(map_base_t *m, map_node_t *node) {
	int n = map_bucketidx(m, node->hash);
	node->next = m->buckets[n];
	m->buckets[n] = node;
}


static int map_resize(map_base_t *m, unsigned int nbuckets) {
	map_node_t *nodes, *node, *next;
	map_node_t **buckets;
	/* Chain all nodes together */
	nodes = NULL;
	unsigned int i = m->nbuckets;
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


static map_node_t **map_getref(map_base_t *m, const char *key) {
	unsigned int hash = map_hash(key);
	if (m->nbuckets > 0) {
		map_node_t **next = &m->buckets[map_bucketidx(m, hash)];
		while (*next) {
			if ((*next)->hash == hash && !strcmp((*next)->key, key)) {
				return next;
			}
			next = &(*next)->next;
		}
	}
	return NULL;
}


int map_init_(map_base_t *m, unsigned int initial_nbuckets) {
	// Clear the memory.
	memset(m, 0, sizeof(*m));
	m->initialized = true;
	// Pre-initialize buckets array only if initial_nbuckets is a power of 2.
	// Anyway, it is reallocated automatically when needed.
	if ((initial_nbuckets > 0) && !(initial_nbuckets & (initial_nbuckets - 1)))
		return map_resize(m, initial_nbuckets);
	else
		return 0;
}


void map_deinit_(map_base_t *m) {
	if (m == NULL)
		return;
	if (!m->initialized)
		return;
	unsigned int i = m->nbuckets;
	while (i--) {
		map_node_t *node = m->buckets[i];
		while (node) {
			map_node_t *next = node->next;
			map_deletenode(node);
			node = next;
		}
	}
	free(m->buckets);
	m->initialized = false;
}


void *map_get_(map_base_t *m, const char *key) {
	if (m == NULL || key == NULL)
		return NULL;
	if (!m->initialized)
		return NULL;
	map_node_t **next = map_getref(m, key);
	return next ? (*next)->value : NULL;
}


int map_set_(map_base_t *m, const char *key, const void *value, int vsize) {
	if (m == NULL || key == NULL || value == NULL)
		return -1;
	if (!m->initialized)
		return -1;
	/* Find & replace existing node */
	map_node_t **next = map_getref(m, key);
	if (next) {
		memcpy((*next)->value, value, vsize);
		return 0;
	}
	/* Add new node */
	map_node_t *node = map_newnode(key, value, vsize);
	if (node == NULL) goto fail;
	if (m->nnodes >= m->nbuckets) {
		unsigned int n = (m->nbuckets > 0) ? (m->nbuckets << 1) : 1;
		int err = map_resize(m, n);
		if (err) goto fail;
	}
	map_addnode(m, node);
	m->nnodes++;
	return 0;
	fail:
	map_deletenode(node);
	return -1;
}


void map_remove_(map_base_t *m, const char *key) {
	if (m == NULL || key == NULL)
		return;
	if (!m->initialized)
		return;
	map_node_t **next = map_getref(m, key);
	if (next) {
		map_node_t *node = *next;
		*next = (*next)->next;
		map_deletenode(node);
		m->nnodes--;
	}
}


map_iter_t map_iter_(void) {
	map_iter_t iter;
	iter.bucketidx = -1;
	iter.node = NULL;
	return iter;
}


const char *map_next_(map_base_t *m, map_iter_t *iter) {
	if (m == NULL || iter == NULL)
		return NULL;
	if (!m->initialized)
		return NULL;
	if (iter->node) {
		iter->node = iter->node->next;
		if (iter->node == NULL) goto nextBucket;
	} else {
		nextBucket:
		do {
			if (++iter->bucketidx >= m->nbuckets) {
				return NULL;
			}
			iter->node = m->buckets[iter->bucketidx];
		} while (iter->node == NULL);
	}
	return iter->node->key;
}
