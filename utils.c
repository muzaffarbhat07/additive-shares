#include "utils.h"
#include <rand.h>

// generate random K for signing/side-channel noise
void generate_k_random(bignum256 *k, const bignum256 *prime) {
  do {
    int i = 0;
    for (i = 0; i < 8; i++) {
      k->val[i] = random32() & ((1u << BN_BITS_PER_LIMB) - 1);
    }
    k->val[8] = random32() & ((1u << BN_BITS_LAST_LIMB) - 1);
    // check that k is in range and not zero.
  } while (bn_is_zero(k) || !bn_is_less(k, prime));
}


// shift a scalar left by a given number of bits
void scalar_left_shift(const bignum256 *input, int shift, bignum256 *output, const bignum256 *prime) {
    bn_copy(input, output);
    while (shift-- > 0) {
        bn_lshift(output);
        bn_mod(output, prime);
    }
}

// negates a scalar with respect to the prime modulus
void scalar_negate(const bignum256 *input, bignum256 *output, const bignum256 *prime) {
    bn_subtract(prime, input, output);
    bn_mod(output, prime);
}