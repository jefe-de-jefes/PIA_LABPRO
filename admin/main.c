#include <stdio.h>
#include "admin.h"
#include "nomina.h"

static const char *texto_menu_admin =
    "Subsistema de Administracion de Usuarios:\n"
    "1-Listar Usuarios\n"
    "2-Desbloquear Usuario\n"
    "3-Anadir Usuario\n"
    "4-Eliminar Usuario\n"
    "5-Salir\n";

int main(void)
{
    FILE* archivo=abrir_archivo_binario(ARCHIVO_USUARIOS);
    while(1) {
        int opc=menu(texto_menu_admin, 1, 5);
        switch(opc) {
        case 1:
            listar_usuarios(archivo);
            break;
        case 2:
            desbloquear_usuario(archivo);
            break;
        case 3:
            anadir_usuario(archivo);
            break;
        case 4:
            eliminar_usuario(archivo);
            break;
        case 5:
            clear();
            fclose(archivo);
            return 0;
        }
    }
}