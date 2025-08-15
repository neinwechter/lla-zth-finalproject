#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <file.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>

#include "../include/parse.h"
#include "../include/common.h"

int add_employee(struct dbheader_t *header, struct employee_t *employees, char *addstring) {
    if (header == NULL || employees == NULL || addstring == NULL) {
        printf("Bad header, employees, or addstring in add_employee\n");
        return STATUS_ERROR;
    }

    char *name = strtok(addstring, ",");
    char *addr = strtok(NULL, ",");
    char *hours = strtok(NULL, ",");

    if (!name || !addr || !hours) {
        printf("Invalid addstring format\n");
        return STATUS_ERROR;
    }

    int idx = header->count;

    strncpy(employees[idx].name, name, sizeof(employees[idx].name) - 1);
    strncpy(employees[idx].address, addr, sizeof(employees[idx].address) - 1);
    employees[idx].hours = atoi(hours);

    header->count++;

    return STATUS_SUCCESS;
}

int read_employees(int fd, struct  dbheader_t *dbhdr, struct employee_t **employeesOut) {
    if (fd < 0 || employeesOut == NULL) {
        printf("Bad FD or employeesOut in read_employees\n");
        return STATUS_ERROR;
    }

    int count = dbhdr->count;
    
    struct employee_t *employees = calloc(count, sizeof(struct employee_t));
    if (!employees) {
        perror("calloc");
        return STATUS_ERROR;
    }
    read(fd, employees, count * sizeof(struct employee_t));
    for (int i = 0; i < count; i++) {
        employees[i].hours = ntohl(employees[i].hours);
    }
    *employeesOut = employees;
    return STATUS_SUCCESS;
}

void output_file(int fd, struct dbheader_t *header, struct employee_t *employees) {
    if (fd < 0 || header == NULL || employees == NULL) {
        printf("Bad FD or header in output_file\n");
        return; // STATUS_ERROR;
    }

    int realcount = header->count;

    header->magic = htonl(header->magic);
    header->version = htons(header->version);
    header->count = htons(header->count);
    header->filesize = htonl(sizeof(struct dbheader_t) + realcount * sizeof(struct employee_t));

    lseek(fd, 0, SEEK_SET);
    ssize_t bytesWritten = write(fd, header, sizeof(struct dbheader_t));
    if (bytesWritten != sizeof(struct dbheader_t)) {
        perror("write");
        return; //STATUS_ERROR;   
    }

    for (int i = 0; i < realcount; i++) {
        employees[i].hours = htonl(employees[i].hours);
        write(fd, &employees[i], sizeof(struct employee_t));
    }

    return; //STATUS_SUCCESS;

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
