#include <assert.h>
#include <string.h>

#include "bn.h"
#include "cps.h"
#include "facs.h"

static void swap_u8(uint8_t *a, uint8_t *b) {
    uint8_t t = *a;
    *a = *b;
    *b = t;
}
static void swap_bnp(struct bn **a, struct bn **b) {
    struct bn *t = *a;
    *a = *b;
    *b = t;
}
void int_to_lehmer(uint8_t *inv_table, int nelems, const struct bn *olarge_code) {
    struct bn nums[3];
    struct bn *large_code  = nums + 0;
    struct bn *large_digit = nums + 1;
    struct bn *tmp_1       = nums + 2;
    assert(nelems < 256);
    bignum_assign(large_code, (struct bn *)olarge_code);

    uint32_t small_code = 0;
    int small_code_init = 0;
    for (int i=1; i<=nelems; i++) {
        int perm_idx = nelems - i;
        if (perm_idx <= 11) {
            //use fast computation
            if (!small_code_init) {
                //initialize from large_code during the transition
                small_code = bignum_to_int(large_code);
                small_code_init = 1;
            }
            int multiplier = small_facs[perm_idx];
            int digit = small_code / multiplier;
            small_code = small_code % multiplier;
            assert(digit <= 255u);
            inv_table[i-1] = (uint8_t) digit;
        }
        else {
            //large multiple = factorial(perm_idx)
            const struct bn *large_multiplier = large_facs + perm_idx;
            //large digit = large_code / large_multiplier;
            bignum_div(large_code, (struct bn *)large_multiplier, large_digit);
            int digit = bignum_to_int(large_digit);
            assert(digit <= 255u);
            inv_table[i-1] = (uint8_t) digit;
            //large_code = large_code % large_multiplier (refactor if aliasing is safe?)
            bignum_mod(large_code, (struct bn *)large_multiplier, tmp_1);
            swap_bnp(&large_code, &tmp_1);
        }
    }
}

void int_to_lehmer_i(uint8_t *inv_table, int nelems, int code) {
    struct bn large_code;
    bignum_from_int(&large_code, code);
    int_to_lehmer(inv_table, nelems, &large_code);
}

//abcd -> c abd -> cd ab-> cdb a -> cdba
//{2, 2, 1, 0} -> {2, 3, 1, 0}
void adjust_lehmer_to_inv_table(uint8_t *inv_table, int nelems) {
    for (int i=0; i<nelems; i++) {
        for (int j=i+1; j<nelems; j++) {
            if (inv_table[j] >= inv_table[i]) {
                inv_table[j]++;
            }
        }
    }
}
void permute_array_with_inv_table(uint8_t *out, uint8_t *src, uint8_t *inv_table, int nelems) {
    for (int i=0; i<nelems; i++) {
        out[i] = src[inv_table[i]];
    }
}
void permute_array_with_int(uint8_t *arr, int nelems, struct bn *large_code) {
    uint8_t inv_table[256];
    uint8_t values[256];
    assert(nelems < 256);
    int_to_lehmer(inv_table, nelems, large_code);
    adjust_lehmer_to_inv_table(inv_table, nelems);
    memcpy(values, arr, nelems);
    permute_array_with_inv_table(arr, values, inv_table, nelems);
}
//inplace
void permute_array_with_int_i(uint8_t *arr, int nelems, int code) {
    struct bn large_code;
    bignum_from_int(&large_code, code);
    permute_array_with_int(arr, nelems, &large_code);
}

