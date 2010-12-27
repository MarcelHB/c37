/**
 * C-Projekt von Gruppe 37
 * Autor:
 * Datum:
 * 
 * memory.h
 */

 #ifndef _C37_MEMORY_H
 #define _C37_MEMORY_H
 
 /* Wrapper für malloc() mit NULL-Exception */
 void* ex_malloc(size_t);
 /* Wrapper für realloc() mit NULL-Exception */
 void* ex_realloc(void*, size_t);
 /* Wrapper für calloc() mit NULL-Exception */
 void* ex_calloc(size_t, size_t);
 
 #endif