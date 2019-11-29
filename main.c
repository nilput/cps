#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cps.h"


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
    }
}

int main(int argc, char *argv[]) {
    tst();
}
