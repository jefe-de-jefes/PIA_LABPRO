#include <stdio.h>
#include <string.h>
#include "nomina.h"
#include "array.h"
#include "auxiliares.h"
#include "misc.h"


const char* get_string_perfil(Perfil perfil) {
    static const char *strings_perfiles[] = {"LIDER", "ADMINISTRADOR BD",
    "ANALISTA", "PROGRAMADOR", "TESTER", "NULL"};
    if(perfil>0&&perfil<6) return strings_perfiles[perfil-1];
    return strings_perfiles[5];
}

const char* get_string_estatus(Estatus estatus) {
    static const char *strings_estatus[] = {"Inactivo", "Activo", "Basura", "NULL"};
    if(estatus>=0&&estatus<=2) return strings_estatus[estatus];
    return strings_estatus[3];
}


int comparar_nombre_usuarios(void* ptr1, void* ptr2)
{
    Usuario *usuario1=(Usuario*)ptr1, *usuario2=(Usuario*)ptr2;
    if(strcmp(usuario1->nombre, usuario2->nombre)==0)
        return true;
    return false;
}


int existe_clave_proyecto(char clave[L_CLAVE+1], FILE* archivo)
{
    Proyecto proyecto;
	rewind(archivo);
    while(fread(&proyecto, sizeof(proyecto), 1, archivo)){
        if((strcmp(proyecto.clave_proyecto, clave)==0) && proyecto.estatus==ACTIVO){
            return 1;
        }
    }
    return 0;
}

int registrar_proyecto(FILE* archivo_proyectos)
{
    Proyecto proy;
    int clave_tomada=0;
    clear();
    printf("\n**REGISTRO DE PROYECTO**\n");

    do{
        printf("\nIntroudzca la clave del proyecto: ");
        leer_string(proy.clave_proyecto, L_CLAVE);
        clave_tomada=existe_clave_proyecto(proy.clave_proyecto, archivo_proyectos);
        if(clave_tomada)
            printf("\n**El proyecto ya existe. Ingrese otra clave para registrar**\n");
    }while(clave_tomada);

    printf("\nIntroduzca el nombre del proyecto: ");
    leer_string(proy.nombre, L_NOMBRE);

    printf("\nIntroduzca el monto inicial del proyecto: ");
    proy.monto_inicial=validar_flotante(0.0, 99999999);

    proy.total_nomina=0;

    printf("\n***FECHA INICIO***\n");
    printf("\nIntroduzca el ano de inicio del proyecto (aaaa): ");
    proy.inicio.ano=validar_entero(MIN_A, MAX_A);
    printf("\nIntroduzca el mes de inicio del proyecto(mm): ");
    proy.inicio.mes=validar_entero(MIN_M, MAX_M);
    printf("\nIntroduzca el dia de inicio del proyecto (dd): ");
    proy.inicio.dia=validar_entero(1, 31);

    printf("\n***FECHA TERMINO***\n");
    printf("\nIntroduzca el ano de termino del proyecto (aaaa): ");
    proy.termino.ano=validar_entero(0, 9999);
    printf("\nIntroduzca el mes de termino del proyecto (mm): ");
    proy.termino.mes=validar_entero(1, 12);
    printf("\nIntroduzca el dia de termino del proyecto (dd): ");
    proy.termino.dia=validar_entero(1, 31);
    printf("\nIntroduzca el status [1-Activo | 0-Inactivo]: ");
    proy.estatus=(Estatus)validar_entero(0, 1);

    fseek(archivo_proyectos, 0, SEEK_END);
    return fwrite(&proy, sizeof(Proyecto), 1, archivo_proyectos);
}

void imprimir_proyecto(Proyecto* proy)
{
    int i;
    for(i=0;i<129;i++)printf("*");
    printf("\n%20s | %10s | %10s | %20s | %20s | %20s | %10s*",
        "Clave Proyecto", "Nombre", "Monto", "Total nomina", "Fecha inicio", "Fecha termino", "Estatus");
    printf("\n%20s | %10s | %10.3f | %20.2f | %12i/%02i/%04i | %12i/%02i/%04i | %10s*\n",
    proy->clave_proyecto, proy->nombre, proy->monto_inicial, proy->total_nomina,
    proy->inicio.dia, proy->inicio.mes, proy->inicio.ano, proy->termino.dia,
    proy->termino.mes, proy->termino.ano, get_string_estatus(proy->estatus));
    for(i=0;i<129;i++)printf("*");
    printf("\n");
}

