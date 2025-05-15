#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define NLOCKS 16
int lock_ids[NLOCKS];
int tournament_id;
int tournament_level;

int lock_idx(int level) {
    return (1 << level) - 1 + (tournament_id >> (tournament_level - level));
}

int role(int level) {
    return tournament_id >> (tournament_level - level - 1) & 1;
}

int tournament_create(int processes)
{
    int i, j, pid;
    // Compute level
    if (processes == 0) return -1;
    tournament_level = 0;
    while ((processes & 1) == 0)
    {
        tournament_level += 1;
        processes = processes / 2;
    }
    if (processes != 1) return -1;
    processes <<= tournament_level;
    
    for (i = 0; i < processes; i++)
    {
        lock_ids[i] = peterson_create();
        if (lock_ids[i] == -1) {
            for (j = 0; j < i; j++)
            {
                peterson_destroy(lock_ids[j]);
            }
            return -1;
        }
    }
    
    tournament_id = 0;
    for (j = tournament_level; j > 0; j--) {
        if ((pid = fork()) == -1) return -1;
        tournament_id += (1 << (j - 1)) * ((pid == 0) ? 1 : 0);
    }

    return tournament_id;
}

int tournament_acquire(void)
{
    int level;
    for (level = tournament_level - 1; level >= 0; level--) {
        //printf("Level: %d/%d. Acquiring lock[%d]= %d, role: %d\n", level, tournament_level, lock_idx(level), lock_ids[lock_idx(level)], role(level));
        if (peterson_acquire(lock_ids[lock_idx(level)], role(level)) == -1) return -1;
    }
    return 0;
}

int tournament_release(void)
{
    int level;
    for (level = 0; level < tournament_level; level++) {
        //printf("Level: %d/%d. Releasing lock[%d]= %d, role: %d\n", level, tournament_level, lock_idx(level), lock_ids[lock_idx(level)], role(level));
        if (peterson_release(lock_ids[lock_idx(level)], role(level)) == -1) return -1;
    }
    return 0;
}