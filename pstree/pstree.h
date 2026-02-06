#include <stdbool.h>
#include <dirent.h>

#define MAX_CHILD_NUMBER 50
#define MAX_PROC_NUMBER 100
#define VERSION_INFO "pstree by zrq"

typedef struct Proc {
    char *name;
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
void printVersionInfo();
int initProc(int pid);
void parseOptions(Options *options, const int argc, char** argv);
int getChildren(int ppid);
void showTree(Options options);

// Helper functions
bool isNumber(const char* dir_name);
int str2num(const char* dir_name);