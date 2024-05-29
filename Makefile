
PROG=./summarize
DATA_FILE=./1brc/measurements.txt

all: ${PROG}
	hyperfine -i '${PROG} ${DATA_FILE} $(nproc)'

${PROG}: 1brc.c
	gcc -O3 -Wall -o ${PROG} 1brc.c -lpthread
