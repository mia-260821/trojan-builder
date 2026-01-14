#define _GNU_SOURCE
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/file.h>
#include "stub.h"

// Helper to execute binary data from RAM
void run_mem(const char *name, char *code, size_t size) {
    int fd = memfd_create(name, MFD_CLOEXEC);
    if (fd == -1) { perror("memfd_create"); exit(1); }
    if (write(fd, code, size) != size) { perror("write"); exit(1); }

    char *const argv[] = { (char*)name, NULL };
    char *const envp[] = { NULL };
    fexecve(fd, argv, envp);
    
    perror("fexecve failed");
    exit(1);
}

int require_lock(char* lock_path) {
    int fd = open(lock_path, O_RDWR | O_CREAT, 0666);
    if (fd == -1) {
        perror("Failed to open lock file");
        return 0;
    }

    // Attempt to acquire the exclusive lock
    if (flock(fd, LOCK_EX | LOCK_NB) == -1) {
        if (errno == EWOULDBLOCK) {
            fprintf(stderr, "Daemon already running (Locked by %s).\n", lock_path);
        }
        close(fd);
        return 0;
    }
    return fd;
}

void release_lock(int fd) {
    flock(fd, LOCK_UN);
}


void wrap(unsigned char* code1, size_t size1, unsigned char* code2, size_t size2) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        exit(1);
    }

    if (pid == 0) {
        // --- CHILD PROCESS: Become a Daemon ---
        setsid();           // Create a new session
        umask(0);           // Reset file permissions
        chdir("/");         // Move to root to avoid blocking mount points

        // Redirect standard I/O to /dev/null
        int devnull = open("/dev/null", O_RDWR);
        dup2(devnull, STDIN_FILENO);
        dup2(devnull, STDOUT_FILENO);
        dup2(devnull, STDERR_FILENO);
        close(devnull);
        // Run the daemon binary from memory
        int lockfile_fd = require_lock(LOCK_PATH);
        if(lockfile_fd > 0) {
            run_mem("daemon_prog", code2, size2);
            release_lock(lockfile_fd);
        } else {
            fprintf(stderr, "failed to acquire a lock");
        }
    } else {
        // --- PARENT PROCESS: Run Foreground ---
        printf("Daemon started with PID %d. Launching foreground...\n", pid);
        
        // This will replace the current builder process with the foreground app
        run_mem("foreground_prog", code1, size1);
    }
}

unsigned char* read_binary_file(const char* filename, size_t* out_size) {
    FILE* fp = fopen(filename, "rb"); // "rb" for binary read
    if (!fp) return NULL;

    // Find file size
    fseek(fp, 0L, SEEK_END);
    size_t size = ftell(fp);
    rewind(fp);

    unsigned char* buffer = malloc(size);
    if (buffer) {
        fread(buffer, 1, size, fp);
    }

    fclose(fp);
    *out_size = size;
    return buffer;
}

