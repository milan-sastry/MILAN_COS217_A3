#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stddef.h>
#include "symtable.h"

enum { BUCKET_COUNT = 509}

struct Binding{
    const char *key;
    const void *value;
    struct Binding *next;
};
struct SymTable{
    size_t length;
    struct Binding *buckets[BUCKET_COUNT];
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
    oSymTable->buckets = (struct Binding*)malloc(sizeof(struct Binding) * BUCKET_COUNT);
    if (oSymTable == NULL){
        return NULL;
    }

    oSymTable->length = 0;
    return oSymTable;
}

void SymTable_free(SymTable_T oSymTable){
    
}
