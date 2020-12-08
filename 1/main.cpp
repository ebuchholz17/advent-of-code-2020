#include "advent.h"

#define LIST_TYPE int
#include "list.h"

int main (int argc, char **argv) {
    memory_arena memory = {};
    memory.capacity = 1 * 1024 * 1024;
    memory.base = malloc(memory.capacity);

    int_list years = intListInit(&memory, 300);

    const int lineLength = 32;
    char line[lineLength];

    while (fgets(line, lineLength, stdin)) {
        int num = atoi(line);
        listPush(&years, num);
    }

    for (int i = 0; i < years.numValues; ++i) {
        int firstYear = years.values[i];
        for (int j = i + 1; j < years.numValues; ++j) {
            int secondYear = years.values[j];
            for (int k = j + 1; k < years.numValues; ++k) {
                int thirdYear = years.values[k];
                if (firstYear + secondYear + thirdYear == 2020) {
                    printf("%d\n", firstYear * secondYear * thirdYear);
                    return 0;
                }
            }
        }
    }

    return 0;
}
