#pragma once

#include "bignum.h"
#include "ecdsa.h"

/**
  * @brief: generates additive shares of x and y: x * y = U + V
  * @param curve: the elliptic curve
  * @param x: the first input number
  * @param y: the second input number
  * @param U: the resultant first additive share
  * @param V: the reaultant second additive share

*/
void generate_additive_shares(const ecdsa_curve *curve, const bignum256 *x, const bignum256 *y, bignum256 *U, bignum256 *V);

/**
  * @brief: verifies additive shares of x and y: x * y == U + V
  * @param curve: the elliptic curve
  * @param U: the first additive share
  * @param V: the second additive share
  * @param x: the first multiplicative share
  * @param y: the second multiplicative share
  * @param product: the product of x and y
  * @param sum: the sum of U and V
  * @return: true if the shares are valid, false otherwise
*/
bool verify_additive_shares(const ecdsa_curve *curve, const bignum256 *x, const bignum256 *y, bignum256 *U, bignum256 *V, bignum256 *product, bignum256 *sum);