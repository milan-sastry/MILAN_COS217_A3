#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stddef.h>
#include "symtable.h"

enum Counts { c1 = 509, c2 = 1021, c3 = 2039, c4 = 4093, c5 = 8191, c6 = 16381, c7 = 32749, c8 = 65521};

struct Binding{
    const char *key;
    const void *value;
    struct Binding *next;
};
struct SymTable{
    size_t numOfBuckets;
    size_t numOfBindings;
    struct Binding *buckets[c1];
};

/* Return a hash code for pcKey that is between 0 and uBucketCount-1,
   inclusive. */

static size_t SymTable_hash(const char *pcKey, size_t uBucketCount)
{
   const size_t HASH_MULTIPLIER = 65599;
   size_t u;
   size_t uHash = 0;

   assert(pcKey != NULL);

   for (u = 0; pcKey[u] != '\0'; u++)
      uHash = uHash * HASH_MULTIPLIER + (size_t)pcKey[u];

   return uHash % uBucketCount;
}

SymTable_T SymTable_new(void){
    SymTable_T oSymTable;

    oSymTable = (SymTable_T)malloc(sizeof(struct SymTable));
    
    if (oSymTable == NULL){
        return NULL;
    }

    for (int i = 0; i < c1; i++){
        oSymTable->buckets[i] = NULL;
    }

    oSymTable->numOfBindings = 0;
    oSymTable->numOfBuckets = 509;
    return oSymTable;
}

void SymTable_free(SymTable_T oSymTable){
    struct Binding *current;
    struct Binding *next;
    assert(oSymTable != NULL);
    for (int i = 0; i < oSymTable->numOfBuckets; i++){
        if (oSymTable->buckets[i] != NULL){
            current = oSymTable->buckets[i];
            while (current != NULL){
                next = current->next;
                free((void *)current->key);
                free(current);
                current = next;
            }
        }
    }

    free(oSymTable);
}

size_t SymTable_getLength(SymTable_T oSymTable){
    assert(oSymTable != NULL);
    return (oSymTable->numOfBindings);
}

int SymTable_put(SymTable_T oSymTable, const char *pcKey, const void *pvValue){
    struct Binding* newBinding;
    size_t hash;
    assert(oSymTable != NULL);
     if (SymTable_contains(oSymTable, pcKey))
        return 0;
    
    if (oSymTable->numOfBindings == oSymTable->numOfBuckets && oSymTable->numOfBindings != c8){
        SymTable_expand(oSymTable,oSymTable->numOfBuckets);
    }

    newBinding = (struct Binding*)malloc(sizeof(struct Binding));
    newBinding->key = (const char*)malloc(strlen(pcKey)+1);
    if (newBinding == NULL || newBinding->key == NULL) return 0;
    strcpy((char *)newBinding->key,pcKey);

    size_t hash = SymTable_hash(pcKey,oSymTable->numOfBuckets);
    if (oSymTable->buckets[hash] == NULL)
        newBinding->next = NULL; 
    else{
        struct Binding *temp = oSymTable->buckets[hash];
        oSymTable->buckets[hash] = newBinding;
        newBinding->next = temp;
    }  
    newBinding->value = pvValue;
    oSymTable->numOfBindings++;
    return 1;
}

void *SymTable_replace(SymTable_T oSymTable, const char *pcKey, const void *pvValue){
    struct Binding* current;
    struct void* temp;
    size_t hash;
    assert(oSymTable != NULL);

    if (!SymTable_contains(oSymTable,pcKey))
        return NULL;

    hash = SymTable_hash(pcKey,oSymTable->numOfBuckets);
    current = oSymTable->buckets[hash];
    while (strcmp((char *)current->key,pcKey) != 0)
        current = current->next;
    temp = current->value;;
    current->value = pvValue;
    return (void *)temp;
}

int SymTable_contains(SymTable_T oSymTable, const char *pcKey){
    struct Binding* current;
    size_t hash;
    assert(oSymTable != NULL);
    current = oSymTable->buckets[hash];
    hash = SymTable_hash(pcKey,oSymTable->numOfBuckets);
    while (current != NULL){
        if (strcmp((const char*)current->key,pcKey) == 0) return 1;
        current = current->next;
    }
    return 0;
}

void *SymTable_get(SymTable_T oSymTable, const char *pcKey){
    struct Binding* current;
    size_t hash;
    assert(oSymTable != NULL);

    hash = SymTable_hash(pcKey,oSymTable->numOfBuckets);
    current = oSymTable->buckets[hash];
    
    while (current != NULL){
        if (strcmp((const char*)current->key,pcKey) == 0) 
            return (void *)current->value;
        current = current->next;
    }
    return NULL;
}

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey){
    struct Binding *current;
    struct Binding *prev;
    size_t hash;
    const void *temp;
    assert(oSymTable != NULL);

    hash = SymTable_hash(pcKey,oSymTable->numOfBuckets);
    current = oSymTable->buckets[hash];
    prev = NULL;

    while (current != NULL){
        if(strcmp((const char *)current->key,pcKey) == 0)
            break;
        prev = current;
        current = current->next;
    }

    if (current == NULL)
        return NULL;

    if (prev == NULL){
        oSymTable->buckets[hash] = current->next;
    }
    else{
        prev->next = current->next;
    }
    temp = current->value;
    oSymTable->length--;
    free((void *)current->key);
    free(current);
    
    return (void *)temp;
}

void SymTable_map(SymTable_T oSymTable,
    void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
    const void *pvExtra){
        struct Binding current*;
        assert(oSymTable != NULL);


        for (int i = 0; i < oSymTable->numOfBuckets; i++){
            current = oSymTable->buckets[i]
            while (current != NULL){
                pfApply(current->key, current->value,pvExtra);
                current = current->next;
            }
        }

    }

// Expands oSymTable from currentNumOfBuckets to the next predetermined number of buckets.
// Exits function if not enough memory to allocate larger array.
static void SymTable_expand(SymTable_T oSymTable, size_t currentNumOfBuckets){
    struct Binding *current;
    struct Binding *next;
    size_t newSize;

    switch (currentNumOfBuckets) {
        case c1:
            newSize = c2;
            break;
        case c2:
            newSize = c3;
            break;
        case c3:
            newSize = c4;
            break;
        case c4:
            newSize = c5;
            break;
        case c5:
            newSize = c6;
            break;
        case c6:
            newSize = c7;
            break;
        case c7:
            newSize = c8;
            break;
        default:
            break;
    }
    struct Binding *newBuckets[newSize];
    assert(oSymTable != NULL);
        
    newBuckets = (struct Binding*)malloc(newSize * sizeof(struct Binding));
    if (newBuckets == NULL) return;

    for (int i = 0; i < newSize; i++){
        newBuckets[i] = NULL;
    }
    for (int i = 0; i < oSymTable->numOfBuckets; i++){
        current = oSymTable->buckets[i];
        next = current->next;
        if (current == NULL) continue;
        while (current != NULL){
            size_t hash = SymTable_hash(current->key,newSize);

            if (newBuckets[hash] != NULL)
                struct Binding *temp = newBuckets[hash];
                current->next = temp;
            }
            newBuckets[hash] = current;
            current = next;
            
    
    }
    free(oSymTable->buckets);
    oSymTable->buckets = newBuckets;
    oSymTable->numOfBuckets = newSize;
}
