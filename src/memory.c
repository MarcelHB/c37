/**
 * C-Projekt von Gruppe 37
 * Autor: Marcel
 * Datum: 13.01.2011
 * 
 * memory.c
 */

#include "globals.h"

 /* die Uni wills so irgendwie ... */

 /*--------------------------------------------------------------------------*/
 void* ex_malloc(size_t size) {
	void* ptr = malloc(size);
	exit_on_low_memory(ptr);
	return ptr;
 }
 
 /*--------------------------------------------------------------------------*/
 void* ex_realloc(void* ptr, size_t size) {
	ptr = realloc(ptr, size);
	exit_on_low_memory(ptr);
	return ptr;
 }
 
 /*--------------------------------------------------------------------------*/
 void* ex_realloc(size_t num, size_t size) {
	ptr = calloc(num, size);
	exit_on_low_memory(ptr);
	return ptr;
 }
 
 /*--------------------------------------------------------------------------*/
 void exit_on_low_memory(void* ptr) {
	if(ptr == NULL)
		fprintf(stderr, "Insufficient memory to continue!");
		exit(1);
	}
 }