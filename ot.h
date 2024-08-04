#pragma once

#include "bignum.h"
#include "ecdsa.h"

/**
  * @brief: performs correlated oblivious transfer
  * @param curve: the elliptic curve
  * @param m0: the first message
  * @param D: the correlation between the two messages
  * @param choice: the choice bit
  * @param result: the result of the oblivious transfer: decrypted message based on the choice bit i.e., m0 if choice is 0, m1(m0 + D) if choice is 1
*/
void correlated_oblivious_transfer(const ecdsa_curve *curve, const bignum256 *m0, const bignum256 *D, int choice, bignum256 *result);

/**
  * @brief: performs oblivious transfer
  * @param curve: the elliptic curve
  * @param m0: the first message
  * @param m1: the second message
  * @param choice: the choice bit
  * @param result: the result of the oblivious transfer: decrypted message based on the choice bit i.e., m0 if choice is 0, m1 if choice is 1
*/
void oblivious_transfer(const ecdsa_curve *curve, const bignum256 *m0, const bignum256 *m1, int choice, bignum256 *result);

/**
  * @brief: computes the standard sha256 hash key from a given curve point and a scalar: key = hash((r * cp).x)
  * @param curve: the elliptic curve
  * @param cp: the curve point
  * @param r: the scalar
  * @param key: the resultant key
*/
void compute_std_hash_key(const ecdsa_curve *curve, const curve_point* cp, bignum256 *r, bignum256 *key);

/** 
  * @brief: subtracts two curve points on the elliptic curve: result = cp1 - cp2
  * @param curve: the elliptic curve
  * @param cp1: the first curve point
  * @param cp2: the second curve point
  * @param result: the resultant curve point
*/
void point_subtract(const ecdsa_curve *curve, const curve_point *cp1, const curve_point *cp2, curve_point *result);

/** 
  * @brief: negates a curve point on the elliptic curve: result = -cp
  * @param curve: the elliptic curve
  * @param cp: the curve point
  * @param result: the resultant curve point
*/
void point_negate(const ecdsa_curve *curve, const curve_point *cp, curve_point *result);