void imprimir_empleado(Empleado *emp){
    printf("\n%20s | %20s | %20s | %20s | %20s | %20s | %10s | %20s | %10s",
           "Clave Proyecto", "Numero de empleado", "Nombre", "CURP",
           "Fecha de nacimiento", "Correo", "Telefono", "Perfil", "Tarifa Mensual");
    printf("\n%20s | %20d | %20s | %20s | %02d/%02d/%04d \t\t | %20s | %10s | %20s | %10.2f\n",
           emp->clave_proyecto, emp->num_empleado, emp->nombre, emp->curp,
           emp->nacimiento.dia, emp->nacimiento.mes, emp->nacimiento.ano,
           emp->correo, emp->telefono, get_string_perfil(emp->perfil), emp->tarifa_mensual);
    printf("\n%20s | %20s | %20s | %20s | %20s | %20s",
           "Calle", "Colonia", "Numero", "Codigo Postal", "Municipio", "Estatus");
    printf("\n%20s | %20s | %20u | %20s | %20d | %20d\n",
           emp->direccion.calle, emp->direccion.colonia, emp->direccion.numero,
           emp->direccion.municipio, emp->direccion.cp, emp->estatus);
}

void baja_empleados_proyecto(char clave[L_CLAVE+1], FILE* archivo_empleados){
    Empleado empleado;
    rewind(archivo_empleados);
    while (fread(&empleado, sizeof(Empleado), 1, archivo_empleados))
    {
        if(strcmp(clave, empleado.clave_proyecto)==0 && empleado.estatus==ACTIVO){
            empleado.estatus=INACTIVO;
            fseek(archivo_empleados, ftell(archivo_empleados)-sizeof(Empleado), SEEK_SET);
            fwrite(&empleado, sizeof(Empleado), 1, archivo_empleados);
            printf("\n**Empleado #%i dado de baja**", empleado.num_empleado);
        }
    }
    return;
}

void baja_proyecto(FILE* archivo_proyectos, FILE* archivo_empleados){
    Proyecto proy;
    char nom[L_NOMBRE+1];

    printf("\nIntroduzca el nombre del proyecto\n");
    leer_string(nom, L_NOMBRE);
    printf("\nDando de baja el proyecto...\n");

    rewind(archivo_proyectos);
    while (fread(&proy, sizeof(Proyecto), 1, archivo_proyectos)){
        if (strcmp(nom, proy.nombre)==0 && proy.estatus==ACTIVO)
        {
                fseek(archivo_proyectos, -(long)sizeof(Proyecto), SEEK_CUR);
                puts("\n**Proyecto encontrado**\n");
                imprimir_proyecto(&proy);
                proy.estatus=INACTIVO;

                if(fwrite(&proy, sizeof(Proyecto), 1, archivo_proyectos)==1){
                    printf("\nProyecto actualizado:\n");
                    imprimir_proyecto(&proy);
                    printf("\n**Proyecto dado de baja exitosamente**\n");
                    baja_empleados_proyecto(proy.clave_proyecto, archivo_empleados);
                }else
                {
                    printf("\nError al actualizar el archivo");
                }
            stop();
            return;
        }
    }
    printf("\nNo se encontro el proyecto ingresado\n");
    stop();
    return;
}


void validar_clave_proyecto(AppData* appdata, Empleado* reg_empleado){
    Proyecto proy;
    while(true){
        printf("Introduzca La clave del proyecto\n");
        leer_string(reg_empleado->clave_proyecto, L_CLAVE);
        rewind(appdata->archivo_proyectos);
        while(fread(&proy, sizeof(Proyecto), 1, appdata->archivo_proyectos)){
            if((strcmp(proy.clave_proyecto, reg_empleado->clave_proyecto) == 0)
                &&proy.estatus==ACTIVO
            ) return;
        }
        printf("La clave del proyecto no esta registrada\n");
    }
}


