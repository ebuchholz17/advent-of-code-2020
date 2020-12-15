#include "advent.h"

struct bitmask {
    uint64 ones;
    uint64 zeroes;

    int scanIndex;
    char string[37];
};

#define LIST_TYPE bitmask
#include "list.h"

struct instruction {
    uint64 address;
    uint64 value;
    int maskID;
};

#define LIST_TYPE instruction
#include "list.h"

uint uint64Touint64HashFunction (uint64 a) {
    return (uint)(a * 2353534590892437L) + (uint)(a * 12307L);
}
bool uint64Touint64HashEquals (uint64 a, uint64 b) {
    return a == b;
}
#define HASH_MAP_KEY_TYPE uint64
#define HASH_MAP_TYPE uint64
#include "hash_map.h"

void readBit (char num, bitmask *mask) {
    if (num == '1') {
        mask->ones |= 1;
        mask->zeroes |= 1;
    }
    else if (num == '0') {
        mask->ones &= 0x000000fffffffffe;
        mask->zeroes &= 0x000000fffffffffe;
    }
    else if (num == 'X') {
        mask->ones &= 0x000000fffffffffe;
        mask->zeroes |= 1;
    }
    else {
        ASSERT(0);
    }
}

int main (int argc, char **argv) {
    memory_arena memory = {};
    memory.capacity = 64 * 1024 * 1024;
    memory.base = malloc(memory.capacity);

    const int lineLength = 50;
    char line[lineLength];

    bitmask_list bitmasks = bitmaskListInit(&memory, 200);
    instruction_list instructions = instructionListInit(&memory, 600);

    fgets(line, lineLength, stdin);
    char word[50];
    char *lineCursor;
    {
        void *inputs[] = {word};
        lineCursor = parseInput(line, "%w", inputs);
    }

    while (true) {
        if (stringsAreEqual(word, "mask")) {
            bitmask mask = {};
            {
                void *inputs[] = {mask.string};
                parseInput(lineCursor, " = %s", inputs);
            }
            readBit(mask.string[0], &mask);
            for (int i = 1; i < 36; ++i) {
                mask.ones = mask.ones << 1;
                mask.zeroes = mask.zeroes << 1;

                readBit(mask.string[i], &mask);
            }
            listPush(&bitmasks, mask);

            char *readResult = fgets(line, lineLength, stdin);
            {
                void *inputs[] = {word};
                lineCursor = parseInput(line, "%w", inputs);
            }
            while (readResult && stringsAreEqual(word, "mem")) {
                instruction inst = {};
                {
                    void *inputs[] = {&inst.address, &inst.value};
                    parseInput(lineCursor, "[%d] = %d", inputs);
                }
                inst.maskID = bitmasks.numValues - 1;
                listPush(&instructions, inst);

                readResult = fgets(line, lineLength, stdin);
                if (readResult) {
                    void *inputs[] = {word};
                    lineCursor = parseInput(line, "%w", inputs);
                }
            }

            if (readResult == 0) {
                break;
            }
        }
        else {
            ASSERT(0);
        }
    }

    // part 1
    const int memsize = 100000;
    uint64 *programMemory = (uint64 *)allocateSize(&memory, memsize * sizeof(uint64));
    for (int i = 0; i < memsize; ++i) {
        programMemory[i] = 0;
    }

    for (int i = 0; i < instructions.numValues; ++i) {
        instruction inst = instructions.values[i];
        bitmask mask = bitmasks.values[inst.maskID];
        uint64 value = inst.value;
        value |= mask.ones;
        value &= mask.zeroes;
        programMemory[inst.address] = value;
    }

    uint64 sum = 0;
    for (int i = 0; i < memsize; ++i) {
        sum += programMemory[i];
    }

    printf("%llu\n", sum);

    // part 2
    uint64_to_uint64_hash_map sparseMemory = uint64Touint64HashMapInit(&memory, 100000);
    bitmask_list addressCombinations = bitmaskListInit(&memory, 512);

    for (int instIndex = 0; instIndex < instructions.numValues; ++instIndex) {
        instruction inst = instructions.values[instIndex];
        bitmask mask = bitmasks.values[inst.maskID];

        uint64 instAddress = inst.address;
        char addressString[37];
        for (int i = 35; i >= 0; --i) {
            char num = (instAddress & 1) + '0';
            addressString[i] = num;
            instAddress = instAddress >> 1;
        }
        addressString[36] = 0;

        listPush(&addressCombinations, mask);
        while (addressCombinations.numValues > 0) {
            bitmask addressCombination = listSplice(&addressCombinations, 0);
            bool copiedAddress = false;
            while (addressCombination.scanIndex < 36) {
                char c = addressCombination.string[addressCombination.scanIndex];
                int scanIndex = addressCombination.scanIndex;
                ++addressCombination.scanIndex;
                switch (c) {
                    case '0': {
                        addressCombination.string[scanIndex] = addressString[scanIndex];
                    } break;
                    case '1': {
                        addressCombination.string[scanIndex] = '1';
                    } break;
                    case 'X': {
                        copiedAddress = true;

                        bitmask zeroAddressCopy = addressCombination;
                        zeroAddressCopy.string[scanIndex] = '0';
                        listPush(&addressCombinations, zeroAddressCopy);

                        bitmask oneAddressCopy = addressCombination;
                        oneAddressCopy.string[scanIndex] = '1';
                        listPush(&addressCombinations, oneAddressCopy);
                    } break;
                    default: {
                        ASSERT(0);
                    } break;
                }
                if (copiedAddress) {
                    break;
                }
            }

            if (!copiedAddress) {
                uint64 address = 0;
                char c = addressCombination.string[0];
                if (c == '0') {
                    address &= 0x000000fffffffffe;
                }
                else if (c == '1') {
                    address |= 1;
                }
                else {
                    ASSERT(0);
                }
                for (int i = 1; i < 36; ++i) {
                    address = address << 1;
                    c = addressCombination.string[i];
                    if (c == '0') {
                        address &= 0x000000fffffffffe;
                    }
                    else if (c == '1') {
                        address |= 1;
                    }
                    else {
                        ASSERT(0);
                    }
                }
                storeHashMapValue(&sparseMemory, inst.value, address);
            }
        }

    }

    sum = 0;
    for (uint memoryIndex = 0; memoryIndex < sparseMemory.capacity; ++memoryIndex) {
        uint64_to_uint64_hash_map_value *memValue = &sparseMemory.entries[memoryIndex];
        if (memValue->enteredIntoHashmap) {
            uint64 value = memValue->value;
            sum += value;
        }
    }
    printf("%llu\n", sum);

    return 0;
}
