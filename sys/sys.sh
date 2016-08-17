#!/bin/bash
#TODO: Write a proper system test framework
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$(pwd)/../lib
time mpirun -np 4 systest
