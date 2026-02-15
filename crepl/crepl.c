#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <unistd.h>
#include <assert.h>
#include <testkit.h>

#define MAX_LINE_LEN 256
#define MAX_PATH_LEN 256

int exp_cnt = 0; // expressions counter
char tmp_file_path[] = "/tmp/crepl_funcXXXXXX";

void build_function_file(const char* func_def, char* file_path) {
    strcpy(file_path, tmp_file_path);
    int tmp_fd = mkstemp(file_path);
    if(tmp_fd < 0) {
        perror("Error: open file in /tmp failed.\n");
        exit(1);
    }
    // puts(line);
    int line_len = strlen(func_def);
    ssize_t n, written = 0;
    while((n = write(tmp_fd, func_def, line_len - written)) > 0) {
        written += n;
    }
    if(n < 0) {
        perror("Error: write in file failed.\n");
        exit(1);
    }
    // puts(file_path);
    char old_name[MAX_PATH_LEN];
    int len = strlen(file_path);
    strcpy(old_name, file_path);
    file_path[len] = '.';
    file_path[len + 1] = 'c';
    file_path[len + 2] = '\0';
    if(rename(old_name, file_path) < 0) {
        perror("Error: change file name failed.\n");
        exit(1);
    }
    // puts(file_path);
}
UnitTest(test_build_function_file) {
    char line[MAX_LINE_LEN] = "int add (int a, int b) { return a + b; }";
    char file_path[MAX_PATH_LEN];
    build_function_file(line, file_path);
    assert(true);
}

void get_so_file_path(const char* func_file_path, char* so_file_path) {
    strcpy(so_file_path, func_file_path);
    int len = strlen(so_file_path);
    so_file_path[len - 1] = 's';
    so_file_path[len] = 'o';
    so_file_path[len + 1] = '\0';
    // puts(so_file_path);
}
UnitTest(test_get_so_file_path) {
    char line[MAX_LINE_LEN] = "int add (int a, int b) { return a + b; }";
    char file_path[MAX_PATH_LEN];
    build_function_file(line, file_path);
    char so_file_path[MAX_PATH_LEN];
    get_so_file_path(file_path, so_file_path);
    assert(true);
}

void compile_so_file(const char* func_file_path, char* so_file_path) {
    int null_fd = open("/dev/null", O_WRONLY);
    dup2(stderr, null_fd);
    dup2(stdout, null_fd);
    strcpy(so_file_path, func_file_path);
    int len = strlen(so_file_path);
    so_file_path[len - 1] = 's';
    so_file_path[len] = 'o';
    so_file_path[len + 1] = '\0';
    char *argv[] = {"gcc", "-fPIC", "-shared", func_file_path, "-o", so_file_path, NULL};
    execvp("/usr/bin/gcc", argv);
    perror("Error: compile function so file failed.\n");
}

void* load_so_file(const char* so_file_path) {
    void* handle = dlopen(so_file_path, RTLD_LAZY | RTLD_GLOBAL);
    if(handle == NULL) {
        perror("Error: open .so failed.\n");
        exit(1);
    }
}

bool call_func(const char* function_name, int (**func)(void)) {
    void *self = dlopen(NULL, RTLD_NOW);
    if(self == NULL) {
        perror("Error: dlopen self failed.\n");
        return false;
    }
    *func = dlsym(self, function_name);
    if(func == NULL) {
        fprintf(stderr, "Error: cant find function %s.\n", function_name);
        return false;
    }
    return true;
}

bool compile_and_load_so(const char* func_file_path) {
    char so_file_path[MAX_PATH_LEN];
    get_so_file_path(func_file_path, so_file_path);
    int pipe_fd[2];
    pipe(pipe_fd);
    pid_t pid = fork();
    if(pid == 0) { // child process
        close(pipe_fd[0]);
        compile_so_file(func_file_path, so_file_path);
        write(pipe_fd[1], "1", 1);
        close(pipe_fd[1]);
        exit(1);
    }
    else { // parent process
        close(pipe_fd[1]);
        char buf;
        ssize_t n = read(pipe_fd[0], &buf, 1);
        if(n < 0) {
            perror("Error: read from pipe failed.\n");
            return false;
        }
        if(buf == '1') {
            perror("Error: wait for child process failed.\n");
            return false;
        }
        load_so_file(so_file_path);
        close(pipe_fd[0]);
    }
    return true;
}

