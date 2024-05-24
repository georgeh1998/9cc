#!/bin/bash

./9cc "$1" > tmp.s
cc -static -o tmp tmp.s c_func.c
./tmp
echo $?