#include <malloc.h>
#include "snake.h"

/**
 * @file
 * @author Mark Said Camilleri
 * @version 20170524.
 */

void SnakeDynArray_add(SnakeDynArray arr, Snake element) {
    // If the array is full, reallocate it
    if(arr.used == arr.size){
        arr.size *= 2;
        arr.array = (Snake *) realloc(arr.array, arr.size * sizeof(Snake));
    }
    arr.array[arr.used++] = element;
};

void SnakeDynArray_removeFromArray(SnakeDynArray arr, unsigned int n) {
    // Shift elements from n to the end 1 step to the left.
    // Does nothing if n >= used
    for (unsigned int i = n; i < arr.used; ++i) {
        arr.array[i] = arr.array[i+1];
    }
};

void SnakeDynArray_clearArray(SnakeDynArray arr) {
    free(arr.array);
    arr.array = NULL;       // reset pointer
    arr.used = arr.size = 0;    // reset sizes
};

