@SETLOCAL
@echo off
set script_admin=.\admin\admin.bat
set script_nomina=.\nomina\nomina.bat

if "%1" == "todos" (
    call %script_admin% %2
    %script_nomina% %2
)
if "%1" == "admin" (
    %script_admin% %2
)
if "%1" == "nomina" (
     %script_nomina% %2
)

echo "Introduzca el programa a compilar como argumento"
echo "'nomina', 'admin' o 'todos'"
echo "seguido de el compilador a usar:"
echo "ej: './compilar.sh todos gcc', './compilar.sh nomina clang'"
ENDLOCAL