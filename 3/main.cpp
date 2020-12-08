#include "advent.h"
#include "win_console.cpp"

struct visited_coord {
    int x;
    int y;
};

#define LIST_TYPE visited_coord
#include "list.h"

uint visited_coordToboolHashFunction (visited_coord coord) {
    return coord.x * 7 + coord.y * 22;
}

bool visited_coordToboolHashEquals (visited_coord a, visited_coord b) {
    return a.x == b.x && a.y == b.y;
}
#define HASH_MAP_TYPE bool
#define HASH_MAP_KEY_TYPE visited_coord
#include "hash_map.h"

void blockUntilUpPressed (win_console *winConsole, win_console_input *input) {
    bool wait = true;
    while (wait) {
        getWinConsoleInput(winConsole, input);
        if (input->upKey.justPressed) {
            wait = false;
        }
        winConsoleResetInput(input);
        startWinConsoleFrame(winConsole);
        waitForEndOfWinConsoleFrame(winConsole);
    }
}

int visualizeTobogganRouteInConsole (char *map, visited_coord_to_bool_hash_map *visitedCoords, int numRows, int numCols, int slopeX, int slopeY, win_console *winConsole, win_console_window *window, win_console_input *input) {
    clearHashMap(visitedCoords);

    int tobogganX = 0;
    int tobogganY = 0;
    int numHitTrees = 0;

    startWinConsoleFrame(winConsole);
    while (tobogganY < numRows) {
        //blockUntilUpPressed(winConsole, input);
        tobogganX += slopeX;
        tobogganY += slopeY;

        visited_coord coord = {};
        coord.x = tobogganX;
        coord.y = tobogganY;
        bool hitTree = map[tobogganY * numCols + (tobogganX % numCols)] == '#';
        storeHashMapValue(visitedCoords, hitTree, coord);

        if (hitTree) {
            ++numHitTrees;
        }

        for (int i = 0; i < window->height; ++i) {
            for (int j = 0; j < window->width; ++j) {
                char c = ' ';
                visited_coord currentCoord;
                currentCoord.x = j - (window->width/2 - 1);
                currentCoord.y = i - (window->height/2 - 1);

                if (currentCoord.x == 0 && currentCoord.y == 0) {
                    c = '@';
                }
                else {
                    currentCoord.x += tobogganX;
                    currentCoord.y += tobogganY;

                    bool alreadyHitTree = false;
                    bool keyExists = getHashMapValueIfExists(visitedCoords, currentCoord, &alreadyHitTree);
                    if (keyExists) {
                        if (alreadyHitTree) {
                            c = 'X';
                        }
                        else {
                            c = 'O';
                        }
                    }
                    else {
                        if (currentCoord.x >= 0 && currentCoord.y >= 0 && currentCoord.y < numRows) {
                            currentCoord.x = currentCoord.x % numCols;
                            c = map[currentCoord.y * numCols + currentCoord.x];
                        }
                    }
                }

                window->buffer[i * window->width + j] = c;
            }
        }
        writeWinConsoleCharsToWholeScreen(winConsole, window->buffer);

        const uint textLength = 16;
        char numTreesText[textLength];
        intToString(numHitTrees, numTreesText, textLength);
        winConsoleWriteText(winConsole, numTreesText, textLength, 0, 0);

        renderWinConsole(winConsole);
        waitForEndOfWinConsoleFrame(winConsole);
        startWinConsoleFrame(winConsole);
    }

    return numHitTrees;
}


int main (int argc, char **argv) {
    memory_arena memory = {};
    memory.capacity = 1 * 1024 * 1024;
    memory.base = malloc(memory.capacity);

    const int lineLength = 100;
    char line[lineLength];

    char *map = (char *)allocateSize(&memory, 20000);
    visited_coord_to_bool_hash_map visitedCoords = visited_coordToboolHashMapInit(&memory, 500);

    int numRows = 0;
    int numCols = 0;
    int mapIndex = 0;

    fgets(line, lineLength, stdin);
    char *lineCursor = line;
    while (*lineCursor != 0 && *lineCursor != '\n') {
        map[mapIndex++] = *lineCursor++;
        ++numCols;
    }
    ++numRows;
    while (fgets(line, lineLength, stdin)) {
        lineCursor = line;
        while (*lineCursor != 0 && *lineCursor != '\n') {
            map[mapIndex++] = *lineCursor++;
        }
        ++numRows;
    }

    win_console winConsole;
    float targetMSPerFrame = 1000.0f / 30.0f;
    initWinConsole(&winConsole, targetMSPerFrame);
    setWinConsoleOutputActive(&winConsole);

    win_console_window window = {};
    window.width = 150;
    window.height = 50;
    window.buffer = (char *)allocateSize(&memory, window.width * window.height);

    int requiredWindowBufferSize = initWinConsoleBuffer(&winConsole, window.width, window.height);
    void *windowInternalBuffer = allocateSize(&memory, requiredWindowBufferSize);
    setWindowBufferMemory(&winConsole, windowInternalBuffer);

    beginPreciseWindowsTiming();

    win_console_input input = {};

    blockUntilUpPressed(&winConsole, &input);

    int64 slope0NumTrees = visualizeTobogganRouteInConsole(map, &visitedCoords, numRows, numCols, 1, 1, &winConsole, &window, &input);
    blockUntilUpPressed(&winConsole, &input);
    
    int64 slope1NumTrees = visualizeTobogganRouteInConsole(map, &visitedCoords, numRows, numCols, 3, 1, &winConsole, &window, &input);
    blockUntilUpPressed(&winConsole, &input);

    int64 slope2NumTrees = visualizeTobogganRouteInConsole(map, &visitedCoords, numRows, numCols, 5, 1, &winConsole, &window, &input);
    blockUntilUpPressed(&winConsole, &input);

    int64 slope3NumTrees = visualizeTobogganRouteInConsole(map, &visitedCoords, numRows, numCols, 7, 1, &winConsole, &window, &input);
    blockUntilUpPressed(&winConsole, &input);

    int64 slope4NumTrees = visualizeTobogganRouteInConsole(map, &visitedCoords, numRows, numCols, 1, 2, &winConsole, &window, &input);
    blockUntilUpPressed(&winConsole, &input);

    setWinConsoleStandardOutputActive(&winConsole);
    printf("%lld\n", slope0NumTrees * slope1NumTrees * slope2NumTrees * slope3NumTrees * slope4NumTrees);

    releaseWinConsoleHandles(&winConsole);
    endPreciseWindowsTiming();

    return 0;
}
