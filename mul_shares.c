#include <stdio.h>
#include "utils.h"
#include "additive_shares.h"
#include "secp256k1.h"


// #include <stdlib.h>
// #include <openssl/rand.h>
// #include <rand.h>
// #include "bignum.h"
// #include "ecdsa.h"

int main() {
    printf("Hello, World!\n");

    bignum256 a, b, c, d;
    generate_k_random(&a, &secp256k1.order);
    generate_k_random(&b, &secp256k1.order);

    generate_additive_shares(&a, &b, &c, &d);
    bool result = verify_additive_shares(&c, &d, &a, &b);
    if (result) {
        printf("Additive shares verified\n");
    } else {
        printf("Additive shares verification failed\n");
    }
    return 0;
}


/*
// generate random K for signing/side-channel noise
static void generate_k_random(bignum256 *k, const bignum256 *prime) {
  do {
    int i = 0;
    for (i = 0; i < 8; i++) {
      k->val[i] = random32() & ((1u << BN_BITS_PER_LIMB) - 1);
    }
    k->val[8] = random32() & ((1u << BN_BITS_LAST_LIMB) - 1);
    // check that k is in range and not zero.
  } while (bn_is_zero(k) || !bn_is_less(k, prime));
}

// Function to generate a random 256-bit number in the range of the curve order
void generate_random_scalar(bignum256 *num, const bignum256 *prime) {
    uint8_t buffer[32];
    do {
        if (RAND_bytes(buffer, 32) != 1) {
            fprintf(stderr, "Error generating random number\n");
            exit(1);
        }
        bn_read_be(buffer, num);
        bn_fast_mod(num, prime);
    } while (bn_is_zero(num));
}

// Function to print a scalar
void print_scalar(const bignum256 *num) {
    uint8_t buffer[32];
    bn_write_be(num, buffer);
    for (int i = 0; i < 32; i++) {
        printf("%02x", buffer[i]);
    }
    printf("\n");
}

// Function to perform OT and return the result based on the choice bit
void correlated_oblivious_transfer(const bignum256 *m0, const bignum256 *D, int choice, bignum256 *result) {
    bignum256 m1;
    bn_copy(m0, &m1);
    bn_addmod(&m1, D, &secp256k1.order);

    if (choice == 0) {
        bn_copy(m0, result);
    } else {
        bn_copy(&m1, result);
    }

    //oblivious_transfer(m0, &m1, choice, result);
}

// Function to shift a scalar left by a given number of bits
void scalar_left_shift(const bignum256 *input, int shift, bignum256 *output) {
    bn_copy(input, output);
    while (shift-- > 0) {
        bn_lshift(output);
        // bn_fast_mod(output, &secp256k1.order);
        // bn_mod(output, &secp256k1.order);
    }
}

// Function to negate a scalar (modular negation)
void bn_mod_negate(const bignum256 *input, bignum256 *output, const bignum256 *modulus) {
    if (bn_is_zero(input)) {
        bn_zero(output);
    } else {
        bn_subtract(modulus, input, output);
    }
}

int main() {
    printf("\nPrint secp256k1 specifics...\n");
    print_scalar(&secp256k1.prime);
    print_scalar(&secp256k1.order);
    print_scalar(&secp256k1.b);
    printf("secp256k1: %d\n", secp256k1.a);


    bignum256 a, b, c, d, temp, ci, shifted, neg_ui;

    // Generate random scalars a and b
    printf("\nGenerate random scalars a and b...\n");
    generate_k_random(&a, &secp256k1.order);
    generate_k_random(&b, &secp256k1.order);

    // Print the random scalars
    printf("\nPrint the random scalars...\n");
    printf("Random Scalar a: ");
    print_scalar(&a);
    printf("Random Scalar b: ");
    print_scalar(&b);

    // Initialize additive shares to zero
    printf("\n\nInitialize additive shares to zero...\n");
    bn_zero(&c);
    bn_zero(&d);

    // Iterate over bits of b and perform OT for each bit
    printf("\n\nIterate over bits of b and perform OT for each bit...\n");
    uint8_t b_bytes[32];
    bn_write_be(&b, b_bytes);
    // for (int i = 0; i < 32; i++) {
    //     for (int j = 7; j >= 0; j--) {
    for (int i = 31; i >= 0; i--) {
        for (int j = 0; j < 8; j++) {

            generate_k_random(&ci, &secp256k1.order);
            int choice = (b_bytes[i] >> j) & 1;
            correlated_oblivious_transfer(&ci, &a, choice, &temp);

            // scalar_left_shift(&ci, (31 - i) * 8 + j, &shifted);
            // bn_subtractmod(&c, &shifted, &c, &secp256k1.order);
            // bn_subtract(&c, &shifted, &c);

            // // Compute result2 = result2 - ui * 2^i
            bn_mod_negate(&ci, &neg_ui, &secp256k1.order); // Negate ui
            scalar_left_shift(&neg_ui, (31 - i) * 8 + j, &shifted);
            bn_addmod(&c, &shifted, &secp256k1.order);

            scalar_left_shift(&temp, (31 - i) * 8 + j, &shifted);
            bn_addmod(&d, &shifted, &secp256k1.order);
        }
    }

    // Print the final summation c
    printf("\n\n\nPrint Results...\n");
    printf("c: ");
    print_scalar(&c);
    printf("d: ");
    print_scalar(&d);


    // VERIFICATION
    printf("\n\n\nVerify Result...\n");
    bignum256 product;
    // Calculate the product of a and b
    printf("\nCalculate the product of a and b...\n");
    bn_copy(&a, &product);
    bn_multiply(&b, &product, &secp256k1.order);

    // Print the product
    printf("Product of a and b: ");
    print_scalar(&product);

    // Verify that c + d = product
    bignum256 verify;
    printf("\n\nAdd c and d...\n");
    bn_copy(&c, &verify);
    bn_addmod(&verify, &d, &secp256k1.order);
    printf("Verification (c + d): ");
    print_scalar(&verify);

    // Check if verification matches the product
    printf("\n\n\nCheck if verification matches the product...\n");
    if (bn_is_equal(&verify, &product)) {
        printf("Verification successful: a * b equals c + d\n\n");
    } else {
        printf("Verification failed: a * b NOT equals c + d\n\n");
    }

    return 0;
}





// // Function to hash a scalar using SHA-256
// void hash_scalar(const bignum256 *num, uint8_t *hash) {
//     uint8_t buffer[32];
//     bn_write_be(num, buffer);
//     SHA256(buffer, 32, hash);
// }

// // Simplified 1-out-of-2 Oblivious Transfer protocol
void oblivious_transfer(const bignum256 *m0, const bignum256 *m1, int choice, bignum256 *output) {
// void oblivious_transfer(const uint8_t *m0, const uint8_t *m1, int choice, uint8_t *output) {
    // Sender's step
    bignum256 a, b, k, r, k0, k1, kc;
    uint8_t K0[32], K1[32], H0[32], H1[32], Hc[32];
    uint8_t E0[32], E1[32], D0[32], D1[32];

    // Generate a random scalar a
    generate_k_random(&a, &secp256k1.order);

    // Compute A = a * G
    curve_point A;
    point_multiply(&secp256k1, &a, &secp256k1.G, &A);

    // Print A (for debugging)
    // uint8_t A_bytes[65];
    // ecdsa_get_public_key65(&SECP256K1, &k, A_bytes);
    // printf("A: ");
    // for (int i = 0; i < 65; i++) {
    //     printf("%02x", A_bytes[i]);
    // }
    // printf("\n");

    // // Receiver's step
    // if (choice == 0) {
    //     generate_random_scalar(&r, &secp256k1.order);
    //     point_multiply(&r, &SECP256K1_G, &R); // R' = r * G
    //     bn_addmod(&k, &r, &k0, &secp256k1.order); // k0 = k + r
    //     k1 = k; // k1 = k
    // } else {
    //     generate_random_scalar(&r, &secp256k1.order);
    //     point_multiply(&r, &SECP256K1_G, &R); // R' = r * G
    //     k0 = k; // k0 = k
    //     bn_addmod(&k, &r, &k1, &secp256k1.order); // k1 = k + r
    // }

    // Receiver's step
    // Generate a random scalar b
    generate_k_random(&b, &secp256k1.order);
    curve_point B;
    if (choice == 0) {
        point_multiply(&secp256k1, &b, &secp256k1.G, &B); // B = b * G
    } else {
        point_multiply(&secp256k1, &b, &secp256k1.G, &B); // B = b * G

        // add R to B if choice is 1
        // curve_point temp;
        // ecdsa_read_pubkey(&SECP256K1, R_bytes, &temp);
        point_add(&secp256k1, &A, &B);
    }

    // Print B (for debugging)
    // uint8_t B_bytes[65];
    // ecdsa_get_public_key65(&SECP256K1, &r, B_bytes);
    // printf("B: ");
    // for (int i = 0; i < 65; i++) {
    //     printf("%02x", B_bytes[i]);
    // }
    // printf("\n");

    // receiver computes kr
    curve_point KC;
    point_multiply(&r, &R, &KC);
    ecdsa_get_public_key65(&SECP256K1, &r, R_bytes); // @TODO


    // sender computes k0 and k1
    curve_point S0, S1;
    point_multiply(&k, &B, &S0);
    // get absciassa of S0 and store in k0
    ecdsa_get_public_key65(&SECP256K1, &k, K0);// @TODO

    // subtract R from B to get S1
    curve_point temp;
    ecdsa_read_pubkey(&SECP256K1, R_bytes, &temp);// @TODO
    point_sub(&B, &temp, &S1);
    // get absciassa of S1 and store in k1
    ecdsa_get_public_key65(&SECP256K1, &k, K1);// @TODO

    
    // Hash the keys
    hash_scalar(&k0, H0);
    hash_scalar(&k1, H1);
    hash_scalar(&kr, Hr);

    // XOR(encrypt) the messages with the hashes and store in result
    uint8_t m0_bytes[32], m1_bytes[32], result_bytes[32];
    bn_write_be(m0, m0_bytes);
    bn_write_be(m1, m1_bytes);
    for (int i = 0; i < 32; i++) {
        E0[i] = m0[i] ^ H0[i];
        E1[i] = m1[i] ^ H1[i];
    }

    // Print the encrypted messages (for debugging)
    printf("E0: ");
    for (int i = 0; i < 32; i++) {
        printf("%02x", E0[i]);
    }
    printf("\n");
    printf("E1: ");
    for (int i = 0; i < 32; i++) {
        printf("%02x", E1[i]);
    }
    printf("\n");


    // // Receiver's final step
    // if (choice == 0) {
    //     for (int i = 0; i < 32; i++) {
    //         output[i] = K0[i] ^ H0[i];
    //     }
    // } else {
    //     for (int i = 0; i < 32; i++) {
    //         output[i] = K1[i] ^ H1[i];
    //     }
    // }

    // Receiver's final step
    for (int i = 0; i < 32; i++) {
        D0[i] = E0[i] ^ Hr[i];
        D1[i] = E1[i] ^ Hr[i];
    }

    // Determine which message is correct based on the choice bit
    if (choice == 0) {
        for (int i = 0; i < 32; i++) {
            output[i] = D0[i];
        }
    } else {
        for (int i = 0; i < 32; i++) {
            output[i] = D1[i];
        }
    }
}












// // Function to generate a random 32-byte number
// void generate_random_32byte_number(unsigned char *number) {
//     if (RAND_bytes(number, 32) != 1) {
//         fprintf(stderr, "Error generating random number\n");
//         exit(1);
//     }
// }

// // Function to print 32-byte number
// void print_32byte_number(const unsigned char *number) {
//     for (int i = 0; i < 32; i++) {
//         printf("%02x", number[i]);
//     }
//     printf("\n");
// }



  // unsigned char a[32], b[32];
  // unsigned char product[32];
  // unsigned char c[32], d[32], ci[32];

  // // Generate random 32-byte numbers a and b
  // generate_random_32byte_number(a);
  // generate_random_32byte_number(b);

  // // Print the random numbers
  // printf("Random Number a: ");
  // print_32byte_number(a);
  // printf("Random Number b: ");
  // print_32byte_number(b);

  // // Iterate over bits of b and perform OT for each bit
  // // uint8_t b_bytes[32];
  // // bn_write_be(&b, b_bytes);
  // for (int i = 31; i >= 0; i--) {
  //     for (int j = 0; j < 8; j++) {
  //         int choice = (b[i] >> j) & 1;
  //         // oblivious_transfer(&a, choice, &temp);
  //         // bn_addmod(&result, &temp, &secp256k1.order);
  //         generate_random_32byte_number(ci);

  //         // add a to ci if choice is 1
          
  //     }
  // }
  */