#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(void)
{
    //int base_pid = getpid();
    int pid = fork();
    void* dst_addr;
    void* shared_va;
    if (pid != 0) {
        if ((dst_addr = map_shared_pages(pid, &shared_va, 4)) == 0) {
            printf("Error in map_shared_pages\n");
            exit(1);
        }
        *(int*)shared_va = 16;
        wait(0);
        printf("dst value: %d\n", *(int*)shared_va);
    }
    else {
        sleep(1);
        *(int*)shared_va = 17;
    }
    exit(0);
}