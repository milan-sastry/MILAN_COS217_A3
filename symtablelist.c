#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stddef.h>
#include "symtable.h"

struct Node {
    const char *key;
    const void *value;
    struct SymTableNode *next;

}

struct SymTable{
    size_t length;
    struct Node *first;

}
SymTable_T SymTable_new(void){
    SymTable_T oSymTable;
    oSymTable = (SymTable_T)malloc(sizeof(struct SymTable));
    if (oSymTable == NULL){
        return NULL;
    }

    oSymTable->first = NULL;
    oSymTable->length = 0;
    return oSymTable;
}

void SymTable_free(SymTable_T oSymTable){
    struct Node *current;
    struct Node *next;

    assert(oSymTable != NULL);
    current = oSymTable->first;

    while(current != NULL){
        next = current->next;
        free((void *)current->key);
        free(current);
        current = next;
    }
    oSymTable->length = 0;
    free(oSymTable);
}

size_t SymTable_getLength(SymTable_T oSymTable){
    assert(oSymTable != NULL);
    return (oSymTable->length);
}

int SymTable_put(SymTable_T oSymTable, const char *pcKey, const void *pvValue){
    struct Node *newNode;
    struct Node *temp;

    assert(oSymTable != NULL);
    if (SymTable_contains(oSymTable, pcKey))
        return 0;

    temp = oSymTable->first;
    newNode = (struct Node*)malloc(sizeof(struct Node));
    newNode->key = (const char*)malloc(strlen(pcKey) + 1);

    if (newNode == NULL || newNode->key == NULL)
        return 0;
    strcpy(newNode->key,pcKey);
    newNode->value = pvValue;
    newNode->next = temp;
    oSymTable->first = newNode;
    oSymTable->length++;

    return 1;
}

void *SymTable_replace(SymTable_T oSymTable, const char *pcKey, const void *pvValue){
    const void *temp;
    struct Node *current;

    assert(oSymTable != NULL);

    if (!SymTable_contains(oSymTable,pcKey))
        return NULL;

    current = oSymTable->first;
    while(strcmp((const char *)current->key,pcKey)!=0)
        current = current->next;
    temp = current->value;
    current->value = pvValue;
    return temp;
}

int SymTable_contains(SymTable_T oSymTable, const char *pcKey){
    struct Node *current;
    assert(oSymTable != NULL);
    current = oSymTable->first;

    while (current != NULL){
        if (strcmp((const char *)current->key,pcKey)==0)
            return 1;
        current = current->next;
    }
    return 0;
}

void *SymTable_get(SymTable_T oSymTable, const char *pcKey){
    struct Node *current;
    assert(oSymTable != NULL);
    current = oSymTable->first;

    while (current != NULL){
        if (strcmp((const char *)current->key,pcKey) == 0){
            return (void *)current->value;
        }
        current = current->next;
    }
    return NULL;
}

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey){
    struct Node *current;
    struct Node *prev;
    const void *temp;
    assert(oSymTable != NULL);

    current = oSymTable->first;
    prev = NULL;

    while (current != NULL && strcmp((const char *)current->key,pcKey)==0){
        prev = current;
        current = current->next;
    }

    if (current == NULL)
        return NULL;

    if (prev == NULL){
        oSymTable->first = current->next;
    }
    else{
        prev->next = current->next;
    }
    temp = current->value;
    oSymTable->length--;
    free((void *)current->key);
    free(current);
    
    return temp;
}

void SymTable_map(SymTable_T oSymTable,
    void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
    const void *pvExtra){
        struct Node *current;

        assert(oSymTable != NULL);
        current = oSymTable->first;
        while (current != NULL){
            pfApply(current->key, current->value,(void *)pvExtra);
            current = current->next;
        }

    }

