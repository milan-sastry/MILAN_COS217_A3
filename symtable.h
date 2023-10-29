/*--------------------------------------------------------------------*/
/* symtable.h                                                         */
/* Author: Milan Sastry                                               */
/*--------------------------------------------------------------------*/
#include <stddef.h>

/**/
typedef struct SymTable *SymTable_T;
  
/*Creates and returns an empty Symbol Table*/
SymTable_T SymTable_new(void);

/*Frees all the memory associated with oSymTable*/
void SymTable_free(SymTable_T oSymTable);

/*Returns number of bindings in oSymTable*/
size_t SymTable_getLength(SymTable_T oSymTable);

/*Inserts new binding with pcKey and pvValue into oSymTable. 
Returns 0 if pcKey is already in the table or if insufficient memory, 1 if succesful*/
int SymTable_put(SymTable_T oSymTable,
    const char *pcKey, const void *pvValue);

/*Replaces the value in oSymTable associated with pcKey with pvValue 
and returns old value. Returns NULL if pcKey is not in oSymTable*/
void *SymTable_replace(SymTable_T oSymTable,
    const char *pcKey, const void *pvValue);

/*Returns 1 if there is a binding with pcKey in oSymTable,
returns 0 if there is not*/
int SymTable_contains(SymTable_T oSymTable, const char *pcKey);

/*Returns the value associated with pcKey in oSymTable*/
void *SymTable_get(SymTable_T oSymTable, const char *pcKey);

/*Removes the binding with pcKey if it exists in oSymTable, returns value
of binding. Returns NULL if pcKey is not in oSymTable*/
void *SymTable_remove(SymTable_T oSymTable, const char *pcKey);

/*Applies the function pfApply to all bindings in oSymTable, passes pvExtra as 
an argument of pfApply*/
void SymTable_map(SymTable_T oSymTable,
    void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
    const void *pvExtra);