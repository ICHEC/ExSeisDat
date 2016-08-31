#TODO: Make into makefile
mpicxx -std=c++14 -L ../lib -lpiol -I ../include assess.cc -o assess
mpicxx -std=c++14 -L ../lib -lpiol -I ../include filemake.cc -o filemake
mpicxx -std=c++14 -L ../lib -lpiol -I ../include example1.cc -o example1
mpicc -std=c99 -Wall -Wextra -pedantic -g  -align -xHost -O3 -fopenmp  -I../api/ -L ../lib -lpiol -lcpiol example1.c  -o example1c
mpicc -std=c99 -Wall -Wextra -pedantic -g  -align -xHost -O3 -fopenmp  -I../api/ -L ../lib -lpiol -lcpiol example2.c  -o example2c
