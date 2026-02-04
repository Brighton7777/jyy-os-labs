#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <testkit.h>
#include <unistd.h>
#include <fcntl.h>
#include "labyrinth.h"

int main(int argc, char *argv[]) {
    // TODO: Implement this function
    Labyrinth labyrinth;
    labyrinth.cols = labyrinth.rows = 0;
    loadMap(&labyrinth, "./maps/map.txt");
    return 0;
}

void printUsage() {
    printf("Usage:\n");
    printf("  labyrinth --map map.txt --player id\n");
    printf("  labyrinth -m map.txt -p id\n");
    printf("  labyrinth --map map.txt --player id --move direction\n");
    printf("  labyrinth --version\n");
}

bool isValidPlayer(char playerId) {
    if(playerId >= '0' && playerId <= '9')
        return true;
    return false;
}
UnitTest(testIsVaildPlayer) {
    assert(isValidPlayer('5') == true);
    assert(isValidPlayer('a') == false);
}
bool loadMap(Labyrinth *labyrinth, const char *filename) {
    int fd = open(filename, O_RDONLY);
    if(fd < 0) {
        perror("Open file failed.");
        return false;
    }
    // printf("%d %d\n", labyrinth->rows, labyrinth->cols);
    // printf("%d\n", fd);
    char buf[MAX_COLS * MAX_ROWS];
    ssize_t n;
    while((n = read(fd, buf, sizeof(buf))) > 0) {
        for(int i = 0; i < n; ++i) {
            labyrinth->map[labyrinth->rows][labyrinth->cols++] = buf[i];
            if(buf[i] == '\n') {
                labyrinth->map[labyrinth->rows][labyrinth->cols] = '\0';
                ++labyrinth->rows, labyrinth->cols = 0;
            }   
        }
        // printf("%lld\n", n);
    }
    --labyrinth->rows;
    // printf("%d %d\n", labyrinth->rows, labyrinth->cols);
    // for(int i = 0; i < labyrinth->rows; ++i){
        // printf("%s", labyrinth->map[i]);
    // }
    if(n < 0) {
        perror("Load map failed.");
        return false;
    }
    return true;
}
UnitTest(testLoadMap) {
    Labyrinth labyrinth;
    labyrinth.cols = labyrinth.rows = 0;
    printf("testing.");
    assert(loadMap(&labyrinth, "./maps/map.txt") == true);
}
Position findPlayer(Labyrinth *labyrinth, char playerId) {
    // TODO: Implement this function
    Position pos = {-1, -1};
    return pos;
}

Position findFirstEmptySpace(Labyrinth *labyrinth) {
    // TODO: Implement this function
    Position pos = {-1, -1};
    return pos;
}

bool isEmptySpace(Labyrinth *labyrinth, int row, int col) {
    // TODO: Implement this function
    return false;
}

bool movePlayer(Labyrinth *labyrinth, char playerId, const char *direction) {
    // TODO: Implement this function
    return false;
}

bool saveMap(Labyrinth *labyrinth, const char *filename) {
    // TODO: Implement this function
    return false;
}

// Check if all empty spaces are connected using DFS
void dfs(Labyrinth *labyrinth, int row, int col, bool visited[MAX_ROWS][MAX_COLS]) {
    // TODO: Implement this function
}

bool isConnected(Labyrinth *labyrinth) {
    // TODO: Implement this function
    return false;
}
