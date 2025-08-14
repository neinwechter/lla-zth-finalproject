#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include "../include/file.h"
#include "../include/common.h"
#include "../include/parse.h"

int create_db_file(char *filename) {
    int fd = open(filename, O_RDONLY);
    if (fd != -1) {
        close(fd);
        printf("File already exists");
        return STATUS_ERROR;
    }

    fd = open(filename, O_RDWR | O_CREAT, 0644);
    if (fd == -1) {
        perror("open");
        return STATUS_ERROR;
    }

    return fd;
}

int open_db_file(char *filename) {
    int fd = open(filename, O_RDWR);
    if (fd == -1) {
        perror("open");
        return STATUS_ERROR;
    }

    return fd;
}

