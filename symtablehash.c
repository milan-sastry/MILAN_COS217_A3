#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stddef.h>
#include "symtable.h"

static const size_t auBucketCounts[] = {509, 1021, 2039, 4093, 8191, 16381, 32749, 65521};

struct Binding{
    const char *key;
    const void *value;
    struct Binding *next;
};
struct SymTable{
    size_t numOfBuckets;
    size_t numOfBindings;
    struct Binding **buckets;
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
    size_t i;

    oSymTable = (SymTable_T)malloc(sizeof(struct SymTable));
    if (oSymTable == NULL) return NULL;
    oSymTable->buckets = (struct Binding **)malloc(sizeof(struct Binding *) * auBucketCounts[0]);
    if (oSymTable->buckets == NULL) return NULL;
    
    for (i = 0; i < auBucketCounts[0]; i++){
        oSymTable->buckets[i] = NULL;
    }

    oSymTable->numOfBindings = 0;
    oSymTable->numOfBuckets = 0;
    return oSymTable;
}

void SymTable_free(SymTable_T oSymTable){
    struct Binding *current;
    struct Binding *next;
    size_t i;
    assert(oSymTable != NULL);
    for (i = 0; i < auBucketCounts[oSymTable->numOfBuckets]; i++){
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
    struct Binding* current;
    struct Binding* next;
    size_t hash;
    size_t i;
    assert(oSymTable != NULL);
    if (SymTable_contains(oSymTable, pcKey)) return 0;
    
    if (oSymTable->numOfBindings == auBucketCounts[oSymTable->numOfBuckets] && oSymTable->numOfBindings != auBucketCounts[7]){
        struct Binding **newBuckets;
        newBuckets = (struct Binding **)malloc(auBucketCounts[oSymTable->numOfBuckets + 1]* sizeof(struct Binding));
        if (newBuckets != NULL) {
            for (i = 0; i < auBucketCounts[oSymTable->numOfBuckets]+1; i++){
                newBuckets[i] = NULL;
            }
            for (i = 0; i < auBucketCounts[oSymTable->numOfBuckets]; i++){
                current = oSymTable->buckets[i];
                while (current != NULL){
                    next = current->next;
                    hash = SymTable_hash(current->key,auBucketCounts[oSymTable->numOfBuckets + 1]);
                    if (newBuckets[hash] != NULL){
                        struct Binding *temp = newBuckets[hash];
                        current->next = temp;
                    }
                    else
                        current->next = NULL;
                    newBuckets[hash] = current;
                    current = next;
                }
            } 
            free(oSymTable->buckets);
            oSymTable->buckets = newBuckets;
            oSymTable->numOfBuckets++;
            printf("Num of buckets is %lu!",auBucketCounts[oSymTable->numOfBuckets]);
        }
    }

    newBinding = (struct Binding*)malloc(sizeof(struct Binding));
    if (newBinding == NULL) return 0;
    newBinding->key = (const char*)malloc(strlen(pcKey)+1);
    if (newBinding->key == NULL) return 0;
    
    strcpy((char *)newBinding->key,pcKey);
    hash = SymTable_hash(pcKey,auBucketCounts[oSymTable->numOfBuckets]);
    if (oSymTable->buckets[hash] == NULL) newBinding->next = NULL; 
    else{
        struct Binding *temp = oSymTable->buckets[hash];
        newBinding->next = temp;
    }  
    newBinding->value = pvValue;
    oSymTable->buckets[hash] = newBinding;
    oSymTable->numOfBindings++;
    return 1;
}

void *SymTable_replace(SymTable_T oSymTable, const char *pcKey, const void *pvValue){
    struct Binding *current;
    const void *temp;
    size_t hash;
    assert(oSymTable != NULL && pcKey != NULL);

    if (!SymTable_contains(oSymTable,pcKey))
        return NULL;

    hash = SymTable_hash(pcKey,auBucketCounts[oSymTable->numOfBuckets]);
    current = oSymTable->buckets[hash];
    while (strcmp((char *)current->key,pcKey) != 0)
        current = current->next;
    temp = current->value;
    current->value = pvValue;
    return (void *)temp;
}

int SymTable_contains(SymTable_T oSymTable, const char *pcKey){
    struct Binding* current;
    size_t hash;
    assert(oSymTable != NULL && pcKey != NULL);

    hash = SymTable_hash(pcKey,auBucketCounts[oSymTable->numOfBuckets]);
    current = oSymTable->buckets[hash];

    while (current != NULL){
        if (strcmp((const char*)current->key,pcKey) == 0) return 1;
        current = current->next;
    }
    return 0;
}

void *SymTable_get(SymTable_T oSymTable, const char *pcKey){
    struct Binding* current;
    size_t hash;
    assert(oSymTable != NULL && pcKey != NULL);

    hash = SymTable_hash(pcKey,auBucketCounts[oSymTable->numOfBuckets]);
    current = oSymTable->buckets[hash];
    
    while (current != NULL){
        if (strcmp((const char*)current->key,pcKey) == 0) return (void *)current->value;
        current = current->next;
    }
    return NULL;
}

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey){
    struct Binding *current;
    struct Binding *prev;
    size_t hash;
    const void *temp;

    assert(oSymTable != NULL && pcKey != NULL);

    hash = SymTable_hash(pcKey,auBucketCounts[oSymTable->numOfBuckets]);
    current = oSymTable->buckets[hash];
    prev = NULL;

    while (current != NULL){
        if(strcmp((const char *)current->key,pcKey) == 0)
            break;
        prev = current;
        current = current->next;
    }

    if (current == NULL) return NULL;

    if (prev == NULL) oSymTable->buckets[hash] = current->next;
    else
        prev->next = current->next;
    
    temp = current->value;
    oSymTable->numOfBindings--;
    free((void *)current->key);
    free(current);
    return (void *)temp;
}

void SymTable_map(SymTable_T oSymTable,
    void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
    const void *pvExtra){
        struct Binding *current;
        size_t i;
        assert(oSymTable != NULL);

        for (i = 0; i < auBucketCounts[oSymTable->numOfBuckets]; i++){
            current = oSymTable->buckets[i];
            while (current != NULL){
                pfApply(current->key, (void *)current->value,(void *)pvExtra);
                current = current->next;
            }
        }
}


