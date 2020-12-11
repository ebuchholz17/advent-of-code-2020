#include "advent.h"
#include "win_console.cpp"

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

int main (int argc, char **argv) {
    memory_arena memory = {};
    memory.capacity = 1 * 1024 * 1024;
    memory.base = malloc(memory.capacity);

    const int lineLength = 100;
    char line[lineLength];

    char *seatMap = (char *)allocateSize(&memory, 10000);

    int numRows = 0;
    int numCols = 0;
    int mapIndex = 0;

    fgets(line, lineLength, stdin);
    char *lineCursor = line;
    while (*lineCursor != 0 && *lineCursor != '\n') {
        seatMap[mapIndex++] = *lineCursor++;
        ++numCols;
    }
    ++numRows;
    while (fgets(line, lineLength, stdin)) {
        lineCursor = line;
        while (*lineCursor != 0 && *lineCursor != '\n') {
            seatMap[mapIndex++] = *lineCursor++;
        }
        ++numRows;
    }

    win_console winConsole;
    float targetMSPerFrame = 1000.0f / 20.0f;
    initWinConsole(&winConsole, targetMSPerFrame);
    setWinConsoleOutputActive(&winConsole);

    win_console_window window = {};
    window.width = numCols;
    window.height = numRows;
    window.buffer = (char *)allocateSize(&memory, window.width * window.height);

    int requiredWindowBufferSize = initWinConsoleBuffer(&winConsole, window.width, window.height);
    void *windowInternalBuffer = allocateSize(&memory, requiredWindowBufferSize);
    setWindowBufferMemory(&winConsole, windowInternalBuffer);

    winConsoleSetFont(&winConsole, 11, 11, L"Terminal", 1000);

    beginPreciseWindowsTiming();

    char *mapCopy = (char *)allocateSize(&memory, 10000);
    for (int i = 0; i < numRows * numCols; ++i) {
        mapCopy[i] = seatMap[i];
    }

    win_console_input input = {};
    blockUntilUpPressed(&winConsole, &input);

    bool changed = true;
    bool render = true;
    while (changed) {
        changed = false;

        startWinConsoleFrame(&winConsole);

        for (int rowIndex = 0; rowIndex < numRows; ++rowIndex) {
            for (int colIndex = 0; colIndex < numCols; ++colIndex) {
                int numOccupiedSeats = 0;

                //for (int i = rowIndex - 1; i <= rowIndex + 1; ++i) {
                //    for (int j = colIndex - 1; j <= colIndex + 1; ++j) {
                //        if (i == rowIndex && j == colIndex) { 
                //            continue; 
                //        }
                //        if (i < 0 || i >= numRows || j < 0 || j >= numCols) {
                //            continue;
                //        }
                //        if (seatMap[i * numCols + j] == '#') {
                //            ++numOccupiedSeats;
                //        }
                //    }
                //}

                for (int rowDiff = -1; rowDiff <= 1; ++rowDiff) {
                    for (int colDiff = -1; colDiff <= 1; ++colDiff) {
                        if (rowDiff == 0 && colDiff == 0) { 
                            continue; 
                        }
                        int currentRow = rowIndex;
                        int currentCol = colIndex;
                        while (true) {
                            currentRow += rowDiff;
                            currentCol += colDiff;

                            if (currentRow >= 0 && currentRow < numRows && currentCol >= 0 && currentCol < numCols) {
                                if (seatMap[currentRow * numCols + currentCol] == '#') {
                                    ++numOccupiedSeats;
                                    break;
                                }
                                else if (seatMap[currentRow * numCols + currentCol] == 'L') {
                                    break;
                                }
                            }
                            else {
                                break;
                            }
                        }
                    }
                }

                //int numSeatsBeforeLeave = 4;
                int numSeatsBeforeLeave = 5;

                if (seatMap[rowIndex * numCols + colIndex] == 'L' && numOccupiedSeats == 0) {
                    mapCopy[rowIndex * numCols + colIndex] = '#';
                    changed = true;
                }
                else if (seatMap[rowIndex * numCols + colIndex] == '#' && numOccupiedSeats >= numSeatsBeforeLeave) {
                    mapCopy[rowIndex * numCols + colIndex] = 'L';
                    changed = true;
                }
            }
        }

        for (int i = 0; i < window.height; ++i) {
            for (int j = 0; j < window.width; ++j) {
                char c = mapCopy[i * numCols + j];
                window.buffer[i * window.width + j] = c;
            }
        }
        writeWinConsoleCharsToWholeScreen(&winConsole, window.buffer);
        if (render) {
            renderWinConsole(&winConsole);
        }
        render = !render;
        waitForEndOfWinConsoleFrame(&winConsole);

        for (int i = 0; i < numRows * numCols; ++i) {
            seatMap[i] = mapCopy[i];
        }

        //blockUntilUpPressed(&winConsole, &input);
    }

    int numOccupiedSeats = 0;
    for (int rowIndex = 0; rowIndex < numRows; ++rowIndex) {
        for (int colIndex = 0; colIndex < numCols; ++colIndex) {
            if (seatMap[rowIndex * numCols + colIndex] == '#') {
                ++numOccupiedSeats;
            }
        }
    }

    blockUntilUpPressed(&winConsole, &input);

    setWinConsoleStandardOutputActive(&winConsole);
    printf("%d\n", numOccupiedSeats);

    releaseWinConsoleHandles(&winConsole);
    endPreciseWindowsTiming();

    return 0;
}
