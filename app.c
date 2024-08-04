#include "utils.h"
#include "additive_shares.h"
#include "secp256k1.h"
#include <stdio.h>

const ecdsa_curve *curve = &secp256k1;

/** 
  * @brief: prints a scalar in hex to stdout
  * @param num: the scalar to print
*/
void print_scalar(const bignum256 *num);

int main() {
  printf("\nGenerate additive shares of multiplication of two random 32-byte numbers using the correlated oblivious transfers\n");

  bignum256 a, b, c, d;

  printf("\nDo you want to enter the numbers manually? (y/n): ");
  char choice;
  scanf(" %c", &choice);
  if (choice == 'y') {
    // read the numbers from the user
    printf("Enter the numbers in hex format\n");
    uint8_t buffer[32];
    printf("Enter a: ");
    for (int i = 0; i < 32; i++) {
        scanf("%02hhx", &buffer[i]);
    }
    bn_read_be(buffer, &a);
    printf("Enter b: ");
    for (int i = 0; i < 32; i++) {
        scanf("%02hhx", &buffer[i]);
    }
    bn_read_be(buffer, &b);
    printf("\n");
  } else {
    // generate random numbers a and b
    printf("\nGenerating random numbers a and b...\n");
    generate_k_random(&a, &curve->order);
    generate_k_random(&b, &curve->order);
  }


  // print the random scalars
  printf("Random Scalar a: ");
  print_scalar(&a);
  printf("Random Scalar b: ");
  print_scalar(&b);

  // generate additive shares of a and b
  printf("\nGenerating additive shares of a and b...\n");
  generate_additive_shares(curve, &a, &b, &c, &d);

  // print the additive shares
  printf("Additive Share c: ");
  print_scalar(&c);
  printf("Additive Share d: ");
  print_scalar(&d);


  // verify the additive shares generated
  printf("\nVerifying additive shares...\n");
  bignum256 product_ab, sum_cd;
  bool result = verify_additive_shares(curve, &a, &b, &c, &d, &product_ab, &sum_cd);
  if (result) {
      printf("\nAdditive shares verified successfully: a * b equals c + d\n\n");
  } else {
      printf("\nAdditive shares verification failed: a * b NOT equals c + d\n\n");
  }

  // print the product and the sum
  printf("Product(a * b): ");
  print_scalar(&product_ab);
  printf("Sum(c + d): ");
  print_scalar(&sum_cd);

  printf("\n");
  return 0;
}

// prints a scalar in hex to stdout
void print_scalar(const bignum256 *num) {
    uint8_t buffer[32];
    bn_write_be(num, buffer);
    for (int i = 0; i < 32; i++) {
        printf("%02x", buffer[i]);
    }
    printf("\n");
}