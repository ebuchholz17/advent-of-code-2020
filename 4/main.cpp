#include "advent.h"
#include "win_console.cpp"

struct passport_fields {
    bool byr;
    bool iyr;
    bool eyr;
    bool hgt;
    bool hcl;
    bool ecl;
    bool pid;
    bool cid;
};

#define LIST_TYPE passport_fields
#include "list.h"

bool validateFourDigitNumberInRange (char *string, int min, int max) {
    bool isFourDigitNumber = true;
    for (int i = 0; i < 4; ++i) {
        if (!string[i]) {
            isFourDigitNumber = false;
            break;
        }
        if (!(string[i] >= '0' && string[i] <= '9')) {
            isFourDigitNumber = false;
            break;
        }
    }
    if (isFourDigitNumber) {
        if (string[4] == 0) {
            int year = stringToInt(string);
            if (year >= min && year <= max) {
                return true;
            }
        }
    }
    return false;
}

void validateField (char *fieldName, passport_fields *passport) {
    char firstHalf[4];
    for (int i = 0; i < 3; ++i) {
        firstHalf[i] = fieldName[i];
    }
    firstHalf[3] = 0;
    char *secondHalf = &fieldName[4];
    if (stringsAreEqual(firstHalf, "byr")) {
        if (validateFourDigitNumberInRange(secondHalf, 1920, 2002)) {
            passport->byr = true;
        }
    }
    else if (stringsAreEqual(firstHalf, "iyr")) {
        if (validateFourDigitNumberInRange(secondHalf, 2010, 2020)) {
            passport->iyr = true;
        }
    }
    else if (stringsAreEqual(firstHalf, "eyr")) {
        if (validateFourDigitNumberInRange(secondHalf, 2020, 2030)) {
            passport->eyr = true;
        }
    }
    else if (stringsAreEqual(firstHalf, "hgt")) {
        int numDigits = 0;
        char heightString[4];
        while (secondHalf[numDigits] >= '0' && secondHalf[numDigits] <= '9' && numDigits < 4) {
            heightString[numDigits] = secondHalf[numDigits];
            ++numDigits;
        }
        if (numDigits > 0 && numDigits < 4) {
            heightString[numDigits] = 0;
            int heightValue = stringToInt(heightString);

            char *unitString = &secondHalf[numDigits];
            numDigits = 0;
            char heightUnits[3];
            while (unitString[numDigits] && numDigits < 3) {
                heightUnits[numDigits] = unitString[numDigits];
                ++numDigits;
            }
            if (numDigits < 3) {
                heightUnits[2] = 0;
                if (stringsAreEqual(heightUnits, "in")) {
                    if (heightValue >= 59 && heightValue <= 76) {
                        passport->hgt = true;
                    }
                }
                else if (stringsAreEqual(heightUnits, "cm")) {
                    if (heightValue >= 150 && heightValue <= 193) {
                        passport->hgt = true;
                    }
                }
            }
        }
    }
    else if (stringsAreEqual(firstHalf, "hcl")) {
        if (secondHalf[0] == '#') {
            int numDigits = 0;
            char *colorString = &secondHalf[1];
            while (((colorString[numDigits] >= '0' && colorString[numDigits] <= '9') ||
                    (colorString[numDigits] >= 'a' && colorString[numDigits] <= 'f')) && 
                   numDigits < 7) 
            {
                ++numDigits;
            }
            if (numDigits == 6) {
                passport->hcl = true;
            }
        }
    }
    else if (stringsAreEqual(firstHalf, "ecl")) {
        if (stringsAreEqual(secondHalf, "amb")) {
            passport->ecl = true;
        }
        else if (stringsAreEqual(secondHalf, "blu")) {
            passport->ecl = true;
        }
        else if (stringsAreEqual(secondHalf, "brn")) {
            passport->ecl = true;
        }
        else if (stringsAreEqual(secondHalf, "gry")) {
            passport->ecl = true;
        }
        else if (stringsAreEqual(secondHalf, "grn")) {
            passport->ecl = true;
        }
        else if (stringsAreEqual(secondHalf, "hzl")) {
            passport->ecl = true;
        }
        else if (stringsAreEqual(secondHalf, "oth")) {
            passport->ecl = true;
        }
    }
    else if (stringsAreEqual(firstHalf, "pid")) {
        int numDigits = 0;
        while (secondHalf[numDigits] >= '0' && secondHalf[numDigits] <= '9' && numDigits < 10) {
            ++numDigits;
        }
        if (numDigits == 9) {
            passport->pid = true;
        }
    }
}

int main (int argc, char **argv) {
    memory_arena memory = {};
    memory.capacity = 1 * 1024 * 1024;
    memory.base = malloc(memory.capacity);

    const int lineLength = 100;
    char line[lineLength];

    passport_fields_list passports = passport_fieldsListInit(&memory, 400);

    bool readingInput = true;
    while (readingInput) {
        fgets(line, lineLength, stdin);
        char *lineCursor = line;

        passport_fields passport = {};
        passport.cid = true;

        while (true) {
            while (true) {
                char field[16] = {};
                void *inputs[] = { field };
                lineCursor = parseInput(lineCursor, "%s", 1, inputs);
                validateField(field, &passport);

                if (*lineCursor == ' ') {
                    ++lineCursor;
                }
                else if (*lineCursor == '\n') {
                    break;
                }
            }

            char *readResult = fgets(line, lineLength, stdin);
            lineCursor = line;
            if (readResult == 0) {
                readingInput = false;
                break;
            }
            else if (*lineCursor == '\n') {
                break;
            }
        }
        listPush(&passports, passport);
    }

    int numValidPassports = 0;
    for (int i = 0; i < passports.numValues; ++i) {
        passport_fields *passport = &passports.values[i];
        if (passport->byr && passport->iyr && passport->eyr && passport->hgt &&
            passport->hcl && passport->ecl && passport->pid)
        {
            ++numValidPassports;
        }
    }

    printf("%d\n", numValidPassports);

    return 0;
}
