/**
 * Shamir Secret Sharing implementation
 * This library contains the functions for sharing and reconstructing a secret
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h> // for seeding with nanoseconds using clock_gettime
#include "sss.h"

#define NEG_MOD(x) ((x % SSS_P) + SSS_P)

static int power(int base, int p) {
  int n;

  for (n = 1; p > 0; p--) {
    n = (n * base) % SSS_P;
  }

  return n;
}

static int evaluate_poly(int x, int poly_coeffs[], int degree)
{
  int i, res;

  res = 0;
  for (i = 0; i < degree + 1; i++) {
    res += (poly_coeffs[i] * power(x, i)) % SSS_P;
    res %= SSS_P;
  }

  return res;
}

/**
 * Calculate the multiplicative inverse of x with mod SSS_P.
 * This is necessary for dividing any number by x.
 * 
 * It uses the extended euclidean algorithm EEA.
 * EEA will calculate s and t which satisfy this linear equation
 * GCD(a, b) = s*a + t*b
 * Because we use b=SSS_P --> s= multiplicative inverse of a
 * since
 * GCD(a, p) = 1 (mod p)
 * -> 1 = s*a + t*p (mod p)
 * -> 1 = s*a (mod p)  --> (because t*p mod p = 0)
 * -> That's how a multiplicative inverse s of a is defined
 */
static int mult_inverse(int x)
{
  // no int because s and t can also end up with negative values
  // cast is done at end
  int r,old_r, s,old_s, t,old_t, quotient, tmp;

  r = SSS_P, old_r = x;
  s = 0, old_s = 1;
  t = 1, old_t = 0;

  while (r != 0) {
    quotient = old_r / r;

    tmp = r;
    r = old_r - quotient * r;
    old_r = tmp;

    tmp = s;
    s = old_s - quotient * s;
    old_s = tmp;

    tmp = t;
    t = old_t - quotient * t;
    old_t = tmp;
  }

  return (old_s >= 0) ? old_s % SSS_P : NEG_MOD(old_s);
}

/**
 * Create num_shares of secret and store them in shares[]
 * The polynomial will be of degree min_reconstruction_shares - 1 so that
 * the secret can only be reconstructed with min_reconstruction_shares or more
 * Returns -1 on error.
 *
 * Note that shares[] has to have at least size num_shares
 */
int get_shares(int secret, int min_reconstruction_shares,
               int num_shares, share_t shares[])
{
  int i, poly_coeffs[min_reconstruction_shares];
  struct timespec current_time;

  // num_shares has to be greater than min_reconstruction_shares
  if (num_shares < min_reconstruction_shares)
    return -1;

  secret %= SSS_P;
  if (secret < 0) // % operator calculates modulos of negative numbers wrong
    secret += SSS_P;

  // random number seed with current time in nanoseconds
  clock_gettime(CLOCK_REALTIME, &current_time);
  srandom(current_time.tv_nsec);

  // generate random polynomial of degree min_reconstruction_shares-1
  // this will just generate random coefficients starting from the one for x_0
  // and stores them in the array poly_coeffs[]
  poly_coeffs[0] = secret; // because f(0) should be the secret
  for (i = 1; i < min_reconstruction_shares; i++) {
    while ((poly_coeffs[i] = random() % SSS_P) == 0); // only non zero coefficients
  }

  // calculate points on polynomial as shares and
  // write them to shares[]
  // x values 1 to num_shares are used
  for (i = 0; i < num_shares; i++) {
    shares[i].x = i+1;
    shares[i].y = evaluate_poly(i+1, poly_coeffs, min_reconstruction_shares-1);
  }
}

/**
 * Combine the shares shares[] using Lagrange Interpolation
 * Returns the secret value
 *
 * It uses Lagrange's interpolation formula with x=0:
 * f(0) = \sum_{i=1}^{n} ( \prod_{j=1, j \ne i}^{n} \frac{-j}{i-j} ) \times f(i)
 */
int combine_shares(int num_shares, share_t shares[])
{
  int res, prod_res, i, j;

  res = 0;
  for (i = 0; i < num_shares; i++) {
    prod_res = 1;
    for (j = 0; j < num_shares; j++) {
      if (j == i)
        continue;
      prod_res *= ( NEG_MOD(-shares[j].x) * mult_inverse(shares[i].x % SSS_P + NEG_MOD(-shares[j].x)) ) % SSS_P;
      prod_res %= SSS_P;
    }
    res += (prod_res * shares[i].y) % SSS_P;
    res %= SSS_P;
  }

  return res;
}
