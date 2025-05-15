// Long-term locks for processes
enum lockstate {
  AVAILABLE,
  ACTIVE,
  DESTROYED
};

struct petersonlock {
  struct spinlock lk;
  enum lockstate state;
  int b[2];
  int turn;
};