void validar_curp_empleado(AppData* appdata, Empleado* empleado){
    bool curp_repetido;
    Empleado buffer_lectura;
    while(true){
        curp_repetido=false;
        printf("Introduzca la curp del empleado\n");
        leer_string(empleado->curp, L_CURP);
        rewind(appdata->archivo_empleados);
        while (fread(&buffer_lectura, sizeof(Empleado), 1, appdata->archivo_empleados)){
            if (strcmp(empleado->curp, buffer_lectura.curp) == 0
                && buffer_lectura.estatus==ACTIVO)
            {
                printf("La CURP de este empleado ya ha sido registrada\n");
                curp_repetido=true;
                break;
            }
        }
        if(!curp_repetido) return;
    }
}


int get_ultimo_num_empleado(FILE* archivo)
{
    Empleado buffer_empleado;
    rewind(archivo);
    fseek(archivo, 0, SEEK_END);
    if(ftell(archivo)==0) return 0;
    else{
        rewind(archivo);
        fseek(archivo, -(long)sizeof(Empleado), SEEK_END);
        fread(&buffer_empleado, sizeof(Empleado), 1, archivo);
        return buffer_empleado.num_empleado;
    }
}


void registro_empleado(AppData* appdata){
    Empleado reg_empleado;
    reg_empleado.num_empleado=get_ultimo_num_empleado(appdata->archivo_empleados);
    while(true){
        clear();
        printf("\n**REGISTRO DE EMPLEADO**\n");
        reg_empleado.num_empleado++;
        validar_clave_proyecto(appdata, &reg_empleado);
        printf("Introduzca el nombre del empleado\n");
        leer_string(reg_empleado.nombre, L_NOMBRE);
        validar_curp_empleado(appdata, &reg_empleado);
        printf("Introduzca el dia de nacimiento del empleado\n");
        reg_empleado.nacimiento.dia = validar_entero(1, 31);
        printf("Introduzca el mes de nacimiento del empleado ( 1 - 12 )\n");
        reg_empleado.nacimiento.mes = validar_entero(1, 12);
        printf("Introduzca el ano de nacimiento del empleado\n");
        reg_empleado.nacimiento.ano = validar_entero(1900, 3000);
        printf("Introduzca el correo electronico del empleado\n");
        leer_string(reg_empleado.correo, L_CORREO);

        while(true){
            printf("Introduzca su numero de telefono\n");
            leer_string(reg_empleado.telefono, L_TELEFONO);
            if(strlen(reg_empleado.telefono) != 10){
                printf("El numero de telefono debe ser de 10 digitos\n");
            }else break;
        }

        static const char* texto_seleccion_perfil=
            "Ingrese el perfil del empleado:\n"
            "1-Lider de proyecto\n"
            "2-Administrador de base de datos\n"
            "3-Analista\n"
            "4-Programador\n"
            "5-Tester\n";

        printf("%s", texto_seleccion_perfil);
        reg_empleado.perfil = validar_entero(1,5);

        while(true){
            printf("Introduzca la tarifa mensual del empleado\n");
            scanf("%f", &reg_empleado.tarifa_mensual);
            if(reg_empleado.tarifa_mensual < 0){
                printf("La tarifa mensual no puede ser menor a 0\n");
            }else break;
        }

        printf("Introduzca la calle del empleado\n");
        leer_string(reg_empleado.direccion.calle, L_NOMBRE);
        printf("Introduzca la colonia del empleado\n");
        leer_string(reg_empleado.direccion.colonia, L_NOMBRE);
        while(1){
            printf("Introduzca el numero de la casa del empleado\n");
            scanf("%d", &reg_empleado.direccion.numero);
            if(reg_empleado.direccion.numero < 0){
            printf("El numero no puede ser negativo\n");
            }else break;
        }
        printf("Introduzca el codigo postal del empleado\n");
        reg_empleado.direccion.cp = validar_entero(100000, 999999);
        printf("Introduzca el municipio del empleado\n");
        leer_string(reg_empleado.direccion.municipio, L_NOMBRE);
        reg_empleado.estatus = ACTIVO;

        rewind(appdata->archivo_empleados);
        fseek(appdata->archivo_empleados, 0, SEEK_END);
        fwrite(&reg_empleado, sizeof(Empleado), 1, appdata->archivo_empleados);

        static const char* texto_seguir_con_registo=
        "Que opcion desea?: \n"
        "1.- Agregar otro empleado\n"
        "2.- Salir al menu principal\n";

        printf("%s", texto_seguir_con_registo);
        int opc=validar_entero(1,2);
        if(opc==2) break;
    }
}


