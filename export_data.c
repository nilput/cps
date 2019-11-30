#include "bn.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * generating facs.h
const static uint32_t small_facs [12] = {
};
//array of uint32_t
const static struct bn large_facs [256] = {
};
*/


static uint32_t small_facs [13] = {0};
static struct bn large_facs [256] = {0};
static int nbits_facs[256] = {0};

static void die(char *msg) {
    fprintf(stderr, "Fatal Error: %s\n", msg);
    exit(1);
}

void generate() {
    struct bn counter;

    small_facs[0] = 1;
    bignum_from_int(large_facs + 0, 1);

    bignum_from_int(&counter, 1);
    for (int i=1; i<256; i++) {
        if (i <= 12) {
            small_facs[i] = small_facs[i-1] * i;
        }
        bignum_mul(large_facs + i - 1, &counter, large_facs + i);
        bignum_inc(&counter);
    }
    for (int i=0; i<256; i++) {
        struct bn tmp_a;
        struct bn tmp_b;
        bignum_assign(&tmp_a, large_facs + i);
        int nbits = 0;
        while (!bignum_is_zero(&tmp_a)) {
            bignum_rshift(&tmp_a, &tmp_b, 1); /* b = a >> nbits */
            bignum_assign(&tmp_a, &tmp_b);
            nbits++;
        }
        nbits_facs[i] = nbits;
    }
}

void export(char *outfile) {
    FILE *f = fopen(outfile, "wb");
    if (!f) {
        die("Can't write to file");
    }
    fprintf(f, "#ifndef FACS_H\n");
    fprintf(f, "#define FACS_H\n");
    fprintf(f, "const static uint32_t small_facs [13] = {\n");
    int line_at_count = 0;
    int line_at = 4;
    for (int i=0; i<=12; i++) {
        if (!(line_at_count++ % line_at))
            fprintf(f, "\n");
        fprintf(f, "% 11lu, ", (unsigned long) small_facs[i]);
    }
    fprintf(f, "\n};");

    line_at_count = 0;
    line_at = 4;
    fprintf(f, "const static struct bn large_facs [256] = {\n");
    for (int i=0; i<256; i++) {
        for (int j=0; j<BN_ARRAY_SIZE; j++) {
            if (!(line_at_count++ % line_at))
                fprintf(f, "\n");
            fprintf(f, "% 11lu, ", (unsigned long) large_facs[i].array[j]);
        }
        fprintf(f, "\n\n");
    }
    fprintf(f, "};\n");

    line_at_count = 0;
    line_at = 8;
    fprintf(f, "const static uint32_t nbits_facs [256] = {");
    for (int i=0; i<256; i++) {
        if (!(line_at_count++ % line_at))
            fprintf(f, "\n");
        fprintf(f, "% 5lu, ", (unsigned long) nbits_facs[i]);
    }
    fprintf(f, "\n};\n");

    fprintf(f, "#endif //FACS_H");
    fclose(f);
}


int main(int argc, char *argv[]) {
    int do_export = 1;
    char *outfile = "facs.h";
    generate();
    if (do_export) {
        export(outfile);
    }
}
