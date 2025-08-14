#include <stdio.h>
#include <getopt.h>
#include <stdbool.h>

//#include "common.h"
//#include "file.h"
//#include "parse.h"

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

    return 0;
}