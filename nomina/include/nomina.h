#pragma once
#include <stdio.h>
#include "misc.h"

#define ARCHIVO_USUARIOS "usuarios.bin"
#define ARCHIVO_PROYECTOS "proyectos.bin"
#define ARCHIVO_EMPLEADOS "empleados.bin"
#define ARCHIVO_NOMINAS "nominas.bin"

#define HORAS_MENSUALES_MINIMAS 180
#define LOGIN_MAX_INTENTOS 3

typedef enum {
    INACTIVO,
    ACTIVO,
    BASURA,
} Estatus;

typedef enum {
    LIDER_PROYECTO=1,
    ADMIN_BD,
    ANALISTA,
    PROGRAMADOR,
    TESTER,
} Perfil;

typedef struct {
    char nombre[L_NOMBRE+1];
    char contrasena[L_CONTRASENA+1];
    unsigned int intentos;
    bool bloqueado;
    Estatus estatus;
} Usuario;

typedef struct {
    char clave_proyecto[L_CLAVE+1];
    char nombre[L_NOMBRE+1];
    float monto_inicial;
    float total_nomina;
    Fecha inicio;
    Fecha termino;
    Estatus estatus;
} Proyecto;

typedef struct {
    char clave_proyecto[L_CLAVE+1];
    char nombre[L_NOMBRE+1];
    char curp[L_CURP+1];
    char correo[L_CORREO+1];
    char telefono[L_TELEFONO+1];
    unsigned int num_empleado;
    float tarifa_mensual;
    Fecha nacimiento;
    Perfil perfil;
    Direccion direccion;
    Estatus estatus;
} Empleado;

typedef struct {
    char clave_proyecto[L_CLAVE+1];
    char curp_empleado[L_CURP+1];
    unsigned int mes;
    unsigned int ano;
    unsigned int horas_trabajadas;
    float sueldo_mensual;
    Estatus estatus;
} Nomina;

typedef struct {
    FILE *archivo_proyectos;
    FILE *archivo_empleados;
    FILE *archivo_nominas;
    char clave_autorizacion[L_CONTRASENA+1];
} AppData;

void login(AppData* appdata);
void cargar_datos(AppData* appdata);
void submenu_proyectos(AppData* appdata);
void submenu_empleados(AppData* appdata);
void submenu_nomina(AppData* appdata);
void submenu_reportes(AppData* appdata);
void submenu_debug(AppData* appdata);
void liberar_memoria(AppData* appdata);
