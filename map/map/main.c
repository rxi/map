//
//  main.c
//  map
//
//  Created by kpishere on 2020-06-18.
//  Copyright © 2020 kpishere
//
// This library is free software; you can redistribute it and/or modify it
// under the terms of the MIT license. See LICENSE for details.
//

#include <stdio.h>
#include <stdlib.h>
#include "../../src/mapsk.h"

// Basic use of map with string keys
void testBasicMap() {
    printf("\nTest basic map\n");
    
    map_int_t m;
    map_init(&m);
    
    map_set(&m, "testkeyA", 123);
    map_set(&m, "testkeyB", 124);
    map_set(&m, "testke", 125);

    int *val = map_get(&m, "testkeyA");
    if (val) {
      printf("value: %d\n", *val);
    } else {
        printf("ERROR 'val == NULL' with 'testkeyA'");
        exit(1);
    }
    
    val = map_get(&m, "testkey");
    if (val) {
        printf("ERROR 'val != NULL' with 'testkey'");
        exit(2);
    } else {
        printf("value not found\n");
    }

    map_deinit(&m);
}

typedef struct {
    uint8_t c;
    int d;
} myValueType;

typedef map_t(myValueType) myValueType_map_t;


// Basic use of map with string keys and struct type values
void testBasicWStructValMap() {
    printf("\nTest basic map with struct type values\n");
    
    myValueType_map_t m;
    map_init(&m);
    
    myValueType v1 = { 0x42, 17};
    myValueType v2 = { 0x42, 18};
    myValueType v3 = { 0x42, 19};


    map_set(&m, "testkeyA", v1);
    map_set(&m, "testkeyB", v2);
    map_set(&m, "testke", v3);

    myValueType *val = map_get(&m, "testkeyA");
    if (val) {
      printf("value: %02X %d\n", val->c, val->d);
    } else {
        printf("ERROR 'val == NULL' with 'testkeyA'");
        exit(1);
    }
    
    val = map_get(&m, "testkey");
    if (val) {
        printf("ERROR 'val != NULL' with 'testkey'");
        exit(2);
    } else {
        printf("value not found\n");
    }

    map_deinit(&m);
}



// Use of map with struct keys that have string serializer for struct key
typedef struct myTypeS {
    uint8_t a;
    int b;
} myType;

#define CALC_SERIALIZESTRINGSIZE(type) (sizeof(type)*2 + 1)
char *serialize_myType(const myType* val) {
    static char serialize_myTypeBuff[CALC_SERIALIZESTRINGSIZE(myType)];
    sprintf(serialize_myTypeBuff,"%02x%08x",val->a, (int32_t)val->b);
    return serialize_myTypeBuff;
}

typedef map_t(myType) uint_map_t;

void testStructSerializeMap() {
    printf("\nTest map with serializer for struct key\n");
    
    const myType a = { 0x40, 123};
    const myType b = { 0x40, 124};
    const myType c = { 0x40, 125};

    map_int_t m;
    map_init(&m);
    
    map_set(&m, serialize_myType(&a), 123);
    map_set(&m, serialize_myType(&b), 124);
    map_set(&m, serialize_myType(&c), 125);

    int *val = map_get(&m, serialize_myType(&b));
    if (val) {
      printf("value(%s): %d =?= %d\n",serialize_myType(&b), *val, b.b);
    } else {
        printf("ERROR 'val == NULL' with '%s'", serialize_myType(&b));
        exit(1);
    }
    
    map_deinit(&m);
}


void testStructMap() {
    printf("\nTest map with struct key\n");
    
    const myType a = { 0x40, 123};
    const myType b = { 0x40, 124};
    const myType c = { 0x40, 125};

    map_int_t m;
    map_init(&m);
    
    map_set_sk(&m, &a, sizeof(myType), 123);
    map_set_sk(&m, &b, sizeof(myType), 124);
    map_set_sk(&m, &c, sizeof(myType), 125);

    int *val = map_get_sk(&m, &c, sizeof(myType));
    if (val) {
      printf("value(%s): %d =?= %d\n",serialize_myType(&c), *val, c.b);
    } else {
        printf("ERROR 'val == NULL' with '%s'", serialize_myType(&c));
        exit(1);
    }
    
    map_remove_sk(&m, &c, sizeof(myType));

    val = map_get_sk(&m, &c, sizeof(myType));
    if (val) {
        printf("value(%s): %d =?= %d\n",serialize_myType(&c), *val, c.b);
        printf("ERROR 'val != NULL' with '%s'", serialize_myType(&c));
        exit(1);
    } else {
        printf("value not found (%s)\n", serialize_myType(&c));
    }
    map_deinit(&m);
}

int main(int argc, const char * argv[]) {

    testBasicMap();
    testBasicWStructValMap();
    testStructSerializeMap();
    testStructMap();
    
    return 0;
}


