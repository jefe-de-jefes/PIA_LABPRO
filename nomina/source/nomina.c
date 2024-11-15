#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "nomina.h"
#include "array.h"
#include "auxiliares.h"
#include "misc.h"

const char* titulo_app= "SISTEMA FCFM";

static const char *texto_menu_proyectos =
    "Opciones de Proyecto:\n"
    "1- Registro\n"
    "2- Baja de Proyecto\n"
    "3- Regresar\n";

static const char *texto_menu_empleados =
    "Opciones de Empleado:\n"
    "1- Registro\n"
    "2- Baja de Empleado\n"
    "3- Regresar\n";

static const char *texto_menu_nomina =
    "Iniciando Registro de Nomina...\n";

static const char *texto_menu_reportes =
    "Impresion de Reportes\n"
    "1- Lista de Proyectos Activos\n"
    "2- Lista de Nomina\n"
    "3- Regresar\n";

static const char* texto_menu_debug =
    "Menu de Debugeado\n"
    "1-Imprimir Proyectos\n"
    "2-Imprimir Empleados\n"
    "3-Imprimir Nominas\n"
    "4-Salir\n ";


void login(AppData* appdata)
{
    Usuario buffer_datos;
    Usuario buffer_usuario;
    long buffer_pos;
    bool bloqueo=false;
    Array usuarios = array_init(sizeof(Usuario), 10, NULL, 0);
    Array posiciones = array_init(sizeof(long), 10, NULL, 0);

    FILE* archivo = fopen(ARCHIVO_USUARIOS, "rb+");
    if(archivo==NULL) {
        printf("ERROR: El archivo de usuarios '%s' no ha sido encontrado...\n", ARCHIVO_USUARIOS);
        exit(EXIT_FAILURE);
    }
    setbuf(archivo, NULL );

    //Ciclo de intentos de acceso;
    while(!bloqueo) {
        clear();
        printf("%s", titulo_app);
        printf("\n\n");
        printf("Usuario: "); leer_string(buffer_datos.nombre, L_NOMBRE);
        putchar('\n');
        printf("Contrasena: "); leer_string(buffer_datos.contrasena, L_CONTRASENA);
        putchar('\n');

        int indice = array_index(usuarios, &buffer_datos, comparar_nombre_usuarios);
        //Conseguimos los datos del usuario del archivo si no los tenemos
        if(!es_posicion_valida(indice)) {
            buffer_pos=encontrar_primero_en_archivo(&buffer_datos, &buffer_usuario,
                sizeof(Usuario), archivo, comparar_nombre_usuarios);
            if(es_posicion_valida(buffer_pos)&&buffer_usuario.estatus==ACTIVO) {
                array_push(posiciones, &buffer_pos);
                indice = array_get_longitud(usuarios);
                array_push(usuarios, &buffer_usuario);
            } else {
                printf("no se encontro el nombre de usuario dado...\n");
                stop();
                continue;
            }
        }
        //Comparamos contrasenas y actualizamos intentos/bloqueaos
        array_get(usuarios, indice, &buffer_usuario);
        array_get(posiciones, indice, &buffer_pos);
        if(!buffer_usuario.bloqueado) {
            if(strcmp(buffer_datos.contrasena, buffer_usuario.contrasena)==0) {
                strcpy(appdata->clave_autorizacion, buffer_usuario.contrasena);
                buffer_usuario.intentos=0;
                fseek(archivo, buffer_pos, SEEK_SET);
                fwrite(&buffer_usuario, sizeof(Usuario), 1, archivo);
                break;
            }
            buffer_usuario.intentos++;
            if(buffer_usuario.intentos >= LOGIN_MAX_INTENTOS) {
                buffer_usuario.bloqueado=true;
                bloqueo=true;
            }
            fseek(archivo, buffer_pos, SEEK_SET);
            fwrite(&buffer_usuario, sizeof(Usuario), 1, archivo);
            array_set(usuarios, indice, &buffer_usuario);
        }
        else bloqueo=true;
    }

    array_destroy(usuarios);
    array_destroy(posiciones);
    fclose(archivo);
    if(bloqueo) {
        printf("Este usuario ha sido bloqueado, favor de contactar con un administrador...\n");
        stop();
        exit(EXIT_FAILURE);
    }
    return;
}


