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


void login(AppData* appdata)
{
    Usuario buffer_datos = {.estatus=ACTIVO};
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
            if(es_posicion_valida(buffer_pos)) {
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
    appdata->archivo_empleados=abrir_archivo_binario(ARCHIVO_EMPLEADOS);
    appdata->archivo_nominas=abrir_archivo_binario(ARCHIVO_NOMINAS);
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
        printf("\n**MENU DE EMPLEADOS**\n");
        opc = menu(texto_menu_empleados, 1, 3);
        switch(opc){
        case 1:
            registro_empleado(appdata);
            break;
        case 2:
            baja_empleado(appdata);
            break;
        default:
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
    //Cargamos el proyecto correspondiente a la nomina
    printf("Introduzca la clave del proyecto con el cual asociar la nomina\n");
    leer_string(buffer_datos.clave_proyecto, L_CLAVE);
    buffer_datos.estatus=ACTIVO;
    pos_proyecto=encontrar_primero_en_archivo(&buffer_datos, &buffer_proyecto,
        sizeof(Proyecto), appdata->archivo_proyectos, comparar_clave_proyectos);
    if(!es_posicion_valida(pos_proyecto)) {
        printf("No se ha encontrado ninguna nomina con la clave dada...\n");
        stop();
        return;
    }

    Array empleados = array_init(sizeof(Empleado), 10, NULL, 0);
    Empleado buffer_empleado;
    //Cargamos empleados relacionados al proyecto
    rewind(appdata->archivo_empleados);
    while(encontrar_siguiente_en_archivo(&buffer_proyecto, &buffer_empleado, sizeof(Empleado),
        appdata->archivo_empleados, comparar_empleado_en_proyecto) != -1
    ) array_push(empleados, &buffer_empleado);
    if(array_get_longitud(empleados)==0) {
        printf("El proyecto no cuenta con empleados registrados\n");
        stop();
        return;
    }

    //Cargamos la fecha correspondiente a la nomina
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
        printf(texto_nomina_fuera_de_fecha);
        int opc = validar_entero(1,2);
        if(opc==2) return;
        char clave_autorizacion[L_CONTRASENA];
        printf("Clave de Autorizacion: ");
        leer_string(clave_autorizacion, L_CONTRASENA);
        if(strcmp(clave_autorizacion, appdata->clave_autorizacion)!=0) {
            printf("Autentificacion fallida...\n");
            stop();
            return;
        }
    }

    Array nominas = array_init(sizeof(Nomina), 10, NULL, 0);
    Array pos_nominas = array_init(sizeof(long), 10, NULL, 0);
    Nomina buffer_nomina;
    long buffer_pos;
    //Cargamos las nominas correpondientes al proyecto y la fecha
    rewind(appdata->archivo_nominas);
    while(
        (buffer_pos=encontrar_siguiente_en_archivo(&buffer_proyecto, &buffer_nomina,
        sizeof(Nomina), appdata->archivo_nominas, comparar_nomina_en_proyecto)) != -1
    ) {
        if(buffer_nomina.mes==fecha_nomina.mes && buffer_nomina.ano==fecha_nomina.ano) {
            array_push(nominas, &buffer_nomina);
            array_push(pos_nominas, &buffer_pos);
        }
    }

    //No se ha registrado la nomina previamente
    if(array_get_longitud(nominas)==0) {
        printf("Iniciando proceso de registro de nomina:\n\n");
    }
    //La nomina ya ha sido registrada completamente anteriormente
    else if(array_get_longitud(empleados)==array_get_longitud(nominas)) {
        static const char *texto_nomina_ya_registrada =
        "Se han encontrado valores ya registrados para las\n"
        "nominas de este proyecto en este preriodo\n"
        "Desea proceder con el riesgo de sobreescribir las\n"
        "nominas que ya fueron anteriormente registradas?\n"
        "1-Si\n"
        "2-No\n";
        printf(texto_nomina_ya_registrada);
        putchar('\n');
        int opc=validar_entero(1,2);
        if(opc==2) return;
    }
    //Se han anadido empleados al proyecto que no cuentan con su nomina
    else if(array_get_longitud(empleados)>array_get_longitud(nominas)) {
        static const char *texto_nomina_desfazada =
        "Se ha encontrado empleados asociados al proyecto que\n"
        "no cuentan con una nomina en este periodo\n"
        "Desea volver a registrar la nomina tomando en cuenta estos empleados?\n"
        "1-Si\n"
        "2-No\n";
        printf(texto_nomina_desfazada);
        putchar('\n');
        int opc=validar_entero(1,2);
        if(opc==2) return;
    }

    empleados=emparejar_empleados(nominas, empleados);

    static const char* texto_sobrepaso_de_presupuesto =
    "El total de nomina ($%d) ha sobrepasado el monto inicial\n"
    "del proyecto ($%d). Esta seguro que desea continuar con la captura?\n"
    "1-Si\n"
    "2-No\n";

    //Agregar nominas faltantes
    for(int i=array_get_longitud(pos_nominas); i<array_get_longitud(empleados); i++) {
        array_get(empleados, i, &buffer_empleado);
        buffer_nomina.estatus=ACTIVO;
        buffer_nomina.ano=fecha_nomina.ano;
        buffer_nomina.mes=fecha_nomina.mes;
        strcpy(buffer_nomina.clave_proyecto, buffer_proyecto.clave_proyecto);
        strcpy(buffer_nomina.curp_empleado, buffer_empleado.curp);
        printf("No. empleado: %d | %s\n", buffer_empleado.num_empleado, buffer_empleado.nombre);
        printf("Horas trabajadas:\n"); buffer_nomina.horas_trabajadas=
            validar_entero(H_TRABAJADAS_MIN, H_TRABAJADAS_MAX);
        buffer_nomina.sueldo_mensual = (float)buffer_empleado.tarifa_mensual*
            ((float)buffer_nomina.horas_trabajadas/HORAS_MENSUALES_MINIMAS);
        buffer_proyecto.total_nomina+=buffer_nomina.sueldo_mensual;
        if(buffer_proyecto.total_nomina>buffer_proyecto.monto_inicial) {
            printf(texto_sobrepaso_de_presupuesto,
            buffer_proyecto.total_nomina, buffer_proyecto.monto_inicial);
            int opc=validar_entero(1,2);
            if(opc==2) return;
        }
        array_push(nominas,&buffer_nomina);
        putchar('\n');
    }

    //Modificar nominas ya existentes
    for(int i=0; i<array_get_longitud(pos_nominas); i++) {
        array_get(empleados, i, &buffer_empleado);
        array_get(nominas, i, &buffer_nomina);
        printf("No. empleado: %d | %s\n", buffer_empleado.num_empleado, buffer_empleado.nombre);
        static const char *texto_nomina_preexistente =
        "Este empleado ya cuenta con una nomina registrada. Que es lo que desea hacer?\n"
        "1-Sobreescribir Nomina\n"
        "2-Continuar con el siguiente empleado\n"
        "3-Terminar captura de nomina\n";
        printf(texto_nomina_preexistente);
        int opc=validar_entero(1,3);
        if(opc==2) continue;
        else if(opc==3) break;
        printf("Horas trabajadas:\n"); buffer_nomina.horas_trabajadas=
            validar_entero(H_TRABAJADAS_MIN, H_TRABAJADAS_MAX);
        buffer_proyecto.total_nomina-=buffer_nomina.sueldo_mensual;
        buffer_nomina.sueldo_mensual = (float)buffer_empleado.tarifa_mensual*
            ((float)buffer_nomina.horas_trabajadas/HORAS_MENSUALES_MINIMAS);
        buffer_proyecto.total_nomina+=buffer_nomina.sueldo_mensual;
        if(buffer_proyecto.total_nomina>buffer_proyecto.monto_inicial) {
            printf(texto_sobrepaso_de_presupuesto,
            buffer_proyecto.total_nomina, buffer_proyecto.monto_inicial);
            int opc=validar_entero(1,2);
            if(opc==2) return;
        }
        array_set(nominas, i, &buffer_nomina);
        putchar('\n');
    }

    fseek(appdata->archivo_proyectos, pos_proyecto, SEEK_SET);
    fwrite(&buffer_proyecto, sizeof(Proyecto), 1, appdata->archivo_proyectos);
    actualizar_nominas(appdata->archivo_nominas, nominas, pos_nominas);
    array_destroy(empleados);
    array_destroy(nominas);
    array_destroy(pos_nominas);
    printf("La nomina correspondiente al proyecto '%s'\n"
        "en el mes %d de %d ha sido actualizada correctamente\n",
        buffer_proyecto.nombre, fecha_nomina.mes, fecha_nomina.ano);
    stop();
    return;
}


void submenu_reportes(AppData* appdata)
{
    int op;
    do{
        op=menu(texto_menu_reportes, 1, 3);
        switch (op)
        {
        case 1:
            reporte_proyectos(appdata);
            break;
        
        case 2:
            break;
        default: break;
        }
    }while(op!=3);
    return;
}

void submenu_debug(AppData* appdata)
{
    clear();
    static const char* texto_menu_debug =
    "Menu de Debugeado\n"
    "1-Imprimir Proyectos\n"
    "2-Imprimir Empleados\n"
    "3-Imprimir Nominas\n"
    "4-Salir\n ";
    int opc=0;
    while(opc!=4) {
        opc=menu(texto_menu_debug, 1,4);
        switch(opc) {
        case 1:
            clear();
            print_all_debug(sizeof(Proyecto), appdata->archivo_proyectos, print_proyecto_debug);
            break;
        case 2:
            clear();
            print_all_debug(sizeof(Empleado), appdata->archivo_empleados, print_empleado_debug);
            break;
        case 3:
            clear();
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

