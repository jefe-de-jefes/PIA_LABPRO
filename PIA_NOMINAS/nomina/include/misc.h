#pragma once
#include <stdlib.h>
#include <stdio.h>

#define L_NOMBRE 60
#define L_CONTRASENA 20
#define L_CLAVE 20
#define L_CORREO 50
#define L_TELEFONO 10
#define L_CURP 18

#ifdef _WIN32
#define clear() system("cls")
#endif

#ifdef __unix__
#define clear() system("clear")
#endif

#ifdef __APPLE__
#define clear() system("clear")
#endif

#ifndef byte
#define byte unsigned char
#endif

#define es_posicion_valida(pos) (pos>=0)

typedef enum {
    false,
    true,
} bool;

typedef struct {
    unsigned int dia;
    unsigned int mes;
    unsigned int ano;
} Fecha;

typedef struct {
    char calle[L_NOMBRE+1];
    char colonia[L_NOMBRE+1];
    char municipio[L_NOMBRE+1];
    unsigned int numero;
    unsigned int cp;
} Direccion;

void flush(void);
void stop(void);
void leer_string(char* buffer, size_t longitud);
int validar_entero(int min, int max);
float validar_flotante(float min, float max);
FILE* abrir_archivo_binario(const char* nombre_archivo);
int menu(const char* msg, int min, int max);
long encontrar_primero_en_archivo(void *buffer_entrada, void *buffer_salida,
    size_t tamano_elemento, FILE* archivo, int (*comparar)(void*, void*));
long encontrar_siguiente_en_archivo(void *buffer_entrada, void *buffer_salida,
    size_t tamano_elemento, FILE* archivo, int (*comparar)(void*, void*));
bool existe_en_archivo(void* buffer_entrada, size_t tamano_elemento,
    FILE* archivo, int (*comparar)(void*, void*));
