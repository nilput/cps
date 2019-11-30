#ifndef XPS_H
#define XPS_H

void bignum_to_bytes(uint8_t *arr, int nelems, struct bn *num);
void bytes_to_bignum(uint8_t *arr, int nelems, struct bn *num);
int bits_in_fac(int n);
void xps_dec(uint8_t *xps_data, int xps_data_len, int nelems, uint8_t *set);
void xps_enc(uint8_t *set, int nelems, uint8_t *output, int *output_len);

#endif
