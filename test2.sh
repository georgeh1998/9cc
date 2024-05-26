#!/bin/bash

./georgecc "$1" > tmp.s
cc -static -o tmp tmp.s c_func.c
./tmp
echo $?