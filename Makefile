
PROG=./summarize
DATA_FILE=./1brc/measurements.txt

all: ${PROG}
	hyperfine -i '${PROG} ${DATA_FILE} $(nproc)'

${PROG}: 1brc.c
	gcc -O3 -DNDEBUG -Wall -o ${PROG} 1brc.c -lpthread

check: ${PROG}
	${PROG} ${DATA_FILE} $(nproc) > out.txt
	diff -q out.txt 1brc/output.txt

debug:
	gcc -g -O -DDEBUG -Wall -o ${PROG} 1brc.c -lpthread
	${PROG} ${DATA_FILE} $(nproc)

profiling:
	gcc -pg -O -DNDEBUG -DINLINE= -Wall -o ${PROG} 1brc.c -lpthread
	${PROG} ${DATA_FILE} 1 > /dev/null
	gprof ${PROG} gmon.out > profiling.txt

clean:
	rm -f ${PROG} out.txt
	rm -f gmon.out profiling.txt
