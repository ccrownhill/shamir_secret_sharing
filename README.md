# Basic Shamir Secret Sharing library

This Shamir secret sharing library provides the functions `get_shares` and `combine_shares` to share a secret and recombine it.

**Disclaimer**: It is by no means really secure and was created for learning purposes.

## How it works

Shamir secret sharing is a *(t, n)-threshold-scheme* which means that you create `n` secret shares and any `t` share holders have enough information by combining their shares to reconstruct the secret (note that we need `t <= n`).

## Test it

To run the test compile it with

```
gcc sss.c test.c -o test
```

and run it with

```
./test <secret_val> <min_shares> <num_shares>
```
