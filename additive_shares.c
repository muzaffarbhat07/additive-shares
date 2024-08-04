#include "additive_shares.h"
#include "ot.h"
#include "utils.h"

// generates additive shares of x and y: x * y = U + V
void generate_additive_shares(const ecdsa_curve *curve, const bignum256 *x, const bignum256 *y, bignum256 *U, bignum256 *V) {

  // Initialize additive shares to zero
  bn_zero(U);
  bn_zero(V);

  // iterate over the bits of y and compute the additive shares: U = -summation(Ui * 2^i), V = summation(mci * 2^i)
  uint8_t y_bytes[32];
  bn_write_be(y, y_bytes);

  bignum256 Ui, mci, Ui_multipled_2_pow_i, mci_multipled_2_pow_i;
  for (int i = 31; i >= 0; i--) {
    for (int j = 0; j < 8; j++) {

      // get the choice bit
      int choice = (y_bytes[i] >> j) & 1;

      int current_bit_number = (31 - i) * 8 + j;

      // generate random scalar Ui
      generate_k_random(&Ui, &curve->order);

      // perform correlated oblivious transfer with m0 = Ui, D = x, mci = Ui + yi(choice) * x
      correlated_oblivious_transfer(curve, &Ui, x, choice, &mci);

      // Compute U = U + Ui * 2^current_bit_number
      scalar_left_shift(&Ui, current_bit_number, &Ui_multipled_2_pow_i, &curve->order);
      bn_addmod(U, &Ui_multipled_2_pow_i, &curve->order);

      // Compute V = V + mci * 2^current_bit_number
      scalar_left_shift(&mci, current_bit_number, &mci_multipled_2_pow_i, &curve->order);
      bn_addmod(V, &mci_multipled_2_pow_i, &curve->order);
    }
  }

  // negate U
  scalar_negate(U, U, &curve->order);
}

// verifies additive shares of x and y: x * y == U + V
bool verify_additive_shares(const ecdsa_curve *curve, const bignum256 *x, const bignum256 *y, bignum256 *U, bignum256 *V, bignum256 *product, bignum256 *sum) {
  
  // calculate product = x * y
  bn_copy(x, product);
  bn_multiply(y, product, &curve->order);

  // calculate sum = U + V
  bn_copy(U, sum);
  bn_addmod(sum, V, &curve->order);

  // compare U + V and x * y
  return bn_is_equal(product, sum);
}