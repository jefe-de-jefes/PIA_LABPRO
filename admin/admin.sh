#!/bin/bash

include_dir="./nomina/include"
source="./admin/main.c ./admin/admin.c ./nomina/source/misc.c"
$1 -o ./admin_app -iquote $include_dir $source