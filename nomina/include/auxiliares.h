#pragma once
#include <stdio.h>
#include "array.h"

#define H_TRABAJADAS_MIN 0
#define H_TRABAJADAS_MAX 300
#define MIN_M 1
#define MAX_M 12
#define MIN_A 1900
#define MAX_A 3000

typedef struct datos_nomina_s {
    Empleado empleado;
    Nomina nomina;
    long pos;
} DatosNomina;

const char* get_string_perfil(Perfil perfil);
const char* get_string_estatus(Estatus estatus);

int comparar_nombre_usuarios(void* ptr1, void* ptr2);
int registrar_proyecto(FILE* archivo_proyectos);
void baja_proyecto(FILE* archivo_proyectos, FILE* archivo_empleados);

void registro_empleado(AppData* appdata);
void baja_empleado(AppData* appdata);

bool fecha_dentro_de_periodo(Fecha *fecha, Proyecto *proyecto);
int comparar_clave_proyectos(void* ptr1, void* ptr2);
void cargar_empleados_proyecto(AppData* appdata, Array nominas, Proyecto* proyecto);
void cargar_nominas_empleados(AppData* appdata, Array nominas, Fecha* fecha_nomina, Proyecto* proyecto);
int comparar_tipo_nomina(void* ptr1, void* ptr2);
int capturar_nominas(Array nominas, Proyecto* proyecto, bool interrumpible,
    DatosNomina* tipo_nomina, int(*condicion)(void*,void*));
void guardar_cambios_nominas(AppData* appdata, Array nominas);

void reporte_proyectos(AppData* appdata);
void reporte_nominas(AppData* appdata);

void print_proyecto_debug(void* proyecto);
void print_empleado_debug(void* empleado);
void print_nomina_debug(void* nomina);
void print_pos_debug(void* pos);
void print_datos_debug(void* datos);
void print_all_debug(size_t tamano_elemento, FILE* archivo, void(*imprimir)(void*));
void print_arreglo_debug(Array arreglo, const char* nombre_arreglo, void (*print_elemento)(void*));