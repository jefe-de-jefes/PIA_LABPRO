#include <stdio.h>
#include "array.h"

#define H_TRABAJADAS_MIN 0
#define H_TRABAJADAS_MAX 300
#define MIN_M 1
#define MAX_M 12
#define MIN_A 1900
#define MAX_A 3000

int comparar_nombre_usuarios(void* ptr1, void* ptr2);
int registrar_proyecto(FILE* archivo_proyectos);
void baja_proyecto(FILE* archivo_proyectos, FILE* archivo_empleados);

void registro_empleado(AppData* appdata);
void baja_empleado(AppData* appdata);

bool fecha_dentro_de_periodo(Fecha *fecha, Proyecto *proyecto);
int comparar_clave_proyectos(void* ptr1, void* ptr2);
int comparar_empleado_en_proyecto(void* ptr1, void* ptr2);
int comparar_nomina_en_proyecto(void* ptr1, void* ptr2);
Array emparejar_empleados(Array nominas, Array empleados);
void actualizar_nominas(FILE* archivo, Array nominas, Array posiciones);

void mostrar_empleado_reporte(Empleado emp);
void reporte_proyectos(AppData* appdata);
void print_proyecto_debug(void* proyecto);
void print_empleado_debug(void* empleado);
void print_nomina_debug(void* nomina);
void print_pos_debug(void* pos);
void print_all_debug(size_t tamano_elemento, FILE* archivo, void(*imprimir)(void*));
