#include <stdio.h>
#include <unistd.h> // Header for sleep()

int main() {
    while (1) {
        printf("Hello I am torjan horse!\n");
        fflush(stdout);
        sleep(10);
    }
    return 0;
}

