#include "advent.h"

struct toboggan_password {
    int policyMin;
    int policyMax;
    char policyChar;
    char text[32];
};

#define LIST_TYPE toboggan_password
#include "list.h"

int main (int argc, char **argv) {
    memory_arena memory = {};
    memory.capacity = 1 * 1024 * 1024;
    memory.base = malloc(memory.capacity);

    toboggan_password_list passwords = toboggan_passwordListInit(&memory, 1000);

    const int lineLength = 32;
    char line[lineLength];

    while (fgets(line, lineLength, stdin)) {
        toboggan_password password = {};

        void *inputVals[] = { &password.policyMin, &password.policyMax, &password.policyChar, password.text };
        parseInput(line, "%d-%d %c: %s", 4, inputVals);

        listPush(&passwords, password);
    }

    int numValidPasswords = 0;
    for (int i = 0; i < passwords.numValues; ++i) {
        toboggan_password *password = &passwords.values[i];

        //char *letter = password->text;
        //int numMatchingLetters = 0;
        //while (*letter != 0) {
        //    if (*letter == password->policyChar) {
        //        numMatchingLetters++;
        //    }
        //    ++letter;
        //}
        //if (numMatchingLetters >= password->policyMin && numMatchingLetters <= password->policyMax) {
        //    numValidPasswords++;
        //}

        char *letter = password->text;
        bool passwordValid = false;
        if (password->text[password->policyMin - 1] == password->policyChar) {
            passwordValid = true;
        }
        if (password->text[password->policyMax - 1] == password->policyChar) {
            passwordValid = !passwordValid;
        }
        if (passwordValid) {
            numValidPasswords++;
        }
    }

    printf("%d\n", numValidPasswords);

    return 0;
}
