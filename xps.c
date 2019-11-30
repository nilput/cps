//how many can be stored
#include <string.h>
#include <assert.h>
#include "bn.h"
#include "facs.h"
#include "cps.h"

//number of bits of factorial(n)
int bits_in_fac(int n) {
    assert(n < 256u);
    return nbits_facs[n];
}

void bytes_to_bignum(uint8_t *arr, int nelems, struct bn *num) {
    bignum_init(num);
    memcpy(&num->array, arr, nelems);
}
void bignum_to_bytes(uint8_t *arr, int nelems, struct bn *num) {
    memcpy(arr, &num->array, nelems);
}

int byte_savings(int nelems) {
    int max_saved = 0;
    int max_at_nbytes = nelems;
    for (int i=nelems; i>0; i--) {
        int capacity = i + (bits_in_fac(i) / 8);
        if (capacity < nelems)
            break;
        int saved = nelems - i;
        if (saved > max_saved) {
            max_saved = saved;
            max_at_nbytes = i;
        }
    }
    return max_at_nbytes;
}

void xps_enc(uint8_t *set, int nelems, uint8_t *output, int *output_len) {
    assert(nelems < 256u);
    int output_nbytes = byte_savings(nelems);
    int bytes_saved = nelems - output_nbytes;
    struct bn perm_num;
    //take last n bytes and interpret them as a permutation
    bytes_to_bignum(set + nelems - bytes_saved, bytes_saved, &perm_num);
    memcpy(output, set, output_nbytes);
    qk_array_sort(output, output_nbytes);
    permute_array_with_int(output, output_nbytes, &perm_num);
    *output_len = output_nbytes;
}
//nelements is the number of elements in the set which can be larger than xps_data_len
void xps_dec(uint8_t *xps_data, int xps_data_len, int nelems, uint8_t *set) {
    int output_nbytes = byte_savings(nelems);
    int bytes_saved = nelems - output_nbytes;
    assert(xps_data_len == output_nbytes);
    struct bn perm_num;
    compute_permutation_number(&perm_num, xps_data, xps_data_len);
    memcpy(set, xps_data, xps_data_len);
    bignum_to_bytes(set + xps_data_len, bytes_saved,  &perm_num);
}
