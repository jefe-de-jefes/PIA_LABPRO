#include <stdio.h>
#include <string.h>
#include "nomina.h"
#include "array.h"
#include "auxiliares.h"
#include "misc.h"

static const char* strings_status[3]= {"Inactivo", "Activo", "Basura"};

/*Seccion Login*/
int comparar_nombre_usuarios(void* ptr1, void* ptr2)
{
    Usuario *usuario1=(Usuario*)ptr1, *usuario2=(Usuario*)ptr2;
    if(
        strcmp(usuario1->nombre, usuario2->nombre)==0
        && usuario1->estatus==usuario2->estatus
    ) return true;
    return false;
}

/*Seccion Proyecto*/
int existe_clave_proyecto(char clave[L_CLAVE+1], FILE* archivo){
    /*Esta funcion valida que la clave de los proyectos no se repita
    devuelve 1 si hay una repetida o 0 si no hay ninguna*/
    Proyecto proyecto;
	rewind(archivo);
    while(fread(&proyecto, sizeof(proyecto), 1, archivo)){
        if((strcmp(proyecto.clave_proyecto, clave)==0) && proyecto.estatus==ACTIVO){
            return 1;//Si son iguales y esta activo devuelve verdadero
        }
    }
    return 0;//Si no hay ninguno igual devuelve false
}

int registrar_proyecto(FILE* archivo_proyectos){
    //esta funcion registra los proyectos, retorna 1 si escribio correctamente el proyecto o 0 en caso contrario
    Proyecto proy;
    int clave_tomada=0;
    clear();
    printf("\n**REGISTRO DE PROYECTO**\n");

    fseek(archivo_proyectos, 0, SEEK_END);//colocamos en la ultima posicion para escribir

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
    printf("\nIntroduzca el año de de inicio del proyecto (aaaa): ");
    proy.inicio.ano=validar_entero(MIN_A, MAX_A);
    printf("\nIntroduzca el mes de de inicio del proyecto(mm): ");
    proy.inicio.mes=validar_entero(MIN_M, MAX_M);
    printf("\nIntroduzca el dia de de inicio del proyecto (dd): ");
    proy.inicio.dia=validar_entero(1, 31);

    printf("\n***FECHA TERMINO***\n");
    printf("\nIntroduzca el año de de termino del proyecto (aaaa): ");
    proy.termino.ano=validar_entero(0, 9999);
    printf("\nIntroduzca el mes de de termino del proyecto (mm): ");
    proy.termino.mes=validar_entero(1, 12);
    printf("\nIntroduzca el dia de de termino del proyecto (dd): ");
    proy.termino.dia=validar_entero(1, 31);
    printf("\nIntroduzca el status [1-Activo | 0-Inactivo]: ");
    proy.estatus=(Estatus)validar_entero(0, 1);

    //retorna 1 si se escribio correctamente
    return fwrite(&proy, sizeof(Proyecto), 1, archivo_proyectos);
}

void imprimir_proyecto(Proyecto* proy){
    int i;

    for(i=0;i<129;i++)printf("*");
    printf("\n%20s | %10s | %10s | %20s | %20s | %20s | %10s*",
        "Clave Proyecto", "Nombre", "Monto", "Total nomina", "Fecha inicio", "Fecha termino", "Estatus");
    printf("\n%20s | %10s | %10.3f | %20.2f | %12i/%02i/%04i | %12i/%02i/%04i | %10s*\n",
    proy->clave_proyecto, proy->nombre, proy->monto_inicial, proy->total_nomina,
    proy->inicio.dia, proy->inicio.mes, proy->inicio.ano, proy->termino.dia,
    proy->termino.mes, proy->termino.ano, strings_status[proy->estatus]);
    for(i=0;i<129;i++)printf("*");
    printf("\n");
}

void imprimir_empleado(Empleado *emp){
    printf("\n%20s | %20s | %20s | %20s | %20s | %20s | %10s | %10s | %10s",
           "Clave Proyecto", "Numero de empleado", "Nombre", "CURP",
           "Fecha de nacimiento", "Correo", "Telefono", "Perfil", "Tarifa Mensual");
    printf("\n%20s | %20d | %20s | %20s | %02d/%02d/%04d \t\t | %20s | %10s | %10u | %10.2f\n",
           emp->clave_proyecto, emp->num_empleado, emp->nombre, emp->curp,
           emp->nacimiento.dia, emp->nacimiento.mes, emp->nacimiento.ano,
           emp->correo, emp->telefono, emp->perfil, emp->tarifa_mensual);
    printf("\n%20s | %20s | %20s | %20s | %20s | %20s",
           "Calle", "Colonia", "Numero", "Codigo Postal", "Municipio", "Estatus");
    printf("\n%20s | %20s | %20u | %20s | %20d | %20d\n",
           emp->direccion.calle, emp->direccion.colonia, emp->direccion.numero,
           emp->direccion.municipio, emp->direccion.cp, emp->estatus);
}

