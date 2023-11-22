#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <signal.h>

#define FIFO_FILE "server_fifo"

void handler(int signum) {
    // Handle signals if needed
}

int main() {
    umask(0077);
    mkfifo(FIFO_FILE, S_IFIFO | 0666);

    signal(SIGCHLD, handler);

    while (1) {
        int client_fd = open(FIFO_FILE, O_RDONLY);
        char client_fifo[256];
        read(client_fd, client_fifo, sizeof(client_fifo));
        close(client_fd);

        pid_t child_pid = fork();

        if (child_pid == 0) {
            // Child process
            close(0);
            int client_write_fd = open(client_fifo, O_WRONLY);

            struct timeval start, end;
            gettimeofday(&start, NULL);

            while (1) {
                gettimeofday(&end, NULL);
                if ((end.tv_sec - start.tv_sec) >= 30) {
                    break;
                }

                char random_char = 'A' + rand() % 26;
                write(client_write_fd, &random_char, sizeof(random_char));

                // Change priority randomly
                int which = PRIO_PROCESS;
                pid_t pid = getpid();
                if (rand() % 100 > 80) {
                    int new_priority = rand() % 11;
                    setpriority(which, pid, new_priority);
                }

                usleep(100000); // Sleep for 0.1 seconds
            }

            close(client_write_fd);
            // Delete the server FIFO file
            unlink(FIFO_FILE);

            exit(0);
        } else {
            // Parent process
            close(client_fd);
        }
    }
    return 0;
}