void cargar_datos(AppData* appdata)
{
    appdata->archivo_proyectos=abrir_archivo_binario(ARCHIVO_PROYECTOS);
    setbuf(appdata->archivo_proyectos, NULL );
    appdata->archivo_empleados=abrir_archivo_binario(ARCHIVO_EMPLEADOS);
    setbuf(appdata->archivo_empleados, NULL);
    appdata->archivo_nominas=abrir_archivo_binario(ARCHIVO_NOMINAS);
    setbuf(appdata->archivo_nominas, NULL);
    return;
}


void submenu_proyectos(AppData* appdata)
{
    int opc=0;
    while(opc!=3){
        opc=menu(texto_menu_proyectos, 1, 3);
        switch(opc){
        case 1:
        /*Registrar proyectos*/
            while(true){
                if(registrar_proyecto(appdata->archivo_proyectos)==1){
                    printf("\n**Proyecto registrado exitosamente**\n");
                }else printf("\n**Error al registrar el proyecto**\n");
                printf("\nDesea registrar otro proyecto? 1-Si | 2-No\n");
                if(validar_entero(1,2)==2) break;
            };
            stop();
            break;
        case 2:
            clear();
            printf("\n**BAJA DE PROYECTO**\n");
            baja_proyecto(appdata->archivo_proyectos, appdata->archivo_empleados);
            break;
        }
    }
    return;
}


void submenu_empleados(AppData* appdata) {
    int opc=0;
    while(opc!=3){
        opc = menu(texto_menu_empleados, 1, 3);
        switch(opc){
        case 1:
            registro_empleado(appdata);
            break;
        case 2:
            baja_empleado(appdata);
            break;
        }
    }
    return;
}