void baja_empleados_proyecto(char clave[L_CLAVE+1], FILE* archivo_empleados){
    /*funcion que utilizamos en baja de proyectos en la que cambiamos
    el estatus cada  empleado del proyecto a inactivo*/
    Empleado empleado;
    rewind(archivo_empleados);
    while (fread(&empleado, sizeof(Empleado), 1, archivo_empleados))
    {
        //verifica que sean las mismas claves y
        //que no este ya dado de baja el empleado
        if(strcmp(clave, empleado.clave_proyecto)==0 && empleado.estatus==ACTIVO){
            empleado.estatus=INACTIVO;
            fseek(archivo_empleados, -sizeof(Empleado), SEEK_CUR);
            fwrite(&empleado, sizeof(Empleado), 1, archivo_empleados);
            printf("\n**Empleado #%i dado de baja**", empleado.num_empleado);
        }
    }
    return;
}

void baja_proyecto(FILE* archivo_proyectos, FILE* archivo_empleados){
    /*da de baja a el proyecto y a los empleados relacionados a este*/
    Proyecto proy;
    char nom[L_NOMBRE+1];

    printf("\nIntroduzca el nombre del proyecto\n");
    leer_string(nom, L_NOMBRE);
    printf("\nDando de baja el proyecto...\n");

    rewind(archivo_proyectos);
    while (fread(&proy, sizeof(Proyecto), 1, archivo_proyectos)){
        if (strcmp(nom, proy.nombre)==0 && proy.estatus==ACTIVO)//buscando nombres iguales y que esten activos
        {
                fseek(archivo_proyectos, -sizeof(Proyecto), SEEK_CUR);//un proyecto atras para actualizar
                puts("\n**Proyecto encontrado**\n");
                imprimir_proyecto(&proy);
                proy.estatus=INACTIVO;//cambio a inactivo

                if(fwrite(&proy, sizeof(Proyecto), 1, archivo_proyectos)==1){//verificacion de errores
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


/*Seccion Empleados*/
void validar_clave_proyecto(AppData* appdata, Empleado* reg_empleado){
    Proyecto proy;
    while(true){
        // Ingresamos la clave
        printf("Introduzca La clave del proyecto\n");
        leer_string(reg_empleado->clave_proyecto, L_CLAVE);
        // Regresamos al inicio del archivo
        rewind(appdata->archivo_proyectos);
        // Validamos que la clave del proyecto realmente exista
        while (fread(&proy, sizeof(Proyecto), 1, appdata->archivo_proyectos)){
            if( (strcmp(proy.clave_proyecto, reg_empleado->clave_proyecto) == 0)
                &&proy.estatus==ACTIVO
            ) return;
        }
        printf("La clave del proyecto no esta registrada\n");
    }
}

void validar_curp_empleado(AppData* appdata, Empleado* empleado){
    bool curp_repetido = false;
    Empleado buffer_lectura;

    while(true){
        printf("Introduzca la curp del empleado\n");
        leer_string(empleado->curp, L_CURP);
        // Regresamos el archivo
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

void registro_empleado(AppData* appdata){
    Empleado reg_empleado;
    // Ciclo para validar si quiere registrar otro empelado o desea regresar al submenu
    while(1){
        clear();
        printf("\n**REGISTRO DE EMPLEADO**\n");
        validar_clave_proyecto(appdata, &reg_empleado);
        // Numero de empleado -> Entero, numero entero consecutivo
        //Ciclo para validar que el entero sea positivo
        while(1){
            printf("Introduzca el numero del empleado\n");
            scanf("%d", &reg_empleado.num_empleado);
            if(reg_empleado.num_empleado < 0){
                printf("Introduzca un numero mayor a 0\n");
            }else break;
        }

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

        // Ciclo para validar que no sean numeros menores a 0 y que sean 10 digitos
        while(1){
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

        printf(texto_seleccion_perfil);
        reg_empleado.perfil = validar_entero(1,5);

        // Tarifa Mensual -> Flotante
        // Ciclo para validar la tarifa mensual < 0
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
        // Numero
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

        //Escribir empleado
        fseek(appdata->archivo_empleados, 0, SEEK_END);
        fwrite(&reg_empleado, sizeof(Empleado), 1, appdata->archivo_empleados);

       //Registrar otro empleado o salir al menu principal
        static const char* texto_seguir_con_registo=
        "Que opcion desea?: \n"
        "1.- Agregar otro empleado\n"
        "2.- Salir al menu principal\n";

        printf(texto_seguir_con_registo);
        int opc=validar_entero(1,2);
        if(opc==2) break;
    }
}

void baja_empleado(AppData * appdata){
    Empleado emp;
    char nombre[L_NOMBRE+1];
    int opc, empleado_encontrado;
    // Ciclo para validar un error;
    while(1){
        printf("\n**BAJA DE EMPLEADO**\n");
        printf("Introduzca el nombre del empleado que desea dar de baja\n\n");
        leer_string(nombre, L_NOMBRE);
        rewind(appdata->archivo_empleados);
        while(fread(&emp, sizeof(Empleado), 1, appdata->archivo_empleados)){
            if(strcmp(nombre, emp.nombre) == 0){
                imprimir_empleado(&emp);
                printf("\nDesea eliminar este empleado?\n1.- Si\n2.- No\n");
                opc = validar_entero(1,2);
                printf("\n");
                if(opc == 1){
                    emp.estatus = INACTIVO;
                    fseek(appdata->archivo_empleados, -sizeof(Empleado), SEEK_CUR);
                    fwrite(&emp, sizeof(Empleado), 1, appdata->archivo_empleados);
                }
                empleado_encontrado = true;
            }
        }
        if(!empleado_encontrado) printf("\nEl empleado no se encontro\n");
        printf("Desea dar de baja a otro empleado?\n1.- Si\n2.- Regresar al menu empleados\n");
        opc = validar_entero(1, 2);
        if(opc==2) break;
    }
}

/*Seccion Nominas*/
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
    if(
        strcmp(proyecto1->clave_proyecto, proyecto2->clave_proyecto)==0
        && proyecto1->estatus==proyecto2->estatus
    ) return true;
    return false;
}

int comparar_empleado_en_proyecto(void* ptr1, void* ptr2)
{
    Proyecto* proyecto = (Proyecto*)ptr1;
    Empleado* empleado = (Empleado*)ptr2;
    if(
        strcmp(proyecto->clave_proyecto, empleado->clave_proyecto)==0
        && proyecto->estatus==empleado->estatus
    ) return true;
    return false;
}

int comparar_nomina_en_proyecto(void* ptr1, void* ptr2)
{
    Proyecto* proyecto = (Proyecto*)ptr1;
    Nomina* nomina = (Nomina*)ptr2;
    if(
        strcmp(proyecto->clave_proyecto, nomina->clave_proyecto)==0
        && proyecto->estatus==nomina->estatus
    ) return true;
    return false;
}

int comparar_curp_empleado(void* ptr1, void* ptr2)
{
    Empleado *empleado1=(Empleado*)ptr1, *empleado2=(Empleado*)ptr2;
    if(
        strcmp(empleado1->curp, empleado2->curp)==0
        && empleado1->estatus==empleado2->estatus
    )  return true;
    return false;
}

Array emparejar_empleados(Array nominas, Array empleados)
{
    Empleado buffer_empleado={.estatus=ACTIVO};
    Nomina buffer_nomina;
    long indice;

    bool posiciones_copiadas[array_get_longitud(empleados)];
    for(int i=0; i<array_get_longitud(empleados); i++) posiciones_copiadas[i]=false;

    Array nuevo_array_emp=array_init(sizeof(Empleado), array_get_longitud(empleados), NULL, 0);
    for(int i=0; i<array_get_longitud(nominas); i++) {
        array_get(nominas, i, &buffer_nomina);
        strcpy(buffer_empleado.curp, buffer_nomina.curp_empleado);
        indice=array_index(empleados, &buffer_empleado, comparar_curp_empleado);
        array_get(empleados, indice, &buffer_empleado);
        array_push(nuevo_array_emp, &buffer_empleado);
        posiciones_copiadas[indice]=true;
    }
    for(int i=0; i<array_get_longitud(empleados); i++) {
        if(!posiciones_copiadas[i]) {
            array_get(empleados, i, &buffer_empleado);
            array_push(nuevo_array_emp, &buffer_empleado);
        }
    }
    array_destroy(empleados);
    return nuevo_array_emp;
}

void actualizar_nominas(FILE* archivo, Array nominas, Array posiciones)
{
    Nomina buffer_nomina;
    long buffer_pos;
    int i;
    for(i=0; i<array_get_longitud(posiciones); i++) {
        array_get(nominas, i, &buffer_nomina);
        array_get(posiciones, i, &buffer_pos);
        fseek(archivo, buffer_pos, SEEK_SET);
        fwrite(&buffer_nomina, sizeof(Nomina), 1, archivo);
    }
    fseek(archivo, 0, SEEK_END);
    for(; i<array_get_longitud(nominas); i++) {
        array_get(nominas, i, &buffer_nomina);
        fwrite(&buffer_nomina, sizeof(Nomina), 1, archivo);
    }
    return;
}

/*Seccion Reportes*/

void mostrar_empleado_reporte(Empleado emp){
    int i;
    char array[5][15]={"LIDER_PROYECTO", "ADMIN_BD", "ANALISTA", "PROGRAMDOR", "TESTER"};
    printf("\n%15i | %10s | %20s |  %12i/%02i/%04i | %15s | $%7.2f*\n", emp.num_empleado, emp.nombre, emp.curp, emp.nacimiento.dia, 
    emp.nacimiento.mes, emp.nacimiento.ano, array[emp.perfil-1], emp.tarifa_mensual);
}

void reporte_proyectos(AppData* appdata){
    Nomina nomina;
    Empleado empleado;
    Proyecto proyecto;
    char clave_consultar[L_CLAVE+1];
    int suma=0;

    clear();
    puts("\n**REPORTE DE PROYECTOS ACTIVOS**\n");
    int existencia=1;
    do
    {
        if (!existencia) printf("\n**El proyecto no existe.**\n");

        printf("\nIntroduzca la clave del proyecto a consultar: ");
        leer_string(clave_consultar, L_CLAVE);
        existencia=existe_clave_proyecto(clave_consultar, appdata->archivo_proyectos);
    }while(!existencia);
    


    rewind(appdata->archivo_proyectos);
    while (fread(&proyecto, sizeof(Proyecto), 1, appdata->archivo_proyectos))
    {
        if ((strcmp(proyecto.clave_proyecto, clave_consultar)==0) && proyecto.estatus)
        {
            break;
        }
        
    }
    
    printf("\nProyecto: %s %s", clave_consultar, proyecto.nombre);
    printf("\n%15i | %10s | %20s | %20s | %15s | %10s\n", "No empleado", "Nombre", "CURP", "Fecha Nac", "Perfil", "Tarifa");

    while (fread(&empleado, sizeof(Empleado), 1, appdata->archivo_empleados))
    {
        if ((strcmp(empleado.clave_proyecto, clave_consultar)==0) && empleado.estatus)
        {
            mostrar_empleado_reporte(empleado);         
        }
    }

    printf("\nMonto inicial del proyecto: $%.2f\n", proyecto.monto_inicial);
    
    rewind(appdata->archivo_nominas);
    printf("\n%s %s  %s", "Mes", "Año", "Total Nomina");
    while (fread(&nomina, sizeof(Nomina), 1, appdata->archivo_nominas))
    {
        if (strcmp(nomina.clave_proyecto, clave_consultar)==0)
        {
            printf("\n%02i %i %13.2f", nomina.mes, nomina.ano, nomina.sueldo_mensual);
            suma+=nomina.sueldo_mensual;
        }
    }
    printf("%12s%.2f", "Total $", suma);

    printf("\nMonto a la fecha: %.2f", proyecto.monto_inicial-suma);
    stop();

    return;
}

/*Seccion Debug*/

void print_proyecto_debug(void* ptr)
{
    Proyecto* proyecto = (Proyecto*)ptr;
    printf("Clave: %s\n", proyecto->clave_proyecto);
    printf("Nombre: %s\n", proyecto->nombre);
    printf("Monto inincial: %f\n", proyecto->monto_inicial);
    printf("Total de Nomina: %f\n", proyecto->total_nomina);
    printf("Fecha inicio: %d-%d-%d\n", proyecto->inicio.dia,
        proyecto->inicio.mes, proyecto->inicio.ano);
    printf("Fecha termino: %d-%d-%d\n", proyecto->termino.dia,
        proyecto->termino.mes, proyecto->termino.ano);
    printf("Estatus: %s\n",  strings_status[proyecto->estatus]);
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
    printf("Tarifa Mensual: %f\n", empleado->tarifa_mensual);
    printf("Fecha nacimiento: %d-%d-%d\n", empleado->nacimiento.dia,
        empleado->nacimiento.mes, empleado->nacimiento.ano);
    printf("Perfil: %d\n", empleado->perfil);
    printf("***Direccion***\n");
    printf("No. de Casa: %u\n", empleado->direccion.numero);
    printf("Calle: %s\n", empleado->direccion.calle);
    printf("Colonia: %s\n", empleado->direccion.colonia);
    printf("Municipio: %s\n", empleado->direccion.municipio);
    printf("Codigo Postal: %u\n", empleado->direccion.cp);
    printf("Estatus: %s\n", strings_status[empleado->estatus]);
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
    printf("Sueldo Mensual: %f\n", nomina->sueldo_mensual);
    printf("Estatus: %s\n", strings_status[nomina->estatus]);
    putchar('\n');
    return;
}

void print_pos_debug(void* ptr) {
    long* pos = (long*)ptr;
    printf("Pos: %d\n", *pos);
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
    printf("Inicio arreglo %s | longitud: %d\n\n",
        nombre_arreglo, array_get_longitud(arreglo));
    array_print(arreglo, print_elemento);
    printf("Fin arreglo %s:\n", nombre_arreglo);
    printf("\n");
    return;
}