// generate matrix, row-major
uint32_t* rand_gen(uint32_t seed, int R, int C) {
  uint32_t *m = (uint32_t *) malloc(sizeof(uint32_t) * R*C);
  uint32_t x = 2, n = R*C;
  for (int i = 0; i < R; i++) {
    for (int j = 0; j < C; j++) {
      x = (x * x + seed + i + j)%n;
      m[i*C + j] = x;
    }
  }
  return m;
}
uint32_t hash(uint32_t x) {
  return (x * 2654435761LU);
}
// output
uint32_t signature(uint32_t *A, int r, int c) {
  uint32_t h = 0;
  for (int i = 0; i < r; i++) {
    for (int j = 0; j < c; j++)
      h = hash(h + A[i*c + j]);
  }
  return h;
}
