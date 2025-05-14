// Sleeping locks

#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "petersonlock.h"

void
initpetersonlock(struct petersonlock *lk)
{
  initlock(&lk->lk, "peterson lock");
  lk->used = 0;
}

int
trycreatepetersonlock(struct petersonlock *lk)
{
  acquire(&lk->lk);
  if (lk->used) {
    release(&lk->lk);
    return 0;
  }
  lk->used = 1;
  lk->b[0] = 0;
  lk->b[1] = 0;
  lk->turn = -1;
  release(&lk->lk);
  return 1;
}

void
acquirepeterson(struct petersonlock *lk, int role)
{
  lk->b[role] = 1;
  lk-> turn = role;
  __sync_synchronize();
  while (lk->b[1 - role] && lk->turn == role) {
    yield();
    __sync_synchronize();
  }

}

void
releasepeterson(struct petersonlock *lk, int role)
{
  lk->b[role] = 0;
  __sync_synchronize();
}

int
holdingpeterson(struct petersonlock *lk, int role)
{
  return lk->b[role] == 1 && lk->b[1 - role] == 0;
}

void
destroypeterson(struct petersonlock *lk)
{
  acquire(&lk->lk);
  lk->used = 0;
  release(&lk->lk);
}