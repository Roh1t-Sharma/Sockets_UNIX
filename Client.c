#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>

#define CLIENT_FIFO_TEMPLATE "client_fifo_%d"

int main() {
    char server_fifo[] = "server_fifo";
    char client_fifo[256];

    // Create a unique FIFO for this client
    pid_t pid = getpid();
    sprintf(client_fifo, CLIENT_FIFO_TEMPLATE, pid);
    mkfifo(client_fifo, S_IFIFO | 0666);

    // Connect to the server
    int server_fd = open(server_fifo, O_WRONLY);
    write(server_fd, client_fifo, sizeof(client_fifo));
    close(server_fd);

    // Receive characters from the server
    int client_fd = open(client_fifo, O_RDONLY);
    char buffer;
    int char_count = 0;

    struct timeval start, end;
    gettimeofday(&start, NULL);

    while (1) {
        gettimeofday(&end, NULL);
        if ((end.tv_sec - start.tv_sec) >= 30) {
            break;
        }

        if (read(client_fd, &buffer, sizeof(buffer)) > 0) {
            char_count++;
            printf("Received character: %c\n", buffer);
        }

        usleep(100000);
    }

    printf("Total characters received: %d\n", char_count);

    close(client_fd);
    unlink(client_fifo);
    return 0;
}
