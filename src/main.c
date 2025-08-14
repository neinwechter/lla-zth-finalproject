#include <stdio.h>
#include <getopt.h>
#include <stdbool.h>

//#include "common.h"
//#include "file.h"
//#include "parse.h"

int main(int argc, char *argv[]) {
    char *filepath = NULL;
    bool newfile =  false;
    int c;

    while ((c = getopt(argc, argv, "nf:")) != -1) {
        switch (c) {
            case 'n':
                newfile = true;
                break;
            case 'f':
                filepath = optarg;
                break;
            case '?':
                printf("Unknown option -%c\n", c);
                break;
            default:
                return -1; 
        }
    }

    printf("Newfile: %d\n", newfile);
    printf("File Path: %s\n", filepath);

    return 0;
}