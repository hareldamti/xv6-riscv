#include "kernel/types.h"
#include "user/user.h"

int
main(void)
{

    printf("Process size: %d\n", memsize());
    void* add = malloc(2 * 10e4);
    printf("Process size: %d\n", memsize());
    free(add);
    printf("Process size: %d\n", memsize());
    exit(0, "");
}