void baja_empleado(AppData * appdata){
    Empleado emp;
    char nombre[L_NOMBRE+1];
    int opc, empleado_encontrado=false;
    while(true){
        clear();
        printf("**BAJA DE EMPLEADO**\n");
        printf("Introduzca el nombre del empleado que desea dar de baja\n\n");
        leer_string(nombre, L_NOMBRE);
        rewind(appdata->archivo_empleados);
        while(fread(&emp, sizeof(Empleado), 1, appdata->archivo_empleados)){
            if((strcmp(nombre, emp.nombre) == 0) && emp.estatus == ACTIVO){
                imprimir_empleado(&emp);
                printf("\nDesea dar de baja a este empleado?\n1.- Si\n2.- No\n");
                opc = validar_entero(1,2);
                printf("\n");
                if(opc == 1){
                    emp.estatus = INACTIVO;
                    fseek(appdata->archivo_empleados, -(long)sizeof(Empleado), SEEK_CUR);
                    fwrite(&emp, sizeof(Empleado), 1, appdata->archivo_empleados);
                    printf("El empleado fue eliminado exitosamente\n\n");
                }
                empleado_encontrado = true;
            }
        }
        if(!empleado_encontrado) printf("\nNo se encontro un empleado con es nombre\n");
        printf("Desea dar de baja a otro empleado?\n1.- Si\n2.- Regresar al menu empleados\n");
        opc = validar_entero(1, 2);
        if(opc==2) break;
    }
}


bool fecha_dentro_de_periodo(Fecha *fecha, Proyecto *proyecto)
{
    long id_fecha = (MAX_M)*(fecha->ano-MIN_A)+(fecha->mes-MIN_M);
    long id_inicio = (MAX_M)*(proyecto->inicio.ano-MIN_A)+(proyecto->inicio.mes-MIN_M);
    long id_termino = (MAX_M)*(proyecto->termino.ano-MIN_A)+(proyecto->termino.mes-MIN_M);
    return (id_fecha >= id_inicio && id_fecha <= id_termino);
}

int comparar_clave_proyectos(void* ptr1, void* ptr2)
{
    Proyecto *proyecto1=(Proyecto*)ptr1, *proyecto2=(Proyecto*)ptr2;
    if(strcmp(proyecto1->clave_proyecto, proyecto2->clave_proyecto)==0)
        return true;
    return false;
}

int comparar_empleado_en_proyecto(void* ptr1, void* ptr2)
{
    Proyecto* proyecto = (Proyecto*)ptr1;
    Empleado* empleado = (Empleado*)ptr2;
    if(strcmp(proyecto->clave_proyecto, empleado->clave_proyecto)==0)
        return true;
    return false;
}

static int comparar_nomina_en_proyecto(void* ptr1, void* ptr2)
{
    Proyecto* proyecto = (Proyecto*)ptr1;
    Nomina* nomina = (Nomina*)ptr2;
    if(strcmp(proyecto->clave_proyecto, nomina->clave_proyecto)==0)
        return true;
    return false;
}

static int comparar_empleado_correspondiente_a_nomina(void* ptr1, void* ptr2)
{
    DatosNomina *datos1=(DatosNomina*)ptr1, *datos2=(DatosNomina*)ptr2;
    if(strcmp(datos1->nomina.curp_empleado, datos2->empleado.curp)==0)
        return true;
    return false;
}


void cargar_empleados_proyecto(AppData* appdata, Array nominas, Proyecto* proyecto)
{
    DatosNomina buffer_datos;
    rewind(appdata->archivo_empleados);
    while(encontrar_siguiente_en_archivo(proyecto, &buffer_datos.empleado,
        sizeof(Empleado), appdata->archivo_empleados, comparar_empleado_en_proyecto) !=-1)
    array_push(nominas, &buffer_datos);
    return;
}


