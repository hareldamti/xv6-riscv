#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define LOG_BUFFER_SIZE 1 << 12

const char* base_msg = "I'm process ";
const char* rand_length = ", misalignment: @#!*$#[$+^#;$#@$^&)";
const char* newline = "\n";

struct log_buffer {
    int index;
    char buffer[LOG_BUFFER_SIZE];
};

void
log_info(struct log_buffer* log_buffer, int id) {
    while (__sync_val_compare_and_swap((int*)(log_buffer->buffer + log_buffer->index), 0, id) != 0);

    int current_index = log_buffer->index + 4,
        rand = 16 + (id + current_index + 41) * 37 % 15,
        msg_length = (12 + 1 + rand + 2 + 3) & -4;
    char c = 'A' + id;

    *(short*)(log_buffer->buffer + log_buffer->index + 1) = msg_length;
    log_buffer->index += 4 + msg_length;

    
    strcpy(log_buffer->buffer + current_index, base_msg);
    memcpy(log_buffer->buffer + current_index + 12, &c, 1);
    memcpy(log_buffer->buffer + current_index + 12 + 1, rand_length, rand);
    strcpy(log_buffer->buffer + current_index + 12 + 1 + rand, newline);
}

void
print_logs(struct log_buffer* log_buffer)
{
    int index = 0;
    while (index < log_buffer->index) {
        printf(
            "%c: %s",
            'A' + *(short*)(log_buffer->buffer + index),
            log_buffer->buffer + index + 4);
        index += 4 + *(short*)(log_buffer->buffer + index + 1);

    }
}

int
main(void)
{
    // int x = (13 << 16) + 7;
    // printf("%d, %d", *(short*)&x, *((short*)&x + 1)); // 7, 13

    int num_writers = 20, id, i;
    struct log_buffer *log_buffer = 0, *writer_log_buffer = 0;
    int pids[num_writers];

    for (id = 0; id < num_writers; id++)
    {
        if ((pids[id] = fork()) != 0) {
            continue;
        }

        while (!writer_log_buffer) sleep(1);

        for (i = 0; i < 2; i++) {
            log_info(writer_log_buffer, id);
        }

        exit(0);
    }
    
    printf("Forked writers\n");
    log_buffer = malloc(sizeof(struct log_buffer));
    printf("Malloced buffer\n");
    for (id = 0; id < num_writers; id++)
        if (map_shared_pages(pids[id], log_buffer, (void**)&writer_log_buffer, sizeof(struct log_buffer)) == 0)
            printf("Failed mapping buffer to writer %c\n", 'A' + id);
    printf("Mapped pages\n");

    while(wait(0) == -1); // Wait for kids to finish writing
    printf("Log buffer length: %d\n", log_buffer->index);
    print_logs(log_buffer);

    exit(0);
}