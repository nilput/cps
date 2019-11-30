#ifndef BNX_H
#define BNX_H
#include "bn.h"
#include <assert.h>

static void bignum_to_string_dec(struct bn* n, char* str, int maxsize) {
    struct bn tmp_n;
    struct bn tmp_q;
    struct bn tmp_r;
    struct bn tmp_zero;
    struct bn tmp_ten;
    bignum_assign(&tmp_q, n);
    bignum_from_int(&tmp_zero, 0);
    bignum_from_int(&tmp_ten, 10);
    int i = 0;
    while (bignum_cmp(&tmp_q, &tmp_zero) == LARGER) {
        
        bignum_assign(&tmp_n, &tmp_q);
        bignum_divmod(&tmp_n, &tmp_ten, &tmp_q, &tmp_r); /* c = a/b, d = a%b */
        if (i >= (maxsize - 1)) {
            goto fail;
        }
        int digit = bignum_to_int(&tmp_r);
        assert(digit < 10);
        str[i] = '0' + digit;
        i++;
    }
    int len = i;
    int hlen = len / 2;
    for (int j=0; j<hlen; j++) {
        int m = len - j - 1;
        char t = str[j];
        str[j] = str[m];
        str[m] = t;
    }
    
    if (len == 0) {
        str[0] = '0';
        i++;
    }
    str[i] = 0;
    return;
fail:
    str[0] = 0;
}
#endif
