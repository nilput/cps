#ifndef CPS_H
#define CPS_H
#include <stdint.h>
#include <assert.h>

#include "bn.h"

void permute_array_with_int_i(uint8_t *arr, int nelems, int code);
void permute_array_with_int(uint8_t *arr, int nelems, struct bn *large_code);
void permute_array_with_inv_table(uint8_t *out, uint8_t *src, uint8_t *inv_table, int nelems);
void adjust_lehmer_to_inv_table(uint8_t *inv_table, int nelems);
void int_to_lehmer_i(uint8_t *inv_table, int nelems, int code);
void int_to_lehmer(uint8_t *inv_table, int nelems, const struct bn *olarge_code);
void adjust_lehmer_to_inv_table(uint8_t *inv_table, int nelems);
void adjust_inv_table_to_lehmer(uint8_t *inv_table, int nelems);
void compute_permutation_number(struct bn *code, uint8_t *arr, int nelems);
int  compute_permutation_number_i(uint8_t *arr, int nelems);

#endif // CPS_H
