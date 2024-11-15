#include <stdio.h>
#include <string.h>
#include "admin.h"
#include "nomina.h"
#include "misc.h"

const char* titulo_app = "ADMIN USUARIOS";

char* afirmacion[2] = {"No", "Si"};

void print_usuario(Usuario* usuario)
{
    printf("Nombre: %s\n", usuario->nombre);
    printf("Contrasena: %s\n", usuario->contrasena);
    printf("Intentos: %u\n", usuario->intentos);
    printf("Bloqueado: %s\n", afirmacion[usuario->bloqueado]);
    printf("Basura: %s\n", afirmacion[usuario->estatus==BASURA]);
    return;
}

void listar_usuarios(FILE* archivo)
{
    Usuario buffer;
    clear();
    printf("Lista de Usuarios Registrados:\n\n");
    rewind(archivo);
    while(fread(&buffer, sizeof(Usuario), 1, archivo)) {
        print_usuario(&buffer);
        putchar('\n');
    }
    stop();
    return;
}

void desbloquear_usuario(FILE* archivo)
{
    return;
}

void anadir_usuario(FILE* archivo)
{
    Usuario buffer_datos;
    Usuario buffer_lectura;
    clear();
    printf("Ingrese los datos solicitados...\n\n");
    printf("Introduzca el nombre del usuario:\n");
    leer_string(buffer_datos.nombre, L_NOMBRE);
    printf("Introduzca la contrasena del usuario:\n");
    leer_string(buffer_datos.contrasena, L_CONTRASENA);
    buffer_datos.intentos=0;
    buffer_datos.bloqueado=false;
    buffer_datos.estatus=ACTIVO;
    rewind(archivo);
    while(fread(&buffer_lectura, sizeof(Usuario), 1, archivo)) {
        if(buffer_lectura.estatus==BASURA) {
            fseek(archivo, -sizeof(Usuario), SEEK_CUR);
            fwrite(&buffer_datos, sizeof(Usuario), 1, archivo);
            printf("El usuario ha sido agregado exitosamente\n");
            stop();
            return;
        }
    }
    fwrite(&buffer_datos, sizeof(Usuario), 1, archivo);
    printf("El usuario ha sido agregado exitosamente\n");
    stop();
    return;
}

void eliminar_usuario(FILE* archivo)
{
    Usuario buffer_datos;
    Usuario buffer_lectura;
    clear();
    printf("Introduzca el nombre del usuario a eliminar:\n");
    leer_string(buffer_datos.nombre, L_NOMBRE);
    rewind(archivo);
    while(fread(&buffer_lectura, sizeof(Usuario), 1, archivo)) {
        if(strcmp(buffer_datos.nombre, buffer_lectura.nombre)==0) {
            buffer_lectura.estatus=BASURA;
            fseek(archivo, -sizeof(Usuario), SEEK_CUR);
            fwrite(&buffer_lectura, sizeof(Usuario), 1, archivo);
            printf("El usuario ha sido eliminado exitosamente\n");
            stop();
            return;
        }
    }
    printf("No se encontro el usuario con el nombre dado\n");
    stop();
    return;
}