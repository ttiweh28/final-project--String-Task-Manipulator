#include <stdio.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8280
#define MAXMSG 512
#define SERVERHOST "localhost"

const char * const command_prompt = "\n   > ";
char message[MAXMSG + 1];
char reply[MAXMSG + 1];

int main(void) {
    void init_sockaddr(struct sockaddr_in *name, const char *hostname, uint16_t port);
    void write_to_server(int filedes);
    int read_from_server(int fd);
    void show_message(char *msg);

    int sock;
    struct sockaddr_in servername;

    /* Create the socket. */
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket (client)");
        exit(EXIT_FAILURE);
    }

    /* Connect to the server. */
    init_sockaddr(&servername, SERVERHOST, PORT);
    if (0 > connect(sock, (struct sockaddr *) &servername, sizeof(servername))) {
        perror("connect (client)");
        exit(EXIT_FAILURE);
    }

    /* Send data to the server. */
    while (1) {
        printf("%s", command_prompt);
        fgets(message, MAXMSG, stdin);

        // Remove the newline character from the end of the string.
        message[strlen(message) - 1] = '\0';

        if ((strlen(message) - 1) == 0) {
            continue;
        }

        if (strncmp(message, "exit", 4) == 0) {  // Someone wants to escape :)
            show_message("Please be back soon...");
            break;
        }

        write_to_server(sock);
        read_from_server(sock);

        // Show reply to the user.
        char *message = strtok(reply, "\n");
        while (1) {
            if (message == NULL) {
                break;
            }
            show_message(message);

            message = strtok(NULL, "\n");
        }
    }

    close(sock);
    exit(EXIT_SUCCESS);
}

void init_sockaddr(struct sockaddr_in *name, const char *hostname, uint16_t port) {
    struct hostent *hostinfo;
    name->sin_family = AF_INET;
    name->sin_port = htons (port);

    hostinfo = gethostbyname(hostname);
    if (hostinfo == NULL) {
        fprintf(stderr, "Unknown host %s.\n", hostname);
        exit(EXIT_FAILURE);
    }

    name->sin_addr = *(struct in_addr *) hostinfo->h_addr;
}

void write_to_server(int filedes) {
    int nbytes;
    nbytes = write(filedes, message, strlen(message) + 1);
    if (nbytes < 0) {
        perror("write");
        exit(EXIT_FAILURE);
    }
}


int read_from_server(int filedes) {
    int nbytes;
    nbytes = read(filedes, reply, MAXMSG);
    if (nbytes < 0) {  /* Read error. */
        perror("read");
        exit(EXIT_FAILURE);
    }
    else if (nbytes == 0) {  /* End-of-file. */
        return -1;
    }
    else {  /* Data read. */
        return 0;
    }
}

void show_message(char *msg) {
    printf("     - %s\n", msg);
}
