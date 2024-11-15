#include <stdlib.h>
#include <string.h>
#include "array.h"

#ifndef byte
#define byte unsigned char
#endif

#define ARRAY_POLITICA_DE_CRECIMIENTO 2

struct array_s {
    size_t anchura;
    size_t longitud;
    size_t capacidad;
    array_error error;
    void *datos;
};


Array array_init(size_t anchura, size_t capacidad, void* datos, size_t longitud)
{
    Array array = (Array)malloc(sizeof(struct array_s));
    array->anchura=anchura;
    array->capacidad=capacidad;
    array->error=ARRAY_SIN_ERROR;
    array->datos=calloc(capacidad, anchura);
    if(datos==NULL) {
        array->longitud=0;
        return array;
    }
    if(longitud>capacidad) {
        array->error=ARRAY_LONGITUD_MAYOR_A_CAPACIDAD_ERROR;
        longitud=capacidad;
    }
    array->longitud=longitud;
    memcpy(array->datos, datos, anchura*longitud);
    return array;
}


void array_set(Array array, size_t indice, void* buffer)
{
    if(indice>=array->longitud) {
        array->error=ARRAY_INDICE_FUERA_DE_RANGO_ERROR;
        return;
    }
    byte* pos = ((byte*)array->datos)+indice*array->anchura;
    memcpy(pos, buffer, array->anchura);
    return;
}


void array_get(Array array, size_t indice, void* buffer)
{
    if(indice>array->longitud) {
        array->error=ARRAY_INDICE_FUERA_DE_RANGO_ERROR;
        return;
    }
    byte* pos = ((byte*)array->datos)+indice*array->anchura;
    memcpy(buffer, pos, array->anchura);
}


void array_resize(Array array, size_t longitud, size_t capacidad)
{
    if(longitud>capacidad){
        array->error=ARRAY_LONGITUD_MAYOR_A_CAPACIDAD_ERROR;
        longitud=capacidad;
    }
    void* new_datos=calloc(capacidad, array->anchura);
    memcpy(new_datos, array->datos, array->longitud*array->anchura);
    array->longitud=longitud;
    array->capacidad=capacidad;
    free(array->datos);
    array->datos=new_datos;
    return;
}


void array_push(Array array, void* buffer)
{
    if(array->longitud >= array->capacidad) {
        array_resize(array, array->longitud, array->capacidad*ARRAY_POLITICA_DE_CRECIMIENTO);
    }
    array->longitud++;
    array_set(array, array->longitud-1, buffer);
    return;
}


int array_index(Array array, void* elemento, int (*comparar_elementos)(void*, void*))
{
    byte *pos = (byte*)array->datos;
    for(int i=0; i< array->longitud; i++) {
        if(comparar_elementos(elemento, pos)) return i;
        pos += array->anchura;
    }
    return -1;
}

int array_existe_elemento(Array array, void* elemento, int(*comparar_elementos)(void*,void*))
{
    byte *pos = (byte*)array->datos;
    for(int i=0; i< array->longitud; i++) {
        if(comparar_elementos(elemento, pos)) return 1;
        pos += array->anchura;
    }
    return 0;
}

void array_print(Array array, void (*print_elemento)(void*))
{
    byte *pos = (byte*)array->datos;
    for(int i=0; i < array->longitud; i++) {
        print_elemento(pos);
        pos += array->anchura;
    }
    return;
}

size_t array_get_longitud(Array array) {return array->longitud;}

size_t array_get_capacidad(Array array) {return array->capacidad;}

array_error array_get_error(Array array) {return array->error;}

void array_clear_error(Array array) {array->error=ARRAY_SIN_ERROR; return;}

void array_destroy(Array array)
{
    free(array->datos);
    free(array);
    return;
}