#include "advent.h"

struct spoken_number {
    int number;
    int lastSpoken;
    int lastSpokenBeforeThen;
};

uint intTospoken_numberHashFunction (int a) {
    return (uint)(a * 23535347) + (uint)(a * 12307);
}
bool intTospoken_numberHashEquals (int a, int b) {
    return a == b;
}
#define HASH_MAP_KEY_TYPE int
#define HASH_MAP_TYPE spoken_number
#include "hash_map.h"

int main (int argc, char **argv) {
    memory_arena memory = {};
    memory.capacity = 100 * 1024 * 1024;
    memory.base = malloc(memory.capacity);

    const int lineLength = 50;
    char line[lineLength];

    int_to_spoken_number_hash_map numberSpokenTurns = intTospoken_numberHashMapInit(&memory, 5000000);

    fgets(line, lineLength, stdin);
    char *lineCursor = line;

    spoken_number spokenNumber = {};
    int turn = 1;
    {
        void *inputs[] = {&spokenNumber.number};
        lineCursor = parseInput(lineCursor, "%d", inputs);
    }
    spokenNumber.lastSpoken = turn;
    storeHashMapValue(&numberSpokenTurns, spokenNumber, spokenNumber.number);
    ++turn;

    while (*lineCursor != 0 && *lineCursor != '\n') {
        {
            void *inputs[] = {&spokenNumber.number};
            lineCursor = parseInput(lineCursor, ",%d", inputs);
        }
        spokenNumber.lastSpoken = turn;
        storeHashMapValue(&numberSpokenTurns, spokenNumber, spokenNumber.number);
        ++turn;
    }

    bool firstTimeSpoken = true;
    int lastNumber = spokenNumber.number;
    for (; turn < 30000001; ++turn) {
        if (firstTimeSpoken) {
            lastNumber = 0;
        }
        else {
            spoken_number spokenNum = getHashMapValue(&numberSpokenTurns, lastNumber);
            lastNumber = spokenNum.lastSpoken - spokenNum.lastSpokenBeforeThen;
        }

        if (keyExistsInHashMap(&numberSpokenTurns, lastNumber)) {
            firstTimeSpoken = false;
            spoken_number *spokenNum = getHashMapValuePointer(&numberSpokenTurns, lastNumber);
            spokenNum->lastSpokenBeforeThen = spokenNum->lastSpoken;
            spokenNum->lastSpoken = turn;
        }
        else {
            firstTimeSpoken = true;

            spoken_number spokenNum = {};
            spokenNum.number = lastNumber;
            spokenNum.lastSpoken = turn;

            storeHashMapValue(&numberSpokenTurns, spokenNum, lastNumber);
        }
    }

    printf("%d\n", lastNumber);

    return 0;
}
