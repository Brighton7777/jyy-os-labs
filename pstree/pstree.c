#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
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

int getPidIdx(int pid, int *pids, int pids_cnt) {
    for(int i = 0; i <= pids_cnt; ++i) {
        if(pid == pids[i]) {
            return i;
        }
    }
    return -1; // pid not found
}
void getStatusPath(char* status_path, int pid) {
    int idx = 0;
    char proc[] = "/proc/";
    for(int i = 0; i <= 5; ++i) {
        status_path[idx++] = proc[i];
    }
    int tmp = pid, top = 0;
    char stack[MAX_PID_DIGITS];
    while(tmp != 0) {
        stack[top++] = (char)(tmp % 10 + '0');
        tmp /= 10;
    }
    while(top != 0) {
        status_path[idx++] = stack[--top];
    }
    // printf("%d\n", idx);
    status_path[idx] = '\0';
    // printf("%s\n", status_path);
    char sta[] = "/status";
    printf("%s\n", sta);
    for(int i = 0; i <= 6; ++i) {
        status_path[idx + i] = sta[i];
    }
    idx += 7;
    status_path[idx] = '\0';
}
UnitTest(testGetStatusPath) {
    char sp[MAX_PATH_LENGTH];
    getStatusPath(sp, 12345);
    printf("%s\n", sp);
    assert(strcmp(sp, "/proc/12345/status") == 0);
}

void praseProcStatus(const char* status, int sta_length, Proc* proc) {
    bool name_find = false, ppid_find = false;
    for(int i = 0; i < sta_length; ++i) {
        if(i + 4 >= sta_length)
            break;
        if(status[i] == 'N' && status[i + 1] == 'a' && status[i + 2] == 'm' && 
           status[i + 3] == 'e' && status[i + 4] == ':') {
            // printf("%d\n", i);
            getNextWord(status, i + 5, proc->name);
            name_find = true;
            printf("%s\n", proc->name);
        }
        if(status[i] == 'P' && status[i + 1] == 'P' && status[i + 2] == 'i' && 
           status[i + 3] == 'd' && status[i + 4] == ':') {
            char* ppid[MAX_PID_DIGITS];
            getNextWord(status, i + 5, ppid);
            proc->ppid = str2num(ppid);
            ppid_find = true;
            printf("%d\n", proc->ppid);
        }
        if(name_find == true && ppid_find == true)
            break;
    }
}
UnitTest(testPraseProcStatus) {
    char status[] = "Name: test\nPPid: 12345";
    int sta_length = strlen(status);
    printf("%d\n", sta_length);
    Proc proc = {0};
    praseProcStatus(status, sta_length, &proc);
    assert(strcmp(proc.name, "test") == 0);
    assert(proc.ppid == 12345);
}

void initProc(int pid, Proc* proc) {
    proc->pid = pid;
    char status_path[MAX_PATH_LENGTH];
    getStatusPath(status_path, pid);
    char status[MAX_STATUS_SIZE];
    int sta_length = readAll(status_path, status);
    praseProcStatus(status, sta_length, proc);
}
UnitTest(testInitProc) {
    Proc proc = {0};
    initProc(1, &proc);
    // printf("%s\n", proc.name);
    assert(strcmp(proc.name, "systemd") == 0);
    assert(proc.ppid == 0);
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

int readAll(const char* src, char* dest) {
    int fd = open(src, O_RDONLY);
    if(fd < 0) {
        fprintf(stderr, "Error: open %s failed.\n", src);
        exit(1);
    }
    ssize_t n;
    int length = 0;
    char buf[4096];
    int dest_idx = 0;
    while((n = read(fd, buf, 4096)) > 0) {
        for(int i = 0; i < n; ++i) {
            dest[dest_idx++] = buf[i];
        }
        length += n;
    }
    if(n < 0) {
        perror("Error: read failed.\n");
        exit(1);
    }
    printf("%d, %d\n", dest_idx, length);
    // assert(dest_idx == length);
    return length;
}
UnitTest(testReadAll) {
    char src[] = "/proc/1/status";
    char dest[MAX_STATUS_SIZE];
    int dest_length = readAll(src, dest);
    printf("%d\n", dest_length);
    int cnt = 0;
    char* dest_idx = dest;
    for(int i = 0; i < dest_length; ++i) {
        putchar(dest[i]);
        ++cnt;
    }
    printf("\n");
    // printf("%d\n", strlen(dest));
    assert(cnt == dest_length);
}

void getNextWord(const char* str, int idx, char* dest) {
    int dest_idx = 0;
    while(str[idx] != '\n' && str[idx]!='\0') {
        if((str[idx] >= 'a' && str[idx] <= 'z') || 
           (str[idx] >= 'A' && str[idx] <= 'Z') ||
           (str[idx] >= '0' && str[idx] <= '9'))
            dest[dest_idx++] = str[idx];
        ++idx;
    }
    dest[dest_idx] = '\0';
}
UnitTest(testGetNextWord) {
    char str[] = "PPid: 12345\nName: test\n";
    char dest[MAX_STATUS_SIZE];
    getNextWord(str, 5, dest);
    printf("%s\n", dest);
    assert(strcmp(dest, "12345") == 0);
    getNextWord(str, 17, dest);
    assert(strcmp(dest, "test") == 0);
}