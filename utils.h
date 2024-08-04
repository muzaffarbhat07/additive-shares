#pragma once

#include "bignum.h"

/** 
  * @brief: generates a random number k
  * @param k: the random number
  * @param prime: the prime modulus
*/
void generate_k_random(bignum256 *k, const bignum256 *prime);

/** 
  * @brief: shifts a scalar left by a given number of bits
  * @param input: the input scalar
  * @param shift: the number of bits to shift
  * @param output: the resultant scalar
  * @param prime: the prime modulus
*/
void scalar_left_shift(const bignum256 *input, int shift, bignum256 *output, const bignum256 *prime);

/** 
  * @brief: negates a scalar with respect to the prime modulus
  * @param input: the input scalar
  * @param output: the resultant scalar
  * @param prime: the prime modulus
*/
void scalar_negate(const bignum256 *input, bignum256 *output, const bignum256 *prime);

