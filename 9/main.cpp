#include "advent.h"

#define LIST_TYPE uint64
#include "list.h"

int main (int argc, char **argv) {
    memory_arena memory = {};
    memory.capacity = 1 * 1024 * 1024;
    memory.base = malloc(memory.capacity);

    const int lineLength = 20;
    char line[lineLength];

    uint64_list numbers = uint64ListInit(&memory, 1000);

    while (fgets(line, lineLength, stdin)) {
        char uintString[20];
        char *uintCursor = uintString;
        char *lineCursor = line;
        while (*lineCursor != 0 && *lineCursor != '\n') {
            *uintCursor++ = *lineCursor++;
        }
        *uintCursor = 0;
        uint64 number = stringToUInt64(uintString);
        listPush(&numbers, number);
    }

    int checkIndex = 25;
    uint64 invalidNumber = 0;
    while (checkIndex < numbers.numValues) {
        uint64 checkNumber = numbers.values[checkIndex];
        bool isValid = false;
        for (int i = checkIndex - 25; i < checkIndex; ++i) {
            for (int j = i + 1; j < checkIndex; ++j) {
                if (numbers.values[i] + numbers.values[j] == checkNumber) {
                    isValid = true;
                    break;
                }
            }
            if (isValid) {
                break;
            }
        }

        if (!isValid) {
            invalidNumber = checkNumber;
            break;
        }
        ++checkIndex;
    }

    printf("%llu\n", invalidNumber);

    uint64_list prevSums = numbers;
    int rangeStartIndex = 0;
    int rangeEndIndex = 0;
    for (int i = 1; i < numbers.numValues; ++i) {
        uint64_list ithSums = uint64ListInit(&memory, numbers.numValues);
        bool foundSum = false;
        for (int j = 0; j < prevSums.numValues; ++j) {
            uint64 sum = prevSums.values[j] + numbers.values[i+j];
            listPush(&ithSums, sum);

            if (sum == invalidNumber) {
                rangeStartIndex = j;
                rangeEndIndex = j+i;
                foundSum = true;
                break;
            }
        }
        prevSums = ithSums;

        if (foundSum) {
            break;
        }
    }

    uint64 smallestNumber = 100861876842027;
    uint64 largestNumber = 0; 
    for (int i = rangeStartIndex; i <= rangeEndIndex; ++i) {
        uint64 number = numbers.values[i];
        if (number < smallestNumber) {
            smallestNumber = number;
        }
        if (number > largestNumber) {
            largestNumber = number;
        }
    }
    printf("smallest: %llu, largest: %llu, sum: %llu\n", smallestNumber, largestNumber, smallestNumber + largestNumber);

    return 0;
}
