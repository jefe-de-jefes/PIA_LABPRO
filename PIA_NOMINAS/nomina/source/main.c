#include "nomina.h"
#include "auxiliares.h"
#include "misc.h"

static const char *texto_menu_principal =
    "1- Proyecto\n"
    "2- Empleados\n"
    "3- Nomina\n"
    "4- Reportes\n"
    "5- Salir\n";

int main(void)
{
    AppData appdata;
    login(&appdata);
    cargar_datos(&appdata);
    while(1) {
        int opc=menu(texto_menu_principal, 1, 6);
        switch(opc) {
        case 1:
            submenu_proyectos(&appdata);
            break;
        case 2:
            submenu_empleados(&appdata);
            break;
        case 3:
            submenu_nomina(&appdata);
            break;
        case 4:
            submenu_reportes(&appdata);
            break;
        case 5:
            clear();
            liberar_memoria(&appdata);
            exit(0);
            break;
        case 6:
            submenu_debug(&appdata);
            break;
        }
    }
}