#!/bin/bash

include_dir="./nomina/include"
source="./nomina/source/array.c ./nomina/source/auxiliares.c ./nomina/source/main.c ./nomina/source/nomina.c ./nomina/source/misc.c"
$1 -o ./nomina_app -iquote $include_dir $source