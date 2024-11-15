#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "nomina.h"

void flush(void)
{
    int c;
    do c=getchar(); while(c!='\n' && c!=EOF);
    return;
}

void stop(void)
{
    scanf("%*c");
    flush();
    return;
}

void leer_string(char* buffer, size_t longitud)
{
    int c, i;
    while(isspace(c=getchar()));
    *(buffer)=c;
    for(i=1; i<longitud; i++) {
        c=getchar();
        if(c=='\n') break;
        *(buffer+i)=c;
    }
    *(buffer+i)='\0';
    return;
}

int validar_entero(int min, int max) {
    int valor, scan_ok;
    do {
        printf("... ");
        scan_ok=scanf("%d", &valor);
        if(!scan_ok) flush();
    } while(valor < min || valor > max);
    return valor;
}

float validar_flotante(float min, float max) {
    float valor;
    int scan_ok;
    do {
        printf("... ");
        scan_ok=scanf("%f", &valor);
        if(!scan_ok) flush();
    } while(scan_ok != 1 || valor < min || valor > max);
    return valor;
}

FILE* abrir_archivo_binario(const char* nombre_archivo)
{
    static const char *texto_archivo_no_encontrado =
    "No se encontro el archivo '%s' en la misma direccion que el ejecutable.\n"
    "Desea crear un nuevo archivo sobreescribiendo el archivo en caso de que exista?\n"
    "1-Si\n"
    "2-No\n";

    static const char *texto_archivo_es_necesario =
    "Se necesita el archivo '%s' para proceder con la ejecucion de la aplicacion.\n"
    "Terminano la ejecucion del programa...\n";

    static const char *texto_archivo_error_al_crear =
    "Se ha producido un error al intentar crear el archivo '%s'...\n";

    int opc;
    FILE* archivo=fopen(nombre_archivo, "rb+");
    if(!archivo) {
        //Crear nuevo archivo? 1-si 2-no
        printf(texto_archivo_no_encontrado, nombre_archivo);
        opc=validar_entero(1,2);
        if(opc==1) {
            archivo=fopen(nombre_archivo, "wb+");
            if(!archivo) {
                printf(texto_archivo_error_al_crear, archivo);
                printf(texto_archivo_es_necesario, archivo);
                exit(EXIT_FAILURE);
            }
        }
        else {
            printf(texto_archivo_es_necesario, nombre_archivo);
            exit(1);
        }
    }
    return archivo;
}

extern const char* titulo_app;
int menu(const char* msg, int min, int max) {
    clear();
    printf("%s", titulo_app);
    printf("\n\n");
    printf("%s", msg);
    int opc=validar_entero(min, max);
    putchar('\n');
    return opc;
}

long encontrar_primero_en_archivo(void *buffer_entrada, void *buffer_salida,
    size_t tamano_elemento, FILE* archivo, int (*comparar)(void*, void*))
{
    rewind(archivo);
    while(fread(buffer_salida, tamano_elemento, 1, archivo)) {
        if(comparar(buffer_entrada, buffer_salida)) return ftell(archivo)-tamano_elemento;
    }
    return -1;
}

long encontrar_siguiente_en_archivo(void *buffer_entrada, void *buffer_salida,
    size_t tamano_elemento, FILE* archivo, int (*comparar)(void*, void*))
{
    while(fread(buffer_salida, tamano_elemento, 1, archivo)) {
        if(comparar(buffer_entrada, buffer_salida)) return ftell(archivo)-tamano_elemento;
    }
    return -1;
}

bool existe_en_archivo(void* buffer_entrada, size_t tamano_elemento,
    FILE* archivo, int (*comparar)(void*, void*))
{
    byte buffer_lectura[tamano_elemento];
    rewind(archivo);
    while(fread(&buffer_lectura, tamano_elemento, 1, archivo)) {
        if(comparar(buffer_entrada, buffer_lectura)) return true;
    }
    return false;
}