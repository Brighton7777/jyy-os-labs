#include <stdbool.h>
#include <dirent.h>

#define MAX_CHILD_NUMBER 50
#define MAX_PROC_NAME 50
#define MAX_PROC_NUMBER 100
#define MAX_PATH_LENGTH 50
#define MAX_PID_DIGITS 10
#define MAX_STATUS_SIZE 4096
#define VERSION_INFO "pstree by zrq"

typedef struct Proc {
    char name[MAX_PROC_NAME];
    int pid;
    int ppid;
    // int pgid;
    // int uid;
    // int ns;
    // char **argv;
    int children[MAX_CHILD_NUMBER];
    int child_cnt;
    int parent_idx;
} Proc;

typedef struct Options {
    bool show_pid;
    // bool show_ppid;
    // bool show_argv;
    // bool show_path;
    bool sort_children_by_pid_asc;
} Options;

typedef struct dirent Dirent; 

int getAllPids(int *pids);
int getPidIdx(int pid, int* pids, int pids_cnt);
void printVersionInfo();
void initProc(int pid, Proc* proc);
void parseOptions(Options *options, const int argc, char** argv);
void getNextWord(const char* src, int idx, char* dest);
void getStatusPath(char* status_path, int pid);
int getChildren(int ppid);
void showTree(Options options);

// Helper functions
bool isNumber(const char* dir_name);
int str2num(const char* dir_name);
int readAll(const char* src, char* dest);
void praseProcStatus(const char* status, int length, Proc* proc);