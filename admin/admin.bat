set include_dir=.\nomina\include
set source=.\admin\main.c .\admin\admin.c .\nomina\source\misc.c
%1 -o admin -iquote %include_dir% %source%