#!/bin/bash

script_admin="./admin/admin.sh"
script_nomina="./nomina/nomina.sh"

case "$1" in
    "todos")
        $script_admin $2
        $script_nomina $2
        ;;
    "admin")
        $script_admin $2
        ;;
    "nomina")
        $script_nomina $2
        ;;
    *)
        echo "Introduzca el programa a compilar como argumento"
        echo "'nomina', 'admin' o 'todos'"
        echo "seguido de el compilador a usar:"
        echo "ej: './compilar.sh todos gcc', './compilar.sh nomina clang'"
esac

