#ifndef SSS_H
#define SSS_H

#define SSS_P 32749 // 2^15 - 19

typedef struct {
  int x;
  int y;
} share_t;

int get_shares(int, int, int, share_t[]);
int combine_shares(int, share_t[]);

#endif
