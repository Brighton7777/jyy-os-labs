#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <testkit.h>
#include "pstree.h"

int main(int argc, char *argv[]) {
    
    Options options = {0};
    parseOptions(&options, argc, argv);

    int pids[MAX_PROC_NUMBER];
    int pids_cnt = 0;
    pids_cnt = getAllPids(pids);
    
    
    return 0;
}

void printVersionInfo() {
    printf("%s\n", VERSION_INFO);
}

void parseOptions(Options *options, const int argc, char** argv) {
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

int getAllPids(int *pids) {
    DIR *dir = opendir("/proc");
    if(dir == 0) {
        perror("Error: open /proc failed.");
        exit(1);
    }
    Dirent *entry;
    int pids_cnt = -1;
    while((entry = readdir(dir)) != NULL) {
        if(isNumber(entry->d_name)) {
            pids[++pids_cnt] = str2num(entry->d_name);
        }
    }
    return pids_cnt;
}
UnitTest(testGetAllPids) {
    int pids[MAX_PROC_NUMBER];
    int pids_cnt = getAllPids(pids);
    printf("%d pids in total.\n", pids_cnt);
    for(int i = 0; i <= pids_cnt; ++i) {
        printf("%d\n", pids[i]);
    }
    assert(true);
}

// Helper functions
bool isNumber(const char* dir_name) {
    for(; *dir_name != '\0'; ++dir_name) {
        if(isdigit(*dir_name) == false)
            return false;
    }
    return true;
}
UnitTest(testIsNumber) {
    assert(isNumber("1234") == true);
    assert(isNumber("abc") == false);
    assert(isNumber("12a") == false);
}

int str2num(const char* dir_name) {
    int pid = 0;
    for(; *dir_name != '\0'; ++dir_name) {
        pid = pid * 10 + (*dir_name - '0');
    }
    return pid;
}
UnitTest(testStr2Num) {
    assert(str2num("1234") == 1234);
    assert(str2num("1000") == 1000);
}