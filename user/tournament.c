#include "kernel/types.h"
#include "user/user.h"

int main(void)
{
    printf("Starting tournament\n");

    int tournament_id = tournament_create(16);
    if (tournament_id == -1) exit(-1);
    tournament_acquire();
    printf("Critical section for ID: %d\n", tournament_id);
    tournament_release();
    
    // To sync final message
    while (wait(0) != -1);
    if (tournament_id == 0) { printf("Finished tournament\n"); }
    exit(0);

}