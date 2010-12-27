/**
 * C-Projekt von Gruppe 37
 * Autor:
 * Datum:
 * 
 * memory.h
 */

 #ifndef _C37_MEMORY_H
 #define _C37_MEMORY_H
 
 /* Wrapper f�r malloc() mit NULL-Exception */
 void* ex_malloc(size_t);
 /* Wrapper f�r realloc() mit NULL-Exception */
 void* ex_realloc(void*, size_t);
 /* Wrapper f�r calloc() mit NULL-Exception */
 void* ex_calloc(size_t, size_t);
 
 #endif