
PROG=./summarize
DATA_FILE=./1brc/measurements.txt
NPROC=$(shell nproc)

all: ${PROG}
	hyperfine -i '${PROG} ${DATA_FILE} ${NPROC}'

${PROG}: 1brc.c
	gcc -O3 -DNDEBUG -Wall -o ${PROG} 1brc.c -lpthread

check: ${PROG}
	${PROG} ${DATA_FILE} ${NPROC} > out.txt
	diff -q out.txt 1brc/output.txt

debug:
	gcc -g -O -DDEBUG -Wall -o ${PROG} 1brc.c -lpthread
	${PROG} ${DATA_FILE} ${NPROC}

profiling:
	gcc -pg -O -DNDEBUG -DINLINE= -Wall -o ${PROG} 1brc.c -lpthread
	${PROG} ${DATA_FILE} 1 > /dev/null
	gprof ${PROG} gmon.out > profiling.txt

clean:
	rm -f ${PROG} out.txt
	rm -f gmon.out profiling.txt
