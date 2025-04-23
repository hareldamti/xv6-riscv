#include "kernel/types.h"
#include "user/user.h"

int
main(void)
{
    int workers = 1 << 2, length = 1 << 16, chunk = length / workers;

    int role, i, sum = 0;
    int pids[workers];
    int statuses[64];

    int* bigarray = malloc(length * sizeof(int));

    for (i = 0; i < length; i++)
        bigarray[i] = i;

    if ((role = forkn(workers, pids)) < 0)
    {
        printf("failed fork");
        exit(1, "");
    }
    if (role == 0) {
        waitall(&workers, statuses);
        printf("parent exited from %d workers\n", workers);
        for (i = 0; i < workers; i++) sum += statuses[i];
        printf("Parent - calculated total sum: %d\n", sum);

    }
    else {
        for (i = chunk * (role - 1); i < chunk * (role); i++)
            sum += bigarray[i];
        sleep(role); // for order of printing
        printf("Worker %d - Calculated partial sum: %d\n", role, sum);
        exit(sum, 0);
    }
    exit(0, "");
}