#pragma once
#include <stddef.h>

typedef enum array_error_e {
    ARRAY_SIN_ERROR,
    ARRAY_LONGITUD_MAYOR_A_CAPACIDAD_ERROR,
    ARRAY_INDICE_FUERA_DE_RANGO_ERROR,
    ARRAY_FUERA_MEMORIA_ERROR, //Implementar en Resize
} array_error;

typedef struct array_s* Array;

Array array_init(size_t anchura, size_t capacidad, void* datos, size_t longitud);
void array_set(Array array, size_t indice, void* buffer);
void array_get(Array array, size_t indice, void* buffer);
void array_resize(Array array, size_t longitud, size_t capacidad);
void array_push(Array array, void* buffer);
int array_index(Array array, void* elemento, int (*comparar_elementos)(void*, void*));
void array_print(Array array, void (*print_elemento)(void*));
size_t array_get_longitud(Array array);
size_t array_get_capacidad(Array array);
array_error array_get_error(Array array);
void array_clear_error(Array array);
void array_destroy(Array array);