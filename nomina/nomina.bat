set include_dir=.\nomina\include
set source=.\nomina\source\array.c .\nomina\source\auxiliares.c .\nomina\source\main.c .\nomina\source\nomina.c .\nomina\source\misc.c
%1 -o nomina -iquote %include_dir% %source%