void cargar_nominas_empleados(AppData* appdata, Array nominas, Fecha* fecha_nomina, Proyecto* proyecto)
{
    size_t indice;
    DatosNomina buffer_datos;
    DatosNomina buffer_auxiliar;

    bool indices_con_nomina[array_get_longitud(nominas)];
    for(int i=0; i<array_get_longitud(nominas); i++) indices_con_nomina[i]=false;

    rewind(appdata->archivo_nominas);
    while( (buffer_datos.pos = encontrar_siguiente_en_archivo(proyecto, &buffer_datos.nomina,
        sizeof(Nomina), appdata->archivo_nominas, comparar_nomina_en_proyecto)) != -1 )
    {
        if(buffer_datos.nomina.mes==fecha_nomina->mes&&buffer_datos.nomina.ano==fecha_nomina->ano){
            indice=array_index(nominas, &buffer_datos, comparar_empleado_correspondiente_a_nomina);
            indices_con_nomina[indice] = true;
            array_get(nominas, indice, &buffer_auxiliar);
            buffer_auxiliar.nomina=buffer_datos.nomina;
            buffer_auxiliar.pos=buffer_datos.pos;
            array_set(nominas, indice, &buffer_auxiliar);
        }
    }

    //print_arreglo_debug(nominas, "Arreglo despues de anadir nominas", print_datos_debug);

    for(int i=0; i<array_get_longitud(nominas); i++) {
        if(indices_con_nomina[i]==false) {
            array_get(nominas, i, &buffer_auxiliar);
            buffer_auxiliar.nomina.ano=fecha_nomina->ano;
            buffer_auxiliar.nomina.mes=fecha_nomina->mes;
            buffer_auxiliar.nomina.estatus=ACTIVO;
            buffer_auxiliar.nomina.horas_trabajadas=0;
            buffer_auxiliar.nomina.sueldo_mensual=0;
            strcpy(buffer_auxiliar.nomina.clave_proyecto, proyecto->clave_proyecto);
            strcpy(buffer_auxiliar.nomina.curp_empleado, buffer_auxiliar.empleado.curp);
            buffer_auxiliar.pos=-1;
            array_set(nominas, i, &buffer_auxiliar);
        }
    }
}

int comparar_tipo_nomina(void* ptr1, void* ptr2)
{
    DatosNomina *datos1=(DatosNomina*)ptr1, *datos2=(DatosNomina*)ptr2;
    if(datos1->empleado.estatus==datos2->empleado.estatus
        && es_posicion_valida(datos1->pos)==es_posicion_valida(datos2->pos))
        return true;
    return false;
}


int capturar_nominas(Array nominas, Proyecto* proyecto, bool interrumpible,
    DatosNomina* tipo_nomina, int(*comparar)(void*,void*))
{
    static const char* msg_interrupcion=
    "Que desea hacer?\n"
    "1-Capturar Empleado\n"
    "2-Saltar Empleado\n"
    "3-Termiar Captura\n";

    DatosNomina buffer_datos;
    for(int i=0; i<array_get_longitud(nominas); i++) {
        array_get(nominas, i, &buffer_datos);
        if(comparar(tipo_nomina ,&buffer_datos)) {
            printf("Empleado No.%d | %s | Estatus: %s\n", buffer_datos.empleado.num_empleado,
                buffer_datos.empleado.nombre, get_string_estatus(buffer_datos.empleado.estatus));
            if(interrumpible) {
                printf("%s", msg_interrupcion);
                int opc=validar_entero(1,3);
                if(opc==2) continue;
                if(opc==3) return true;
            }
            printf("Horas Trabajadas: ");
            buffer_datos.nomina.horas_trabajadas = validar_entero(0, 1000);
            proyecto->total_nomina-=buffer_datos.nomina.sueldo_mensual;
            buffer_datos.nomina.sueldo_mensual =
                (float)buffer_datos.empleado.tarifa_mensual*
                ((float)buffer_datos.nomina.horas_trabajadas/
                (float)HORAS_MENSUALES_MINIMAS);
            proyecto->total_nomina+=buffer_datos.nomina.sueldo_mensual;
            array_set(nominas, i,&buffer_datos);
            putchar('\n');
        }
    }
    return false;
}


