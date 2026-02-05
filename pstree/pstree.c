#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include "pstree.h"

int main(int argc, char *argv[]) {
    
    Options options = {0};
    parseOptions(&options, argc, argv);

    return 0;
}

void printVersionInfo() {
    printf("%s\n", VERSION_INFO);
}

void parseOptions(Options *options, const int argc, const char* const argv[]) {
    for(int i = 1; i < argc; ++i) {
        if(strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--show-pids") == 0) {
            options->show_pid = true;
        }
        else if(strcmp(argv[i], "-n") == 0 || strcmp(argv[i], "--numeric-sort") == 0) {
            options->sort_children_by_pid_asc = true;
        }
        else if(strcmp(argv[i], "-V") == 0 || strcmp(argv[i], "--version") == 0) {
            // printf("--version\n");
            if(argc != 2) {
                perror("Error: --V or --version cannot be combined with other options.\n");
                exit(1);
            }
            printVersionInfo();
            exit(0);
        }
    }
}