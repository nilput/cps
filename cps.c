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
    uint8_t count_arr[256];
    for (int i=0; i<nelems; i++) {
        count_arr[i] = i;
    }
    for (int i=0; i<nelems; i++) {
        int idx = inv_table[i];
        int final_idx = count_arr[idx];
        inv_table[i] = final_idx;
        //remove count_arr[idx]
        int count_len = nelems - i - 1;
        for (int j=idx; j<count_len; j++) {
            count_arr[j] = count_arr[j+1];
        }
    }
}
//{2, 3, 1, 0} -> {2, 2, 1, 0}
void adjust_inv_table_to_lehmer(uint8_t *inv_table, int nelems) {
    for (int i=nelems-1; i>=0; i--) {
        for (int j=i-1; j>=0; j--) {
            if (inv_table[j] <= inv_table[i]) {
                inv_table[i]++;
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

static int qk_array_sort(uint8_t *arr, int nelems)
{
    if (nelems == 0)
        return 0;
    assert(nelems > 0);
    //  This public-domain C implementation was written by Darel Rex Finley.
    #define  MAX_LEVELS  1000 /*typically 1000 * 2 * 8 bytes, that's 1.6% of the stack space assuming 1MB*/
    #define CMP(a, b) (a - b)
    //needs to be modified: elements, arr, CMP and piv's type
    long elements = nelems;
    uint8_t piv;

    long beg[MAX_LEVELS];
    long end[MAX_LEVELS];
    long i=0;
    long L;
    long R;

    beg[0]=0;
    end[0]=elements;
    while (i >= 0) {
        L = beg[i];
        R = end[i]-1;
        if (L < R) {
            piv = arr[L]; 
            if (i == MAX_LEVELS-1)
                return 1;
            while (L<R) {
                while ((CMP(arr[R], piv) >= 0) && L<R)
                    R--;
                if (L < R) 
                    arr[L++] = arr[R];
                while ((CMP(arr[L], piv) <= 0) && L<R)
                    L++;
                if (L < R)
                    arr[R--] = arr[L]; 
            }
            arr[L] = piv;
            beg[i+1] = L+1;
            end[i+1] = end[i];
            end[i++] = L; 
        }
        else {
            i--; 
        }
    }
    return 0; 
    #undef  MAX_LEVELS
    #undef CMP
}

static long array_bsearch(uint8_t *arr, long beg, long end, uint8_t x) {
    long len = end;
    long m;
    int r;
    while (beg < end) {
        m = beg + (end - beg) / 2;
        r = arr[m] - x;
        if (r == 0) 
            return m;
        else if (r > 0) 
            end = m;
        else
            beg = m + 1;
    }
    return -1;
}

void compute_permutation_number(struct bn *code, uint8_t *arr, int nelems) {
    uint8_t sorted[256];
    memcpy(sorted, arr, nelems);
    qk_array_sort(sorted, nelems);
    uint8_t inv_table[256];
    for (int i=0; i<nelems; i++) {
        int idx = array_bsearch(sorted, 0, nelems, arr[i]);
        assert(idx != -1);
        inv_table[i] = idx;
    }
    printf("\tInversion table:"); print_arr_n(inv_table, nelems);
}
int  compute_permutation_number_i(uint8_t *arr, int nelems) {
    struct bn large_code;
    compute_permutation_number(&large_code, arr, nelems);
    int code = bignum_to_int(&large_code);
    return code;
}
