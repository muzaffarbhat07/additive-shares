#include "ot.h"
#include "utils.h"

// performs correlated oblivious transfer
void correlated_oblivious_transfer(const ecdsa_curve *curve, const bignum256 *m0, const bignum256 *D, int choice, bignum256 *result) {

  bignum256 m1;
  bn_copy(m0, &m1);
  bn_addmod(&m1, D, &curve->order);

  // if (choice == 0) {
  //     bn_copy(m0, result);
  // } else {
  //     bn_copy(&m1, result);
  // }

  oblivious_transfer(curve, m0, &m1, choice, result);
}

// performs oblivious transfer
void oblivious_transfer(const ecdsa_curve *curve, const bignum256 *m0, const bignum256 *m1, int choice, bignum256 *result) {

  // sender generates a random scalar a, point multiplies it with G and sends it to receiver as point A
  bignum256 a;
  generate_k_random(&a, &curve->order);

  curve_point A;
  point_multiply(curve, &a, &curve->G, &A); // A = a * G

  // receiver generates a random scalar b, point multiplies it with G, adds the point A only if the choice bit is 1, and sends it back to sender as point B
  bignum256 b;
  generate_k_random(&b, &curve->order);

  curve_point B;
  point_multiply(curve, &b, &curve->G, &B); // B = b * G

  if(choice == 1) {
    point_add(curve, &A, &B); // B = A + B
  }

  // sender prepares two keys for encryption, k0 and k1
  bignum256 k0, k1;
  compute_std_hash_key(curve, &B, &a, &k0); // k0 = hash((a * B).x)

  curve_point B_minus_A;
  point_subtract(curve, &B, &A, &B_minus_A); // B - A
  compute_std_hash_key(curve, &B_minus_A, &a, &k1); // k1 = hash((a * (B - A)).x)


  // receiver generates a decryption key kc by point multiplying b with A and taking the abscissa.
  bignum256 kc;
  compute_std_hash_key(curve, &A, &b, &kc); // kc = hash((b * A).x)

  // sender encrypts the messages m0 and m1 using k0 and k1
  bignum256 e0, e1;
  bn_xor(&e0, m0, &k0);
  bn_xor(&e1, m1, &k1);

  // receiver decrypts the message using kc
  if(choice == 0) {
    bn_xor(result, &e0, &kc);
  } else {
    bn_xor(result, &e1, &kc);
  }
}

// computes a standard hash key; the key is the sha256 hash of the x-coordinate of the point obtained by multiplying the curve point cp with the scalar r
void compute_std_hash_key(const ecdsa_curve *curve, const curve_point* cp, bignum256 *r, bignum256 *key) {
  curve_point r_cp;
  point_multiply(curve, r, cp, &r_cp); // r_cp = r * cp

  bignum256 x;
  bn_copy(&r_cp.x, &x); // x = r_cp.x

  // hash x to get key
  uint8_t temp1[32], temp2[32];
  bn_write_be(&x, temp1);
  sha256_Raw(temp1, 32, temp2);
  bn_read_be(temp2, key);
}

// subtracts two points on the curve
void point_subtract(const ecdsa_curve *curve, const curve_point *cp1, const curve_point *cp2, curve_point *result) {
  point_negate(curve, cp2, result);
  point_add(curve, cp1, result);
}

// negates a point on the curve
void point_negate(const ecdsa_curve *curve, const curve_point *cp, curve_point *result) {
  // if P == (x, y), then -P would be (x, -y) on this curve
  scalar_negate(&cp->y, &result->y, &curve->prime);
  bn_copy(&cp->x, &result->x);
}