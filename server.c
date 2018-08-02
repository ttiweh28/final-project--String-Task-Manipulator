#include <stdio.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "utility.h"

#define PORT 8280
#define MAXMSG 512

char reply[MAXMSG + 1];
char buffer[MAXMSG + 1];
char message[MAXMSG + 1];

int main(void) {
    int make_socket(uint16_t port);
    void handle_command(char *command);
    int read_from_client(int filedes);
    void write_to_client(int fd);
    void store_command();

    int sock;
    fd_set active_fd_set, read_fd_set;
    int i;
    struct sockaddr_in clientname;
    socklen_t size;

    /* Create the socket and set it up to accept connections. */
    sock = make_socket(PORT);
    if (listen(sock, 5) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    /* Initialize the set of active sockets. */
    FD_ZERO(&active_fd_set);
    FD_SET(sock, &active_fd_set);
    while (1) {
        /* Block until input arrives on one or more active sockets. */
        read_fd_set = active_fd_set;
        if (select(FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0) {
            perror("select");
            exit(EXIT_FAILURE);
        }

        /* Service all the sockets with input pending. */
        for (i = 0; i < FD_SETSIZE; ++i) {
            if (FD_ISSET(i, &read_fd_set)) {
                if (i == sock) {
                    /* Connection request on original socket. */
                    int new;
                    size = sizeof(clientname);
                    new = accept(sock, (struct sockaddr *) &clientname, &size);

                    if (new < 0) {
                        perror("accept");
                        exit(EXIT_FAILURE);
                    }

                    fprintf(stderr,
                            "Server: connection from host %s, on port %hd.\n",
                            inet_ntoa(clientname.sin_addr),
                            ntohs(clientname.sin_port));
                    FD_SET(new, &active_fd_set);
                }
                else {
                    /* Data arriving on an already-connected socket. */
                    if (read_from_client(i) < 0) {
                        close(i);
                        FD_CLR(i, &active_fd_set);
                    }

                    // Clear the reply array.
                    sprintf(reply, "%s", "");

                    // Handle multiple commands.
                    int count = 0;
                    char* commands[10];
                    char* command = strtok(message, ";");
                    while (1) {
                        if (command == NULL) {
                            break;
                        }
                        commands[count] = command;
                        ++count;

                        // Get next command
                        command = strtok(NULL, ";");
                    }

                    for (int j = 0; j < count; ++j) {
                        // Create a copy to be used by handle command.
                        char *cmd = (char *) malloc(strlen(commands[j]) * sizeof(char));
                        strcpy(cmd, commands[j]);

                        handle_command(cmd);
                        store_command(commands[j]);

                        // Free allocated memory.
                        free(cmd);
                    }

                    // Send reply back to the client.
                    write_to_client(i);
                }
            }
        }
    }
}

int make_socket(uint16_t port) {
    int sock;
    struct sockaddr_in name;

    /* Create the socket. */
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    /* Give the socket a name. */
    name.sin_family = AF_INET;
    name.sin_port = htons(port);
    name.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(sock, (struct sockaddr *) &name, sizeof(name)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    return sock;
}

int read_from_client(int filedes) {
    int nbytes;
    nbytes = read(filedes, message, MAXMSG);
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

void write_to_client(int fd) {
    int nbytes;
    nbytes = write(fd, reply, strlen(reply) + 1);
    if (nbytes < 0) {
        perror("write");
        printf("%s\n", "Failed to write to client");
        exit(EXIT_FAILURE);
    }
}

void handle_command(char *command) {
    if (starts_with(command, "double")) {
        // Remove the word double
        strtok(command, " ");

        // Get the argument
        char *arg = strtok(NULL, " ");

        // Set message to be sent back to client
        sprintf(buffer, "\nThe doubled string is: %s", double_string(arg));
        strcat(reply, buffer);
    }
    else if (starts_with(command, "reverse")) {
        // Remove the word reverse
        strtok(command, " ");

        // Get the argument
        char *arg = strtok(NULL, " ");

        // Set message to be sent back to client
        sprintf(buffer, "\nThe reversed string is: %s", reverse_string(arg));
        strcat(reply, buffer);
    }
    else if (starts_with(command, "delete")) {
        // Remove the word delete
        strtok(command, " ");

        // Get the word to delete from
        char *word = strtok(NULL, " ");

        // Get the positions to delete from
        char *pos_str = strtok(NULL, " ");

        // Convert it into an array.
        int positions[10] = {0};
        int count = 0;
        char *pos = strtok(pos_str, ",");
        while (1) {
            if (pos == NULL) {
                break;
            }

            positions[count] = atoi(pos);
            ++count;

            pos = strtok(NULL, ",");
        };

        if (count == 0) {
            sprintf(buffer, "\nDelete Error: %s", "Please supply the positions to delete");
            strcat(reply, buffer);
        }
        else {
            sprintf(buffer, "\nAfter deleting characters: %s", delete_characters(word, positions, count));
            strcat(reply, buffer);
        }
    }
    else if (starts_with(command, "replace")) {
        // Remove the word replace
        strtok(command, " ");

        // Get the word to replace from
        char *word = strtok(NULL, " ");

        // Get the replacements
        char *args_str = strtok(NULL, " ");

        // Convert it into an array.
        int count = 0;
        char *args[10];
        char *arg = strtok(args_str, ",");
        while (1) {
            if (arg == NULL) {
                break;
            }

            args[count] = arg;
            ++count;

            arg = strtok(NULL, ",");
        }

        char characters[10];
        int positions[10] = {0};
        for (int j = 0; j < count; ++j) {
            positions[j] = atoi(strtok(args[j], "-"));
            characters[j] = strtok(NULL, "-")[0];
        }

        if (count == 0) {
            sprintf(buffer, "\nReplace Error: %s", "Please supply the positions to replace");
            strcat(reply, buffer);
        }
        else {
            sprintf(buffer, "\nThe replaced string is: %s", replace_characters(word, characters, positions, count));
            strcat(reply, buffer);
        }
    }
    else if (starts_with(command, "encrypt")) {
        // Remove the word encrypt
        strtok(command, " ");

        // Get the word to be encrypted
        char *word = strtok(NULL, " ");

        //encrypt each character at a time
        char encrypted[1000] = "";
        for (int j = 0; j < strlen(word); ++j){
            char result[100] = "";
            encrypt_char(result, word[j]);
            strcat(encrypted, result);
        }

        // Set message to be sent back to client
        sprintf(buffer, "\nThe encrypted string is: %s", encrypted);
        strcat(reply, buffer);
    }
    else {
        sprintf(buffer, "\nUnknown command: %s", command);
        strcat(reply, buffer);
    }
}

void store_command(char *command){
    time_t curtime;
    struct tm *loctime;
    curtime = time(NULL);
    loctime = localtime(&curtime);
    FILE* fd;
    fd = fopen("ready_jobs.txt", "a+");
    fprintf(fd, "%s %s %d %s\n", getenv("USERNAME"), command, getpid(), asctime(loctime));
    fclose(fd);
}
