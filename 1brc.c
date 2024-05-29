/*
 * 1 Billion Row Challenge, just for fun :-)
 * $ gcc -O3 -o summarize 1brc.c -lpthread
 * $ ./summarize measurements.txt $(nproc)
 * Written by Tie Liao, May 28, 2024
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>


/*
 * optimizations:
 *   - mmap() file once
 *   - using hashig table to save records
 *   - hashig code computed when reading buffer
 *   - using minimal malloc()
 *   - inline critical function
 *   - using integer operations instead of floating operations
 *   - using local arrays to decode temperature
 *   - using linear code as much as possbile
 *   - parallel processing using multi-threads
 *   - comparing hashing code instead of name string
 * minimal error processing is done here
 */

#define MAX_NAME_LEN          48
#define MAX_THREADS           32

#define HASH_BITS             14
#define HASH_SIZE             (1 << HASH_BITS)
#define HASH_MASK             (HASH_SIZE - 1)

struct city_s {
    struct city_s *next;
    char name[MAX_NAME_LEN];
    int min, max, sum;                  /* scaled by 10 */
    int samples;
    int hash;
};

struct proc_context_s {
    pthread_t thread_id;
    char *buff_start;
    char *buff_end;
    struct city_s *hash_table[HASH_SIZE];
    int entries;

    struct city_s **sorted;
} ctx[MAX_THREADS];

int input_fd;
int input_file_sz;
char *input_buff;
int nproc = 4;


void err_exit(int err, char *msg) {
    printf("ERROR: %s\n", msg);
    exit(err);
}

void open_file(const char *filename) {
    struct stat st;
    int i, size, len;
    struct proc_context_s *pctx;

    input_fd = open(filename, O_RDONLY);
    if (input_fd < 0)
        err_exit(1, "failed to open file");

    fstat(input_fd, &st);
    input_file_sz = st.st_size;

    input_buff = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, input_fd, 0);
    if (input_buff == MAP_FAILED)
        err_exit(2, "failed to mmap");

    size = input_file_sz/nproc;
    len = 0;
    for (i = 0; i < nproc; i ++) {
        pctx = &ctx[i];
        pctx->buff_start = input_buff + len;
        if (i < (nproc - 1)) {
            pctx->buff_end = memchr(pctx->buff_start + size, '\n',
                                    input_file_sz - len);
            if (!pctx->buff_end)
                err_exit(10, "input file error");
        } else
            pctx->buff_end = input_buff + input_file_sz;
        len += pctx->buff_end - pctx->buff_start + 1;
    }
}

void close_file() {
    munmap(input_buff, input_file_sz);
    close(input_fd);
}

inline void record_city(struct proc_context_s *pctx, int hash,
                        char *name, int name_len, int temp) {
    struct city_s *city;
    int index;

    index = hash + (hash >> HASH_BITS);
    index &= HASH_MASK;
    city = pctx->hash_table[index];
    for (; city; city = city->next) {
        if (hash == city->hash) {
            /* uniquely identify an entry */
            city->samples ++;
            city->sum += temp;
            if (temp < city->min)
                city->min = temp;
            else if (temp > city->max)
                city->max = temp;
            return;
        }
    }

    city = (struct city_s *)malloc(sizeof(*city));
    memcpy(city->name, name, name_len);
    city->name[name_len] = '\0';
    city->min = temp;
    city->max = temp;
    city->sum = temp;
    city->samples = 1;
    city->hash = hash;
    city->next = pctx->hash_table[index];
    pctx->hash_table[index] = city;
    pctx->entries ++;
}

