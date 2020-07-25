#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cps.h"
#include "bnx.h"
#include "xps.h"
#include "vg.h"


void die(char *msg) {
    fprintf(stderr, "Error: %s\n", msg);
    exit(1);
}
void print_arr_n(uint8_t *arr, int len) {
    printf("{");
    for (int i=0; i<len; i++) {
        if (i)
            printf(", ");
        printf("%u", (unsigned) arr[i]);
    }
    printf("}\n");
}
void print_arr_chars(uint8_t *arr, int len) {
    printf("{");
    for (int i=0; i<len; i++) {
        if (i)
            printf(", ");
        printf("%c", (char) arr[i]);
    }
    printf("}\n");
}
void tst() {
    uint8_t c[6];
    int clen = sizeof c / sizeof c[0];
    uint8_t inv_table[256];
    for (int i=0; i<720; i++) {
        printf("Permutation %i/720: \n", i);
        int_to_lehmer_i(inv_table, 6, i);
        printf("\tLehmer:          "); print_arr_n(inv_table, 6);
        adjust_lehmer_to_inv_table(inv_table, 6);
        printf("\tInversion table: "); print_arr_n(inv_table, 6);
        adjust_inv_table_to_lehmer(inv_table, 6);
        printf("\tLehmer(inv_table):"); print_arr_n(inv_table, 6);

        memcpy(c, (uint8_t[6]){'a','b','c','d','e','f'}, 6);
        permute_array_with_int_i(c, 6, i);
        printf("\tAlphabet:         "); print_arr_chars(c, 6);
        int perm_num = compute_permutation_number_i(c, 6);
        printf("\tPermutation number: %d\n", perm_num);
        assert(perm_num == i);
    }
}

int main(int argc, char *argv[]) {
    
    struct vgstate vg;
    vg_init(&vg, argc, argv);
    if (vg_get_boolean(&vg, "--test-cps")) {
        tst();
    }
    else if (vg_get_boolean(&vg, "--encode")) {
        uint8_t data[256];;
        int data_len = fread(data, 1, 256, stdin);
        if (data_len > 0 && data[data_len - 1] == '\n') data_len--;
        if (data_len > 0 && data[data_len - 1] == '\r') data_len--;
        uint8_t xps_data[256];;
        int xps_data_len;
        xps_enc(data, data_len, xps_data, &xps_data_len);
        int len_len = fprintf(stdout, "%d.", data_len);
        fwrite(xps_data, 1, xps_data_len, stdout);
        int output_size = xps_data_len;
        fflush(stdout);
        fprintf(stderr, "\n"
                        "Input size:  %d\n"
                        "Output size: %d\n"
                        "Saving:      %.2f%%\n", data_len, len_len + xps_data_len, 100.0 * (1.0 - ((double) output_size / data_len)));
    }
    else if (vg_get_boolean(&vg, "--decode")) {
        uint8_t in_data[257];;
        int input_len = fread(in_data, 1, 256, stdin);
        uint8_t *xps_data = in_data;
        for (int i=0; i<4 && *xps_data != '.'; i++, xps_data++)
            ;
        if (*(xps_data++) != '.')
            die("invalid input!");
        if (input_len > 0 && in_data[input_len - 1] == '\n') input_len--;
        if (input_len > 0 && in_data[input_len - 1] == '\r') input_len--;
        assert(input_len >= 1);
        int xps_data_len = input_len - (xps_data - in_data);
        uint8_t set_len = atoi(in_data);
        uint8_t data[256];
        xps_dec(xps_data, xps_data_len, set_len, data);
        qk_array_sort(data, set_len);
        fwrite(data, 1, set_len, stdout);
    }
    else {
        printf(""
"       Options:\n"
"       --encode        read a set of bytes from stdin, writing encoded data to stdout\n"
"       --decode        read a set of bytes from stdin, writing decoded data to stdout\n"
"       --test-cps      test permutations module\n"
        );
    }
}