void guardar_cambios_nominas(AppData* appdata, Array nominas)
{
    DatosNomina buffer_datos;
    for(int i=0; i<array_get_longitud(nominas); i++) {
        array_get(nominas, i, &buffer_datos);
        if(es_posicion_valida(buffer_datos.pos))
            fseek(appdata->archivo_nominas, buffer_datos.pos, SEEK_SET);
        else fseek(appdata->archivo_nominas, 0, SEEK_END);
        fwrite(&buffer_datos.nomina, sizeof(Nomina), 1, appdata->archivo_nominas);
    }
    return;
}


void imprimir_empleado_reporte(Empleado emp)
{
    char array[5][15]={"LIDER_PROYECTO", "ADMIN_BD", "ANALISTA", "PROGRAMDOR", "TESTER"};
    printf("\n%15i | %14s | %20s |  %6i/%02i/%04i | %15s | $%8.2f", emp.num_empleado, emp.nombre, emp.curp, emp.nacimiento.dia,
    emp.nacimiento.mes, emp.nacimiento.ano, array[emp.perfil-1], emp.tarifa_mensual);
}


void reporte_proyectos(AppData* appdata)
{
    Nomina nomina;
    Empleado empleado;
    Proyecto proyecto;
    char clave_consultar[L_CLAVE+1];
    float suma_nomias=0;

    clear();
    puts("**REPORTE DE PROYECTOS ACTIVOS**\n");
    bool existe_proyecto = true;
    do {
        if (!existe_proyecto) printf("\n**El proyecto no existe.**\n");
        printf("\nIntroduzca la clave del proyecto a consultar: ");
        leer_string(clave_consultar, L_CLAVE);
        existe_proyecto=existe_clave_proyecto(clave_consultar, appdata->archivo_proyectos);
    }while(!existe_proyecto);

    rewind(appdata->archivo_proyectos);
    while (fread(&proyecto, sizeof(Proyecto), 1, appdata->archivo_proyectos)) {
        if((strcmp(proyecto.clave_proyecto, clave_consultar)==0)
            && proyecto.estatus==ACTIVO
        ) break;
    }

    clear();
    printf("**Proyecto**: %s %s\n", clave_consultar, proyecto.nombre);
    printf("\n%s | %14s | %20s | %15s | %15s | %10s\n","**No empleado**",
    "**Nombre**","**CURP**", "**Fecha Nac**", "**Perfil**", "**Tarifa**");

    bool proyecto_contiene_empelados=false;
    rewind(appdata->archivo_empleados);
    while(fread(&empleado, sizeof(Empleado), 1, appdata->archivo_empleados)) {
        if ((strcmp(empleado.clave_proyecto, clave_consultar)==0)
            && empleado.estatus==ACTIVO)
        {
            proyecto_contiene_empelados=true;
            imprimir_empleado_reporte(empleado);
        }
    }
    if(!proyecto_contiene_empelados)
    {
        puts("\nNo hay ningun empleado registrado en la nomina de ese proyecto\n");
        stop();
        return;
    }

    printf("\n\nMonto inicial del proyecto: $%.2f\n", proyecto.monto_inicial);
    rewind(appdata->archivo_nominas);
    printf("\n%s %s  %s", "Mes", "Ano", "Total Nomina");
    while(fread(&nomina, sizeof(Nomina), 1, appdata->archivo_nominas)) {
        if (strcmp(nomina.clave_proyecto, clave_consultar)==0) {
            printf("\n%02i  %i %13.2f", nomina.mes, nomina.ano, nomina.sueldo_mensual);
            suma_nomias+=nomina.sueldo_mensual;
        }
    }
    printf("\n%18s%.2f\n", "Total $", suma_nomias);

    printf("\nMonto a la fecha: %.2f", proyecto.monto_inicial-suma_nomias);
    stop();

    return;
}

