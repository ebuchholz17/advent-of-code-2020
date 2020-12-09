#ifndef ADVENT_H
#define ADVENT_H

#include <stdio.h> 
#include <stdlib.h> 

#define ASSERT(x) if (!(x)) { *((char *)0) = 0; }

typedef unsigned int uint;
typedef long long int64;
typedef unsigned long long uint64;

struct memory_arena {
    void *base;
    uint64 size;
    uint64 capacity;
};

void *allocateSize (memory_arena *memory, uint64 size) {
    ASSERT(memory->size + size <= memory->capacity);

    void *result = (char *)memory->base + memory->size;
    memory->size += size;
    return result;
}

char *allocateString (char *string, memory_arena *memory) {
    int length = 0;
    char *letter = string;
    while(*letter != 0) {
        ++letter;
        ++length;
    }
    char *newString = (char *)allocateSize(memory, sizeof(char) * (length+1));

    for (int i = 0; i < length; ++i) {
        newString[i] = string[i];
    }
    newString[length] = 0;
    return newString;
}

char *readUntilCharacter (char *currentLetter, char *currentWord, char character) {
    int letterIndex = 0;
    while (*currentLetter != character && *currentLetter != '\n' && currentLetter != 0) {
        currentWord[letterIndex] = *currentLetter;
        letterIndex++;
        currentLetter++;
    }
    currentWord[letterIndex] = 0;
    return currentLetter;
}

char *readInt (char *currentLetter, char *currentWord) {
    int letterIndex = 0;
    while ((*currentLetter >= '0' && *currentLetter <= '9') && *currentLetter != '\n' && currentLetter != 0) {
        currentWord[letterIndex] = *currentLetter;
        letterIndex++;
        currentLetter++;
    }
    currentWord[letterIndex] = 0;
    return currentLetter;
}

bool stringsAreEqual (char *a, char *b) {
    int i, j;
    for (i = 0, j = 0; a[i] && b[j]; ++i, ++j) {
        if (a[i] != b[j]) { 
            return false;
        }
    }
    return a[i] == 0 && b[j] == 0;
}

int stringToInt (char *string) {
    int result = 0;
    bool negative = false;
    if (*string == '-') {
        negative = true;
        ++string;
    }
    while (*string != 0) {
        char digit = *string;
        result *= 10;
        result += (int)(digit - '0');
        ++string;
    }
    if (negative) {
        result = -result;
    }
    return result;
}

// assumes not negative
uint64 stringToUInt64 (char *string) {
    uint64 result = 0;
    while (*string != 0) {
        char digit = *string;
        result *= 10;
        result += (uint64)(digit - '0');
        ++string;
    }
    return result;
}

char *intToString (int num, char *buffer, uint bufferLength) {
    const int maxStringLength = 20;
    char numberBuffer[maxStringLength] = {};

    char *currentDigit = numberBuffer + (maxStringLength - 1);
    char *stringEnd = currentDigit;

    bool isNegative = false;
    if (num < 0) {
        isNegative = true;
        num = -num;
    }

    do {
        --currentDigit;
        *currentDigit = '0' + num % 10;
        num /= 10;
    } while (num != 0);

    if (isNegative) {
        --currentDigit;
        *currentDigit = '-';
    }

    uint stringLength = (uint)(stringEnd - currentDigit);
    ASSERT(stringLength < bufferLength);

    char *string = buffer;
    for (uint i = 0; i < stringLength; ++i) {
        string[i] = *currentDigit;
        ++currentDigit;
    }
    string[stringLength] = 0;

    return string;
}


char *parseInput (char *line, char *format, void *argList[]) {
    char *lineCursor = line;
    char *formatCursor = format;
    bool parsing = true;
    char inputBuffer[255]; // TODO(ebuchholz): error checking
    int argIndex = 0;
    while (parsing) {
        switch (*formatCursor) {
            case '%': {
                ++formatCursor;
                switch (*formatCursor) {
                    case 'd': {
                        lineCursor = readInt(lineCursor, inputBuffer);
                        int *num = (int *)argList[argIndex++];
                        *num = stringToInt(inputBuffer);
                    } break;
                    case 'c': {
                         char *character = (char *)argList[argIndex++];
                         *character = *lineCursor;
                         ++lineCursor;
                    } break;
                    case 's': {
                         // NOTE(ebuchholz): reads until newline, eof, or space
                         lineCursor = readUntilCharacter(lineCursor, inputBuffer, ' ');
                         char *string = (char *)argList[argIndex++];
                         int stringLength = 0;
                         while(inputBuffer[stringLength] != 0) {
                             string[stringLength] = inputBuffer[stringLength];
                             ++stringLength;
                         }
                         string[stringLength] = 0;
                    } break;
                    default: {
                        ASSERT(0);
                    } break;
                }
            } break;
            case 0:
            case '\n': {
                parsing = false;
            } break;
            default: {
                ASSERT(*formatCursor == *lineCursor);
                ++lineCursor;
            } break;
        }
        ++formatCursor;
        if (!(*formatCursor)) {
            parsing = false;
        }
    }
    return lineCursor;
}

#endif
