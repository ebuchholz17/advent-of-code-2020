#include "advent.h"

#define LIST_TYPE int
#include "list.h"

struct joltage_adapter {
    int joltage;
    uint64 waysToReach;
};

#define LIST_TYPE joltage_adapter
#include "list.h"

void quickSortIntRange (int *values, int startIndex, int endIndex) {
    int pivotIndex = endIndex;
    int greaterThanIndex = startIndex;

    for (int unsortedIndex = startIndex; unsortedIndex < pivotIndex; ++unsortedIndex) {
        if (values[unsortedIndex] <= values[pivotIndex]) {
            int temp = values[unsortedIndex];
            values[unsortedIndex] = values[greaterThanIndex];
            values[greaterThanIndex] = temp;

            ++greaterThanIndex;
        }
    }

    int temp = values[pivotIndex];
    values[pivotIndex] = values[greaterThanIndex];
    values[greaterThanIndex] = temp;

    if ((greaterThanIndex - 1) > 0) {
        quickSortIntRange(values, 0, greaterThanIndex - 1);
    }
    if (pivotIndex - (greaterThanIndex + 1) > 0) {
        quickSortIntRange(values, greaterThanIndex + 1, pivotIndex);
    }
}

void quickSortInts (int *values, int length) {
    quickSortIntRange(values, 0, length - 1);
}

int main (int argc, char **argv) {
    memory_arena memory = {};
    memory.capacity = 1 * 1024 * 1024;
    memory.base = malloc(memory.capacity);

    const int lineLength = 5;
    char line[lineLength];

    int_list adapterJoltages = intListInit(&memory, 120);

    while (fgets(line, lineLength, stdin)) {
        int adapterJoltage;
        void *inputs[] = {&adapterJoltage};
        parseInput(line, "%d", inputs);
        listPush(&adapterJoltages, adapterJoltage);
    }

    quickSortInts(adapterJoltages.values, adapterJoltages.numValues);
    int oneJoltageJumps = 0;
    int threeJoltageJumps = 0;
    int joltage = 0;
    for (int i = 0; i < adapterJoltages.numValues; ++i) {
        int adapterJoltage = adapterJoltages.values[i];
        if (adapterJoltage - joltage == 1) {
            ++oneJoltageJumps;
        }
        else if (adapterJoltage - joltage == 3) {
            ++threeJoltageJumps;
        }
        else {
            ASSERT(0);
        }
        joltage = adapterJoltage;
    }
    ++threeJoltageJumps;
    printf("%d * %d = %d\n", oneJoltageJumps, threeJoltageJumps, oneJoltageJumps * threeJoltageJumps);

    joltage_adapter_list joltageAdapters = joltage_adapterListInit(&memory, adapterJoltages.numValues+2);

    joltage_adapter wallAdapter = {};
    wallAdapter.joltage = 0;
    wallAdapter.waysToReach = 1;
    listPush(&joltageAdapters, wallAdapter);

    for (int i = 0; i < adapterJoltages.numValues; ++i) {
        joltage_adapter adapter = {};
        adapter.joltage = adapterJoltages.values[i];
        listPush(&joltageAdapters, adapter);
    }

    joltage_adapter builtInAdapter = {};
    builtInAdapter.joltage = adapterJoltages.values[adapterJoltages.numValues - 1] + 3;
    listPush(&joltageAdapters, builtInAdapter);

    for (int i = 0; i < joltageAdapters.numValues; ++i) {
        joltage_adapter adapter = joltageAdapters.values[i];

        for (int j = 1; j <= 3 && i + j < joltageAdapters.numValues; ++j) {
            joltage_adapter *nextAdapter = &joltageAdapters.values[i+j];
            if (nextAdapter->joltage - adapter.joltage > 3) {
                break;
            }
            else {
                nextAdapter->waysToReach += adapter.waysToReach;
            }
        }
    }

    printf("%llu\n", joltageAdapters.values[joltageAdapters.numValues - 1].waysToReach);

    return 0;
}