void submenu_nomina(AppData* appdata)
{
    clear();
    long pos_proyecto;
    Proyecto buffer_datos;
    Proyecto buffer_proyecto;
    printf("Introduzca la clave del proyecto con el cual asociar la nomina\n");
    leer_string(buffer_datos.clave_proyecto, L_CLAVE);
    buffer_datos.estatus=ACTIVO;
    pos_proyecto=encontrar_primero_en_archivo(&buffer_datos, &buffer_proyecto,
        sizeof(Proyecto), appdata->archivo_proyectos, comparar_clave_proyectos);
    if(!es_posicion_valida(pos_proyecto) || buffer_proyecto.estatus!=ACTIVO) {
        printf("No se ha encontrado ninguna nomina con la clave dada\n");
        printf("*Regresando al menu principal*\n");
        stop();
        return;
    }

    Array nominas = array_init(sizeof(DatosNomina), 10, NULL, 0);
    cargar_empleados_proyecto(appdata, nominas, &buffer_proyecto);
    if(array_get_longitud(nominas)==0) {
        printf("El proyecto proporcionado no cuenta con ningun empleado\n");
        printf("*Regresando al menu principal*\n");
        stop();
        return;
    }

    Fecha fecha_nomina = {.dia=0};
    printf("Ingrese el mes de la nomina a registrar como un entero: ");
    fecha_nomina.mes=validar_entero(MIN_M, MAX_M);
    printf("Ingrese el ano de la nomina a registrar como un entero: ");
    fecha_nomina.ano=validar_entero(MIN_A, MAX_A);
    if(!fecha_dentro_de_periodo(&fecha_nomina, &buffer_proyecto)) {
        static const char *texto_nomina_fuera_de_fecha =
        "La fecha especificada se encuentra fuera del\n"
        "periodo activo del proyecto especificado\n"
        "Desea proceder con la captura de nomina?\n"
        "1-Si\n"
        "2-No\n";
        printf("%s", texto_nomina_fuera_de_fecha);
        int opc = validar_entero(1,2);
        if(opc==2) return;
        char clave_autorizacion[L_CONTRASENA];
        printf("Clave de Autorizacion: ");
        leer_string(clave_autorizacion, L_CONTRASENA);
        if(strcmp(clave_autorizacion, appdata->clave_autorizacion)!=0) {
            printf("Autentificacion fallida...\n");
            printf("*Regresando al menu principal*\n");
            stop();
            return;
        }
    }

    cargar_nominas_empleados(appdata, nominas, &fecha_nomina, &buffer_proyecto);
    //print_arreglo_debug(nominas, "Arreglo recien cargado", print_datos_debug);

    DatosNomina tipo_nomina;
    bool terminar_captura=false;

    tipo_nomina.empleado.estatus=ACTIVO;
    tipo_nomina.pos=-1;
    if(array_existe_elemento(nominas,&tipo_nomina, comparar_tipo_nomina)) {
        printf("Registando nominas de empleados activos\n");
        capturar_nominas(nominas, &buffer_proyecto, false,
            &tipo_nomina, comparar_tipo_nomina);
    }

    tipo_nomina.empleado.estatus=INACTIVO;
    tipo_nomina.pos=-1;
    if(!terminar_captura&&array_existe_elemento(nominas, &tipo_nomina, comparar_tipo_nomina)) {
        printf("Desea capturar en esta nomina algun empleado inactivo? 1-Si | 2-No\n");
        if(validar_entero(1,2)==1)
        terminar_captura=capturar_nominas(nominas, &buffer_proyecto, true,
            &tipo_nomina, comparar_tipo_nomina);
    }

    tipo_nomina.empleado.estatus=ACTIVO;
    tipo_nomina.pos=0;
    if(!terminar_captura&&array_existe_elemento(nominas, &tipo_nomina, comparar_tipo_nomina)) {
        printf("Desea sobreescribir la nomina de algun empleado que ya fue registrado? 1-Si | 2-No\n");
        if(validar_entero(1,2)==1)
        terminar_captura=capturar_nominas(nominas, &buffer_proyecto, true,
            &tipo_nomina, comparar_tipo_nomina);
    }

    tipo_nomina.empleado.estatus=INACTIVO;
    tipo_nomina.pos=0;
    if(!terminar_captura&&array_existe_elemento(nominas, &tipo_nomina, comparar_tipo_nomina)) {
        printf("Desea sobreescribir la nomina algun empleado inactivo que ya fue registrado? 1-Si | 2-No\n");
        if(validar_entero(1,2)==1)
        terminar_captura=capturar_nominas(nominas, &buffer_proyecto, true,
            &tipo_nomina, comparar_tipo_nomina);
    }
    //print_arreglo_debug(nominas, "Arreglo final de nominas", print_datos_debug);
    printf("Fin de la captura de la nomina del proyecto %s, de %s de %d\n",
        buffer_proyecto.clave_proyecto, get_string_mes(fecha_nomina.mes), fecha_nomina.ano);
    printf("Guardando los cambios realizados a la nomina\n");
    guardar_cambios_nominas(appdata, nominas);
    array_destroy(nominas);
    fseek(appdata->archivo_proyectos, pos_proyecto, SEEK_SET);
    fwrite(&buffer_proyecto, sizeof(Proyecto), 1, appdata->archivo_proyectos);
    printf("*Regresando al menu principal*");
    stop();
    return;
}


void submenu_reportes(AppData* appdata)
{
    Proyecto proyecto;
    Nomina nomina;
    int opc=0;
    while(opc!=3){
        opc = menu(texto_menu_reportes, 1, 3);
        switch(opc){
        case 1:
            reporte_proyectos(appdata);
            break;
        case 2:
            reporte_nominas(appdata);
            break;
        }
    }
    return;
}

void submenu_debug(AppData* appdata)
{
    int opc=0;
    while(opc!=4) {
        opc=menu(texto_menu_debug, 1,4);
        clear();
        switch(opc) {
        case 1:
            print_all_debug(sizeof(Proyecto), appdata->archivo_proyectos, print_proyecto_debug);
            break;
        case 2:
            print_all_debug(sizeof(Empleado), appdata->archivo_empleados, print_empleado_debug);
            break;
        case 3:
            print_all_debug(sizeof(Nomina), appdata->archivo_nominas, print_nomina_debug);
            break;
        }
    }
}

void liberar_memoria(AppData* appdata)
{
    fclose(appdata->archivo_proyectos);
    fclose(appdata->archivo_empleados);
    fclose(appdata->archivo_nominas);
    return;
}

