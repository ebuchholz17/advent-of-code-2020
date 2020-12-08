#include "advent.h"

struct boarding_pass {
    char seat[11];
};

#define LIST_TYPE boarding_pass
#include "list.h"

#define LIST_TYPE int
#include "list.h"

int main (int argc, char **argv) {
    memory_arena memory = {};
    memory.capacity = 1 * 1024 * 1024;
    memory.base = malloc(memory.capacity);

    const int lineLength = 100;
    char line[lineLength];

    boarding_pass_list boardingPasses = boarding_passListInit(&memory, 1000);
    int_list seatIDs = intListInit(&memory, 1000);

    while(fgets(line, lineLength, stdin)) {
        boarding_pass pass = {};
        for (int i = 0; i < 10; ++i) {
            pass.seat[i] = line[i];
        }
        listPush(&boardingPasses, pass);
    }

    //int highestSeatID = 0;
    for (int passIndex = 0; passIndex < boardingPasses.numValues; ++passIndex) {
        boarding_pass *pass = &boardingPasses.values[passIndex];
        int rowMin = 0;
        int rowMax = 128;
        for (int seatIndex = 0; seatIndex < 7; ++seatIndex) {
            int half = (rowMax - rowMin) / 2;
            if (pass->seat[seatIndex] == 'F') {
                rowMax -= half;
            }
            else if (pass->seat[seatIndex] == 'B') {
                rowMin += half;
            }
        }
        ASSERT(rowMin == rowMax - 1);
        int row = rowMin;

        int colMin = 0;
        int colMax = 8;
        for (int seatIndex = 7; seatIndex < 10; ++seatIndex) {
            int half = (colMax - colMin) / 2;
            if (pass->seat[seatIndex] == 'L') {
                colMax -= half;
            }
            else if (pass->seat[seatIndex] == 'R') {
                colMin += half;
            }
        }
        ASSERT(colMin == colMax - 1);
        int col = colMin;

        int seatID = row * 8 + col;
        //if (seatID > highestSeatID) {
        //    highestSeatID = seatID;
        //}

        bool inserted = false;
        for (int i = 0; i < seatIDs.numValues; ++i) {
            if (seatID < seatIDs.values[i]) {
                listInsert(&seatIDs, seatID, i);
                inserted = true;
                break;
            }
        }
        if (!inserted) {
            listPush(&seatIDs, seatID);
        }
    }

    for (int i = 1; i < seatIDs.numValues; ++i) {
        if (seatIDs.values[i] != seatIDs.values[i-1] + 1) {
            printf("%d\n", seatIDs.values[i-1]);
            printf("%d\n", seatIDs.values[i]);
        }
    }

    //printf("%d\n", highestSeatID);

    return 0;
}
