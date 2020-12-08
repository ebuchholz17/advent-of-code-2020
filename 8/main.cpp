#include "advent.h"

enum operation {
    OPERATION_ACC,
    OPERATION_JMP,
    OPERATION_NOP
};

struct instruction {
    operation op;
    int arg;
};

#define LIST_TYPE instruction
#include "list.h"

#define LIST_TYPE int
#include "list.h"

bool runProgram (instruction_list *instructions, int *lineRanCount, int *accumulatorOut) {
    int programCounter = 0;
    int accumulator = 0;

    bool terminated = false;
    while (true) {
        if (programCounter == instructions->numValues) {
            terminated = true;
            break;
        }

        lineRanCount[programCounter]++;
        if (lineRanCount[programCounter] > 1) {
            break;
        }

        instruction inst = instructions->values[programCounter];
        switch (inst.op) {
            case OPERATION_ACC: {
                accumulator += inst.arg;
                programCounter++;
            } break;
            case OPERATION_JMP: {
                programCounter += inst.arg;
            } break;
            case OPERATION_NOP: {
                programCounter++;
            } break;
        }
    }
    *accumulatorOut = accumulator;
    return terminated;
}

int main (int argc, char **argv) {
    memory_arena memory = {};
    memory.capacity = 1 * 1024 * 1024;
    memory.base = malloc(memory.capacity);

    const int lineLength = 10;
    char line[lineLength];

    instruction_list instructions = instructionListInit(&memory, 1000);

    while (fgets(line, lineLength, stdin)) {
        instruction inst = {};

        char *lineCursor = line;
        char buffer[10];
        void *inputs[] = {buffer};
        lineCursor = parseInput(lineCursor, "%s ", inputs);

        if (stringsAreEqual("acc", buffer)) {
            inst.op = OPERATION_ACC;
        }
        else if (stringsAreEqual("jmp", buffer)) {
            inst.op = OPERATION_JMP;
        }
        else if (stringsAreEqual("nop", buffer)) {
            inst.op = OPERATION_NOP;
        }
        else {
            ASSERT(0);
        }

        bool isNegative = *lineCursor == '-';
        ++lineCursor;

        void *nextInput[] = {&inst.arg};
        lineCursor = parseInput(lineCursor, "%d", nextInput);
        if (isNegative) {
            inst.arg = -inst.arg;
        }

        listPush(&instructions, inst);
    }

    int *lineRanCounts = (int *)allocateSize(&memory, 600 * sizeof(int));
    int accumulatorValue = 0;
    runProgram(&instructions, lineRanCounts, &accumulatorValue);
    printf("%d\n", accumulatorValue);

    instruction_list instructionsCopy = listCopy(&instructions, &memory);
    for (int i = 0; i < 600; ++i) {
        lineRanCounts[i] = 0;
    }

    for (int lineToInvert = 0; lineToInvert < instructions.numValues; ++lineToInvert) {
        instruction *inst = &instructionsCopy.values[lineToInvert];
        if (inst->op == OPERATION_JMP) {
            inst->op = OPERATION_NOP;
        }
        else if (inst->op == OPERATION_NOP) {
            inst->op = OPERATION_JMP;
        }
        else {
            continue;
        }

        bool terminated = runProgram(&instructionsCopy, lineRanCounts, &accumulatorValue);
        if (terminated) {
            printf("%d\n", accumulatorValue);
            break;
        }

        listCloneToList(&instructions, &instructionsCopy);
        for (int i = 0; i < 600; ++i) {
            lineRanCounts[i] = 0;
        }
    }


    return 0;
}
