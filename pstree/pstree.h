#include <stdbool.h>

#define MAX_CHILD_NUMBER 50
#define VERSION_INFO "pstree by zrq"

struct Proc {
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
};
struct Options {
    bool show_pid;
    // bool show_ppid;
    // bool show_argv;
    // bool show_path;
    bool sort_children_by_pid_asc;
};

int initProc(int pid);
void initOptions(Options *options, const char* const argv[]);
int getChildren(int ppid);
void showTree(Options options);