void imprimir_nomina_reporte(FILE *archivo_empleados, Nomina* nomina, FILE *salida)
{
    Empleado empleado;
    rewind(archivo_empleados);
    while(fread(&empleado, sizeof(Empleado), 1, archivo_empleados)) {
        if( strcmp(nomina->curp_empleado, empleado.curp)==0) {
            fprintf(salida, "%20u | %20s | %20s | %20.2f | %20u | %20.2f ",
            empleado.num_empleado, empleado.nombre, get_string_perfil(empleado.perfil),
            empleado.tarifa_mensual, nomina->horas_trabajadas, nomina->sueldo_mensual);
            if(nomina->horas_trabajadas < HORAS_MENSUALES_MINIMAS)
                fprintf(salida, " **");
            fprintf(salida, "\n");
        }
    }
}


void imprimir_lista_nominas(AppData* appdata, Proyecto* proyecto, Fecha* fecha_nomina, FILE* salida){
    Nomina nomina;
    fprintf(salida, "Proyecto: %s\t%s\n", proyecto->clave_proyecto, proyecto->nombre);
    fprintf(salida, "Nomina de: %s\t%d\n", get_string_mes(fecha_nomina->mes), fecha_nomina->ano);
    fprintf(salida, "\n%20s | %20s | %20s | %20s | %20s | %20s\n",
        "No. Empleado", "Nombre", "Perfil", "Tarifa", "Horas", "Sueldo");
    rewind(appdata->archivo_nominas);
    while(fread(&nomina, sizeof(Nomina), 1, appdata->archivo_nominas)){
        if(strcmp(nomina.clave_proyecto, proyecto->clave_proyecto)==0
            && nomina.mes==fecha_nomina->mes && nomina.ano==fecha_nomina->ano
        ) imprimir_nomina_reporte(appdata->archivo_empleados, &nomina, salida);
    }
    fprintf(salida, "\t\tTotal de nominas: %.2f\n\n", proyecto->total_nomina);
    fprintf(salida, "** No cumplio con el minimo de horas\n");
}

void reporte_nominas(AppData* appdata)
{
    Proyecto proyecto;
    bool existe_proyecto=false;
    char clave_proyecto[L_CLAVE+1];

    clear();
    while(true) {
        printf("Introduzca la clave del proyecto\n");
        leer_string(clave_proyecto, L_CLAVE);
        rewind(appdata->archivo_proyectos);
        while(fread(&proyecto, sizeof(Proyecto), 1, appdata->archivo_proyectos)) {
            if(strcmp(proyecto.clave_proyecto, clave_proyecto)==0
            && proyecto.estatus==ACTIVO) {existe_proyecto=true; break;}
        }
        if(existe_proyecto) break;
        printf("No se ha encontrado ningun proyecto con la clave especificada\n");
    }

    Fecha fecha_nomina = {.dia=0};
    printf("Introduzca el mes de la nomina\n");
    fecha_nomina.mes = validar_entero(1,12);
    printf("Introduzca el ano de la nomina\n");
    fecha_nomina.ano = validar_entero(1900, 3000);

    Nomina nomina;
    bool existe_nomina_de_fecha=false;
    rewind(appdata->archivo_nominas);
    while(fread(&nomina, sizeof(Nomina), 1, appdata->archivo_nominas)) {
        if( strcmp(nomina.clave_proyecto, proyecto.clave_proyecto)==0
            && nomina.mes==fecha_nomina.mes
            && nomina.ano==fecha_nomina.ano) {
            existe_nomina_de_fecha=true;
            break;
        }
    }
    if(!existe_nomina_de_fecha) {
        printf("No se encontro nunguna nomina de la fecha especificada\n");
        stop();
        return;
    }

    imprimir_lista_nominas(appdata, &proyecto, &fecha_nomina, stdout);
    printf("Desea guardar este reporte de nomina en un archivo? 1-Si | 2-No\n");
    int opc=validar_entero(1,2);
    if(opc==1) {
        char nombre_reporte[L_CLAVE+12];
        sprintf(nombre_reporte, "%s_%d_%d.txt", proyecto.clave_proyecto,
            fecha_nomina.ano, fecha_nomina.mes);
        FILE* archivo_reporte=fopen(nombre_reporte, "w");
        if(archivo_reporte==NULL) {
            printf("Hubo un error al crear el archivo para el reporte\n");
            stop();
            return;
        }
        imprimir_lista_nominas(appdata, &proyecto, &fecha_nomina, archivo_reporte);
        fclose(archivo_reporte);
        printf("Reporte guardado exitosamente como '%s'\n", nombre_reporte);
        stop();
    }
    return;
}


