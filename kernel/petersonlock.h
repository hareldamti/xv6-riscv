// Long-term locks for processes
struct petersonlock {
  struct spinlock lk;
  int used;
  int b[2];
  int turn;
};