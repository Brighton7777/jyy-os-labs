#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <testkit.h>
#include <unistd.h>
#include <fcntl.h>
#include "labyrinth.h"

int main(int argc, char *argv[]) {
    Labyrinth labyrinth = {0};
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
    labyrinth->rows = 0;
    labyrinth->cols = -1;
    while((n = read(fd, buf, sizeof(buf))) > 0) {
        for(int i = 0; i < n; ++i) {
            if(labyrinth->map[labyrinth->rows][labyrinth->cols] == '\n') {
                labyrinth->map[labyrinth->rows][labyrinth->cols + 1] = '\0';
                ++labyrinth->rows, labyrinth->cols = 0;
            }
            else {
                ++labyrinth->cols;
            } 
            labyrinth->map[labyrinth->rows][labyrinth->cols] = buf[i];
        }
        // printf("%lld\n", n);
    }
    --labyrinth->cols;
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
    return findPos(labyrinth, playerId);
}
UnitTest(testFindPlayer){
    Labyrinth l = {0};
    loadMap(&l, "./maps/find_player.txt");
    showMap(&l);
    Position p1 = findPlayer(&l, '1');
    Position p2 = findPlayer(&l, '2');
    assert(p1.col == 1 && p1.row == 2);
    assert(p2.col == -1 && p2.row == -1);
}
Position findFirstEmptySpace(Labyrinth *labyrinth) {
    return findPos(labyrinth, '.');
}
UnitTest(testFindEmpty){
    Labyrinth l = {0};
    loadMap(&l, "./maps/map.txt");
    showMap(&l);
    Position emptyPos = findFirstEmptySpace(&l);
    printf("(%d, %d)\n", emptyPos.row, emptyPos.col);
    assert(emptyPos.col == 2 && emptyPos.row == 1);
}


bool isEmptySpace(Labyrinth *labyrinth, int row, int col) {
    if(labyrinth->map[row][col] == EMPTY_SPACE)
        return true;
    return false;
}

// Helper direction array
Position ds[4] = { {1, 0}, {-1, 0}, {0, -1}, {0, 1} };
bool movePlayer(Labyrinth *labyrinth, char playerId, const char *direction) {
    Position playerPos = findPos(labyrinth, playerId);
    int dirNumber = dir2num(direction);
    if(dir2num(direction) == -1) {
        perror("Wrong direcion.");
        return false;
    }

    int nextRow = playerPos.row + ds[dirNumber].row;
    int nextCol = playerPos.col + ds[dirNumber].col;
    if(isEmptySpace(labyrinth, nextRow, nextCol) == false) {
        perror("Object position is not empty space.");
        return false;
    }
    labyrinth->map[playerPos.row][playerPos.col] = EMPTY_SPACE; 
    labyrinth->map[nextRow][nextCol] = playerId;
    return true;
}
UnitTest(testMovePlayer) {
    Labyrinth l = {0};
    loadMap(&l, "./maps/map.txt");
    assert(movePlayer(&l, '0', "left") == false);
    assert(movePlayer(&l, '0', "right") == true);
}
bool saveMap(Labyrinth *labyrinth, const char *filename) {
    // TODO: Implement this function
    return false;
}

// Check if all empty spaces are connected using DFS
int dfs(Labyrinth *labyrinth, int row, int col, bool visited[MAX_ROWS][MAX_COLS]) {
    visited[row][col] = true;
    int nextRow, nextCol, emptyCnt = 1;
    for(int i = 0; i < 4; ++i) {
        nextRow = row + ds[i].row;
        nextCol = col + ds[i].col;
        if(isEmptySpace(labyrinth, nextRow, nextCol) && visited[nextRow][nextCol] == false) {
            // printf("(%d, %d)\n", nextRow, nextCol);
            emptyCnt += dfs(labyrinth, nextRow, nextCol, visited);
        }
    }
    return emptyCnt;
}

bool isConnected(Labyrinth *labyrinth) {
    int emptyNumber = getEmptyNumber(labyrinth);
    Position firstEmpty = findFirstEmptySpace(labyrinth);
    printf("First Empty Position is (%d, %d)\n", firstEmpty.row, firstEmpty.col);
    bool visited[MAX_ROWS][MAX_COLS] = {0};
    if(dfs(labyrinth, firstEmpty.row, firstEmpty.col, visited) == emptyNumber)
        return true;
    return false;
}
UnitTest(testIsConnected) {
    Labyrinth l1 = {0}, l2 = {0};
    loadMap(&l1, "./maps/map.txt");
    loadMap(&l2, "./maps/test_is_connected.txt");
    showMap(&l1);
    Position firstEmpty = findFirstEmptySpace(&l1);
    printf("First Empty Position is (%d, %d)\n", firstEmpty.row, firstEmpty.col);
    assert(isConnected(&l1) == true);
    assert(isConnected(&l2) == false);
}

// Some Helper Functions
void showMap(const Labyrinth *const labyrinth) {
    printf("(%d, %d)\n", labyrinth->rows, labyrinth->cols);
    for(int i = 0; i <= labyrinth->rows; ++i) {
        printf("%s", labyrinth->map[i]);
    }
}

Position findPos(const Labyrinth *const labyrinth, char c) {
    Position pos = {-1, -1};
    for(int i = 0; i <= labyrinth->rows; ++i) {
        for(int j = 0; j <= labyrinth->cols; ++j) {
            if(labyrinth->map[i][j] == c) {
                pos.row = i;
                pos.col = j;
                return pos;
            }
        }
    }
    return pos;
}

int getEmptyNumber(const Labyrinth *const labyrinth) {
    int emptyNumber = 0;
    for(int i = 0; i <= labyrinth->rows; ++i)
        for(int j = 0; j <= labyrinth->cols; ++j)
            if(labyrinth->map[i][j] == EMPTY_SPACE)
                ++emptyNumber;
    return emptyNumber;
}
int dir2num(const char* const direction) {
    if(strcmp(direction, "up") == 0)
        return 0;
    else if(strcmp(direction, "down") == 0)
        return 1;
    else if(strcmp(direction, "left") == 0)
        return 2;
    else if(strcmp(direction, "right") == 0)
        return 3;
    return -1;
}