void get_wrap_def(char* exp, char* wrap_func_name) {
    char wrap_def[MAX_LINE_LEN] = "int ";
    strcat(wrap_def, wrap_func_name);
    strcat(wrap_def, "() { return ");
    int exp_len = strlen(exp); // exp end with \n
    if(exp[exp_len - 1] == '\n')
        exp[exp_len - 1] = '\0';
    strcat(wrap_def, exp);
    strcat(wrap_def, "; }");
    strcpy(exp, wrap_def);
}
UnitTest(test_get_wrap_def) {
    char line[MAX_LINE_LEN] = "gcd(4, 8)";
    char wrap_name[MAX_LINE_LEN] = "__expression_0001";
    get_wrap_def(line, wrap_name);
    // puts(line);
    assert(strcmp(line, "int __expression_0001() { return gcd(4, 8); }") == 0);
}

void get_wrap_func_name(char* wrap_func_name) {
    int base = 1000;
    int len = strlen(wrap_func_name);
    for(int i = 0; i < len; ++i) {
        if(wrap_func_name[i] == 'X') {
            wrap_func_name[i] = (char)(exp_cnt / base + '0');
            base /= 10;
        }
    }
    // puts(wrap_func_name);
}

void parse_command_line(const char* line) {
    char func_file_path[MAX_LINE_LEN];
    strcpy(func_file_path, tmp_file_path);
    bool isExperssion = !(strlen(line) >= 3 && line[0] == 'i' && line[1] == 'n' && line[2] == 't');
    if(isExperssion) { // expression
        char wrap_func_name[MAX_LINE_LEN] = "__expresion_func_XXXX";
        get_wrap_func_name(wrap_func_name);
        ++exp_cnt;
        char wrap_def[MAX_LINE_LEN];
        strcpy(wrap_def, line);
        get_wrap_def(wrap_def, wrap_func_name);
        // puts(wrap_def);
        fflush(stdout);
        build_function_file(wrap_def, func_file_path);
        compile_and_load_so(func_file_path);
        int (*func)(void);
        call_func(wrap_func_name, &func);
        printf("%d.\n", func());
        fflush(stdout);
    }
    else {
        build_function_file(line, func_file_path);
        compile_and_load_so(func_file_path);
        printf("OK.\n");
        fflush(stdout);
    }
}

int main() {
    printf(">> ");
    fflush(stdout);

    char line[MAX_LINE_LEN];
    while(fgets(line, sizeof(line), stdin) != NULL) {
        parse_command_line(line);
        printf(">> ");
        fflush(stdout);
    }

    return 0;
}

// just for test
bool compile_and_load_function(const char* function_def) {
    char func_file_path[MAX_LINE_LEN];
    strcpy(func_file_path, tmp_file_path);
    build_function_file(function_def, func_file_path);
    return compile_and_load_so(func_file_path);
}

bool evaluate_expression(const char* expression, int* result) {
    char wrap[MAX_LINE_LEN];
    char wrap_func_name[MAX_LINE_LEN] = "__expresion_func_XXXX";
    char wrap_def[MAX_LINE_LEN];
    get_wrap_func_name(wrap_func_name);
    ++exp_cnt;
    strcpy(wrap_def, expression);
    get_wrap_def(wrap_def, wrap_func_name);
    // puts(wrap_def);
    if(compile_and_load_function(wrap_def) == false) {
        fprintf(stderr, "Error: compile or load function %s failed.\n", wrap_def);
        return false;
    }
    int (*func)(void);
    if(call_func(wrap_func_name, &func) == false) {
        fprintf(stderr, "Error: call function %s failed.\n", wrap_func_name);
        return false;
    }
    *result = func();
    printf("%d.\n", *result);
    fflush(stdout);
    return true;
}