#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <file.h>
#include <stdlib.h>
#include <arpa/inet.h>

#include "../include/parse.h"
#include "../include/common.h"

int output_file(int fd, struct dbheader_t *header, void *future) {
    if (fd < 0 || header == NULL) {
        printf("Bad FD or header in output_file\n");
        return STATUS_ERROR;
    }

    header->magic = htonl(header->magic);
    header->version = htons(header->version);
    header->count = htons(header->count);
    header->filesize = htonl(header->filesize);

    lseek(fd, 0, SEEK_SET);
    ssize_t bytesWritten = write(fd, header, sizeof(struct dbheader_t));
    if (bytesWritten != sizeof(struct dbheader_t)) {
        perror("write");
        return STATUS_ERROR;   
    }

    return 0;

}

int validate_db_header(int fd, struct dbheader_t **headerOut) {
    if (fd < 0 || headerOut == NULL) {
        printf("Bad FD from user in validate_db_header\n");
        return STATUS_ERROR;
    }

    struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t));
    if (!header) {
        perror("calloc");
        return STATUS_ERROR;
    }

    ssize_t bytesRead = read(fd, header, sizeof(struct dbheader_t));
    if (bytesRead != sizeof(struct dbheader_t)) {
        perror("read");
        free(header);
        return STATUS_ERROR;
    }

    header->version = ntohs(header->version);
    header->count = ntohs(header->count);
    header->filesize = ntohl(header->filesize);
    header->magic = ntohl(header->magic);

    if (header->magic != HEADER_MAGIC) {
        printf("Invalid magic number: 0x%X\n", header->magic);
        free(header);
        return STATUS_ERROR;
    }

    if (header->version != 1) {
        printf("Unsupported version: %d\n", header->version);
        free(header);
        return STATUS_ERROR;
    }

    struct stat dbstat = {0};
    fstat(fd, &dbstat);
    if (header->filesize != dbstat.st_size) {
        printf("File size mismatch: expected %u, got %ld\n", header->filesize, dbstat.st_size);
        free(header);
        return STATUS_ERROR;
    }   

    *headerOut = header;
    return STATUS_SUCCESS;
}

int create_db_header(struct dbheader_t **headerOut) {
    if (headerOut == NULL) {
        printf("Bad headerOut pointer in create_db_header\n");
        return STATUS_ERROR;
    }
    struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t));
    if (!header) {
        perror("calloc");
        return STATUS_ERROR;
    }

    header->magic = HEADER_MAGIC;
    header->version = 1;
    header->count = 0;
    header->filesize = sizeof(struct dbheader_t);

    *headerOut = header;
    return STATUS_SUCCESS;
}