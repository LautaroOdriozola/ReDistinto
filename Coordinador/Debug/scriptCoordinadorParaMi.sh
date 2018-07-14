#!/bin/sh
echo "Compilando Coordinador"
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/Escritorio/tp-2018-1c-EnMiCompuCompilaba/socket/Debug/
make clean
make
