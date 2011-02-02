/**
 * C-Projekt von Gruppe 37
 * Autor: Marcel
 * Datum: 13.01.2011
 * 
 * memory.c
 */

 #include "globals.h"
 #include "memory.h"

 /* die Uni wills so irgendwie ... */

 /*--------------------------------------------------------------------------*/
 void* ex_malloc(size_t size) {
    void* ptr = malloc(size);
    #ifdef _C37_MEMORY_LOG
    printf("malloc'd: %d to %d\n", size, ptr);
    #endif
    exit_on_low_memory(ptr);
    return ptr;
 }
 
 /*--------------------------------------------------------------------------*/
 void* ex_realloc(void* ptr, size_t size) {
    ptr = realloc(ptr, size);
    #ifdef _C37_MEMORY_LOG
    printf("realloc'd: %d to %d\n", size, ptr);
    #endif
    exit_on_low_memory(ptr);
    return ptr;
 }
 
 /*--------------------------------------------------------------------------*/
 void* ex_calloc(size_t num, size_t size) {
    void* ptr = calloc(num, size);
    #ifdef _C37_MEMORY_LOG
    printf("calloc'd: %d * %d to %d\n", num, size, ptr);
    #endif
    exit_on_low_memory(ptr);
    return ptr;
 }
 
 /*--------------------------------------------------------------------------*/
 void exit_on_low_memory(void* ptr) {
    if(ptr == NULL) {
        fprintf(stderr, "Insufficient memory to continue!");
        exit(1);
    }
 }