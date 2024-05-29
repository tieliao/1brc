
PROG=./summarize
DATA_FILE=./1brc/measurements.txt

all: ${PROG}
	hyperfine -i '${PROG} ${DATA_FILE} $(nproc)'

${PROG}: 1brc.c
	gcc -O3 -Wall -o ${PROG} 1brc.c -lpthread

check:
	${PROG} ${DATA_FILE} $(nproc) > out.txt
	diff -q out.txt result.txt

profiling:
	gcc -pg -O -Wall -o ${PROG} 1brc.c -lpthread
	${PROG} ${DATA_FILE} $(nproc) > /dev/null
	gprof ${PROG} gmon.out > profiling.txt

clean:
	rm -f ${PROG} out.txt
	rm -f gmon.out profiling.txt
