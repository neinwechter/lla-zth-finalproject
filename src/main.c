#include <stdio.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <file.h>

#include "../include/file.h"
#include "../include/common.h"
#include "../include/parse.h"

void print_usage(char *argv[]) {
    printf("Usage: %s -n -f <database file>\n", argv[0]);
    printf("\t-f <database file>\tThe employee database file (REQUIRED)\n");
    printf("\t-n\t\t\tCreate new database file (defualt: NO)\n");
    printf("\t-h\t\t\tDisplay usage information\n");
    printf("\n");
    return;
}

int main(int argc, char *argv[]) {
    char *filepath = NULL;
    bool newfile =  false;
    int c;
    int dbfd = -1;
    struct db_header_t *header = NULL;

    while ((c = getopt(argc, argv, "nhf:")) != -1) {
        switch (c) {
            case 'n':
                newfile = true;
                break;
            case 'f':
                filepath = optarg;
                break;
            case 'h':
                print_usage(argv);
                return 0;
            case '?':
                printf("Unknown option -%c\n", c);
                break;
            default:
                return -1; 
        }
    }

    if (filepath == NULL) {
        printf("ERROR: Filepath (-f <filename>) is a required argument\n\n");
        print_usage(argv);
        return 0;
    }

    if (newfile) {
        dbfd = create_db_file(filepath);
        if (dbfd == STATUS_ERROR) {
            printf("Unable to create database file %s\n", filepath);
            return -1;
        }
        if (create_db_header(dbfd, &header) == STATUS_ERROR) {
            printf("Unable to create database header\n");
            return -1;
        }

        output_file(dbfd, header);

    }
    else {
        dbfd = open_db_file(filepath);
        if (dbfd == STATUS_ERROR) {
            printf("Unable to open database file %s\n", filepath);
            return -1;
        }
      
        if (validate_db_header(dbfd, &header) == STATUS_ERROR) {
            printf("Invalid database header in file %s\n", filepath);
            free(header);
            close(dbfd);
            return -1;
        }
    }

    return 0;
}