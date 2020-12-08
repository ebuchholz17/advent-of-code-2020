#include "advent.h"

struct person_answers {
    bool answers[26];
};

struct group_answers {
    person_answers personAnswers[10];
    int numPeopleInGroup;
};

#define LIST_TYPE group_answers
#include "list.h"

int main (int argc, char **argv) {
    memory_arena memory = {};
    memory.capacity = 1 * 1024 * 1024;
    memory.base = malloc(memory.capacity);

    const int lineLength = 100;
    char line[lineLength];

    group_answers_list groups = group_answersListInit(&memory, 1000);

    bool readingInput = true;
    while (readingInput) {
        group_answers groupAnswers = {};
        fgets(line, lineLength, stdin);

        while(true) {
            person_answers *personAnswers = &groupAnswers.personAnswers[groupAnswers.numPeopleInGroup];
            *personAnswers = {};

            char *lineCursor = line;
            while (*lineCursor && *lineCursor != '\n') {
                personAnswers->answers[*lineCursor - 'a'] = true;
                ++lineCursor;
            }
            ++groupAnswers.numPeopleInGroup;
            ASSERT(groupAnswers.numPeopleInGroup < 10);
            
            char *readResult = fgets(line, lineLength, stdin);
            if (readResult == 0) {
                readingInput = false;
                break;
            }
            else if (*line == '\n') {
                break;
            }
        }
        listPush(&groups, groupAnswers);
    }

    int yesCount = 0;
    for (int i = 0; i < groups.numValues; ++i) {
        group_answers *groupAnswers = &groups.values[i];
        for (int j = 0; j < 26; ++j) {
            bool allAnsweredYes = true;
            for (int k = 0; k < groupAnswers->numPeopleInGroup; ++k) {
                person_answers *personAnswers = &groupAnswers->personAnswers[k];
                if (!personAnswers->answers[j]) {
                    allAnsweredYes = false;
                    break;
                }
            }
            if (allAnsweredYes) {
                ++yesCount;
            }
        }
    }

    printf("%d\n", yesCount);

    return 0;
}