void *process_data(void *data) {
    char *in, *name, *end;
    int hash, name_len, temp;
    struct proc_context_s *pctx;
    int digit_x10[256], digit_x100[256];

    memset(digit_x10, 0, sizeof(digit_x10));
    memset(digit_x100, 0, sizeof(digit_x100));
    for (int i = '0'; i <= '9'; i ++) {
        digit_x10[i] = (i - '0')*10;
        digit_x100[i] = (i - '0')*100;
    }

    pctx = (struct proc_context_s *)data;
    in = pctx->buff_start;
    end = pctx->buff_end;
    for ( ; in < end; ) {
        /* process a line */
        name = in;

        /* name has at least 1 char followed by ';' */
        hash = (in[1] << 7) ^ in[0];
        for (; ; ) {
            if (in[1] == ';') {
                break;
            }
            if (in[2] == ';') {
                in ++;
                break;
            }
            in += 2;
            hash <<= 2;
            hash ^= (in[1] << 7) ^ in[0];
        }
        name_len = in - name + 1;

        /* get the temperature, e.g., -5.2, 32.5 */
        in += 2;
        if (*in == '-') {
            if (in[2] == '.') {
                /* e.g., -5.2 */
                temp = -(digit_x10[(int)in[1]] + in[3] - '0');
                in += 5;
            } else {
                /* e.g., -32.5 */
                temp = -(digit_x100[(int)in[1]] + digit_x10[(int)in[2]] + in[4] - '0');
                in += 6;
            }
        } else {
            if (in[1] == '.') {
                /* e.g., 5.2 */
                temp = digit_x10[(int)in[0]] + in[2] - '0';
                in += 4;
            } else {
                /* e.g., 32.5 */
                temp = digit_x100[(int)in[0]] + digit_x10[(int)in[1]] + in[3] - '0';
                in += 5;
            }
        }
        record_city(pctx, hash, name, name_len, temp);
    }
    return NULL;
}

void merge_result() {
    struct proc_context_s *pctx0, *pctx;
    struct city_s *city0, *city;
    int n, index;

    /* merge to ctx[0] */
    pctx0 = &ctx[0];
    for (n = 1; n < nproc; n ++) {
        pctx = &ctx[n];
        for (index = 0; index < HASH_SIZE; index ++) {
            city = pctx->hash_table[index];
            for (; city; city = city->next) {
                city0 = pctx0->hash_table[index];
                for (; city0; city0 = city0->next) {
                    if (city->hash == city0->hash) {
                        city0->samples += city->samples;
                        city0->sum += city->sum;
                        if (city->min < city0->min)
                            city0->min = city->min;
                        if (city->max > city0->max)
                            city0->max = city->max;
                        break;
                    }
                }
                if (!city0) {
                    city->next = pctx0->hash_table[index];
                    pctx0->hash_table[index] = city;
                    pctx0->entries ++;
                }
            }
        }
    }
}

int cmp_entries(const void *p1, const void *p2) {
    const struct city_s *s1, *s2;
    s1 = *(struct city_s **)p1;
    s2 = *(struct city_s **)p2;
    return (strcmp(s1->name, s2->name));
}

void sort_records(struct proc_context_s *pctx) {
    struct city_s **sorted;
    struct city_s *city;
    int i, n;

    sorted = (struct city_s **)malloc(sizeof(*sorted)*pctx->entries);
    n = 0;
    for (i = 0; i < HASH_SIZE; i ++) {
        city = pctx->hash_table[i];
        for (; city; city = city->next) {
            sorted[n] = city;
            n ++;
        }
    }
    qsort(&sorted[0], pctx->entries, sizeof(struct city_s *), &cmp_entries);
    pctx->sorted = sorted;
}

void dump_records(struct city_s **sorted, int entries) {
    struct city_s *city;
    int n, last;

    printf("{");
    last = entries - 1;
    for (n = 0; n < entries; n ++) {
        city = sorted[n];
        if (n != last)
            printf("%s=%.1f/%.1f/%.1f, ", city->name,
                   (float)city->min/10.0f,
                   (float)city->sum/city->samples/10.0f,
                   (float)city->max/10.0f);
        else
            printf("%s=%.1f/%.1f/%.1f", city->name,
                   (float)city->min/10.0f,
                   (float)city->sum/city->samples/10.0f,
                   (float)city->max/10.0f);
    }
    printf("}\n");
}

int main(const int argc, const char *argv[]) {
    int i;

    if (argc < 2)
        err_exit(-1, "missing file name");

    if (argc >= 3) {
        nproc = atoi(argv[2]);
        if (nproc > MAX_THREADS)
            nproc = MAX_THREADS;
    }

    open_file(argv[1]);

    for (i = 0; i < nproc; i ++)
        pthread_create(&ctx[i].thread_id, NULL, process_data, &ctx[i]);

    for (i = 0; i < nproc; i ++)
        pthread_join(ctx[i].thread_id, NULL);

    merge_result();

    sort_records(&ctx[0]);
    dump_records(ctx[0].sorted, ctx[0].entries);

    close_file();
    return 0;
}