void print_proyecto_debug(void* ptr)
{
    Proyecto* proyecto = (Proyecto*)ptr;
    printf("Clave: %s\n", proyecto->clave_proyecto);
    printf("Nombre: %s\n", proyecto->nombre);
    printf("Monto inincial: %.2f\n", proyecto->monto_inicial);
    printf("Total de Nomina: %.2f\n", proyecto->total_nomina);
    printf("Fecha inicio: %d-%d-%d\n", proyecto->inicio.dia,
        proyecto->inicio.mes, proyecto->inicio.ano);
    printf("Fecha termino: %d-%d-%d\n", proyecto->termino.dia,
        proyecto->termino.mes, proyecto->termino.ano);
    printf("Estatus: %s\n",  get_string_estatus(proyecto->estatus));
    putchar('\n');
    return;
}


void print_empleado_debug(void* ptr)
{
    Empleado* empleado = (Empleado*)ptr;
    printf("Clave Proyecto: %s\n", empleado->clave_proyecto);
    printf("Nombre: %s\n", empleado->nombre);
    printf("CURP: %s\n", empleado->curp);
    printf("Correo: %s\n", empleado->correo);
    printf("Telefono: %s\n", empleado->telefono);
    printf("No. Empleado: %u\n", empleado->num_empleado);
    printf("Tarifa Mensual: %.2f\n", empleado->tarifa_mensual);
    printf("Fecha nacimiento: %d-%d-%d\n", empleado->nacimiento.dia,
        empleado->nacimiento.mes, empleado->nacimiento.ano);
    printf("Perfil: %s\n", get_string_perfil(empleado->estatus));
    printf("***Direccion***\n");
    printf("No. de Casa: %u\n", empleado->direccion.numero);
    printf("Calle: %s\n", empleado->direccion.calle);
    printf("Colonia: %s\n", empleado->direccion.colonia);
    printf("Municipio: %s\n", empleado->direccion.municipio);
    printf("Codigo Postal: %u\n", empleado->direccion.cp);
    printf("Estatus: %s\n", get_string_estatus(empleado->estatus));
    putchar('\n');
    return;
}


void print_nomina_debug(void* ptr)
{
    Nomina* nomina = (Nomina*)ptr;
    printf("Clave Proyecto: %s\n", nomina->clave_proyecto);
    printf("Curp Empleado: %s\n", nomina->curp_empleado);
    printf("Mes: %d\n", nomina->mes);
    printf("Ano: %d\n", nomina->ano);
    printf("Horas Trabajadas: %d\n", nomina->horas_trabajadas);
    printf("Sueldo Mensual: %.2f\n", nomina->sueldo_mensual);
    printf("Estatus: %s\n", get_string_estatus(nomina->estatus));
    putchar('\n');
    return;
}


void print_pos_debug(void* ptr) {
    long* pos = (long*)ptr;
    printf("Pos: %ld\n", *pos);
    return;
}

void print_datos_debug(void* ptr)
{
    DatosNomina* datos = (DatosNomina*)ptr;
    printf("-----------------------\n");
    print_empleado_debug(&datos->empleado);
    print_nomina_debug(&datos->nomina);
    print_pos_debug(&datos->pos);
    printf("-----------------------\n\n");
    return;
}


void print_all_debug(size_t tamano_elemento, FILE* archivo, void(*imprimir)(void*))
{
    rewind(archivo);
    byte buffer[tamano_elemento];
    while(fread(buffer, tamano_elemento, 1, archivo))
        imprimir(buffer);
    stop();
    return;
}


void print_arreglo_debug(Array arreglo, const char* nombre_arreglo, void (*print_elemento)(void*))
{
    printf("Inicio arreglo %s | longitud: %zu\n\n",
        nombre_arreglo, array_get_longitud(arreglo));
    array_print(arreglo, print_elemento);
    printf("Fin arreglo %s\n", nombre_arreglo);
    printf("\n");
    return;
}