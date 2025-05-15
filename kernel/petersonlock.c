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
  lk->state = AVAILABLE;

}

int
trycreatepetersonlock(struct petersonlock *lk)
{
  acquire(&lk->lk);
  __sync_synchronize();
  if (!lk->state == AVAILABLE) {
    release(&lk->lk);
    return 0;
  }
  lk->state = ACTIVE;
  lk->b[0] = 0;
  lk->b[1] = 0;
  lk->turn = -1;
  __sync_synchronize();
  release(&lk->lk);
  return 1;
}

int
acquirepeterson(struct petersonlock *lk, int role)
{
  // Don't acquire if AVAILABLE (synchronize for creation acquizition concurrency)
  // Don't acquire if DESTROYED (waiting for last holder to release)
  acquire(&lk->lk); 
  if (lk->state != ACTIVE) {
    release(&lk->lk);
    return -1;
  }
  release(&lk->lk);

  lk->b[role] = 1;
  lk-> turn = role;
  __sync_synchronize();
  while (lk->b[1 - role] && lk->turn == role) {
    yield();
    __sync_synchronize();
  }
  return 0;
}

int
releasepeterson(struct petersonlock *lk, int role)
{
  __sync_synchronize();
  if (lk->state == AVAILABLE) return -1;

  lk->b[role] = 0;
  acquire(&lk->lk);
  if (lk->state == DESTROYED) {
    lk->state = AVAILABLE;
    release(&lk->lk);
  }
  release(&lk->lk);
  __sync_synchronize();
  return 0;
}

int
destroypeterson(struct petersonlock *lk)
{
  acquire(&lk->lk);
  if (lk->state != ACTIVE) {
    release(&lk->lk);
    return -1;
  }
  if (lk->b[0] || lk->b[1]) {
    lk->state = DESTROYED;
  } else lk->state = AVAILABLE;
  __sync_synchronize();
  release(&lk->lk);
  return 0;
}