#include <stdio.h>
#include <stdlib.h>
#include "sss.h"

int main(int argc, char *argv[])
{
  int i, secret, min_shares, num_shares, reconstructed_secret;

  if (argc != 4) {
    fprintf(stderr, "Usage: test <secret_val> <min_shares> <num_shares>\n");
    exit(1);
  }

  secret = atoi(argv[1]);
  min_shares = atoi(argv[2]);
  num_shares = atoi(argv[3]);
  share_t shares[num_shares];

  if (get_shares(secret, min_shares, num_shares, shares) == -1) {
    fprintf(stderr, "Fatal: Number of shares is smaller "
        "than minimum to reconstruct the secret\n");
    exit(1);
  }

  for (i = 0; i < num_shares; i++)
    printf("Share %i: (%i|%i)\n", i+1, shares[i].x, shares[i].y);

  // reconstruction with all shares
  // (it also works with any subset >= min_shares)
  reconstructed_secret = combine_shares(num_shares, shares);
  printf("Secret reconstructed by all players: %i\n", reconstructed_secret);
}
