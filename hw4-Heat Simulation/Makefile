CC=gcc
MPICC=mpicc
CFLAG=-O2 -lX11 -lrt -lm

all: seq mpi pthread omp

seq: hw4-seq.c display.h models.h const.h
	${CC} hw4-seq.c -o seq ${CFLAG}

pthread: hw4-pth.c display.h models.h const.h
	${CC} hw4-pth.c -o pthread ${CFLAG} -lpthread

mpi: hw4-mpi.c display.h models.h const.h
	${MPICC} hw4-mpi.c -o mpi ${CFLAG} 

omp: hw4-omp.c display.h models.h const.h
	${CC} hw4-omp.c -o omp ${CFLAG} -fopenmp

clean:
	rm -f *~
	rm -f seq
	rm -f pthread
	rm -f mpi
	rm -f omp