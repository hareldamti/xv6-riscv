#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(void)
{
    int pid = fork();
    int shared_size = 18;
    void* dst_va;
    char* msg;
    if (pid != 0) {
        msg = malloc(shared_size);
        // Turn 1
        if (map_shared_pages(pid, msg, &dst_va, shared_size) == 0) {
            printf("Parent/ Got an error in map_shared_pages\n");
            exit(1);
        }
        printf("Parent/ Mapped shared pages\n");
        sleep(2);

        // Turn 3
        printf("Parnet/ Buffer has: %s\n", msg);
        strcpy(msg, "Hello offspring");
        sleep(2);

        // Turn 5
        if (unmap_shared_pages(pid, dst_va, shared_size) == -1) {
            printf("Parent/ Got an error in unmap_shared_pages\n");
            exit(1);
        }
        printf("Parent/ Unmapped shared pages\n");
        msg = malloc(shared_size);
        strcpy(msg, "Malloc after unmap success");
        printf("Parent/ %s\n", msg);
        wait(0);
    }
    else {
        sleep(1);

        // Turn 2
        if (dst_va == 0) exit(1);
        strcpy(dst_va, "Hello caregiver");
        sleep(2);

        // Turn 4
        printf("Child/ Buffer has: %s\n", dst_va);
        sleep(2);

        // Turn 5
        dst_va = malloc(shared_size);
        strcpy(dst_va, "Malloc after unmap success");
        printf("Child/ %s\n", dst_va);
    }
    exit(0);
}