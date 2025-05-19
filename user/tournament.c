#include "kernel/types.h"
#include "user/user.h"

void exit_error(char* msg, int id) {
    printf(msg, id);
    exit(1);
}
int main(void)
{
    int tournament_id;
    printf("Creating tournament\n");
    if ((tournament_id = tournament_create(16)) == -1) exit_error("Tournament failed to create\n", 0);
    if (tournament_acquire() == -1) exit_error("ID %d failed to acquire\n", tournament_id);
    printf("Critical section for ID: %d\n", tournament_id);
    if (tournament_release() == -1) exit_error("ID %d failed to release\n", tournament_id);
    
    while (wait(0) != -1); // Wait for all children to finish to sync final message
    if (tournament_id == 0) { printf("Finished tournament\n"); }
    exit(0);
}