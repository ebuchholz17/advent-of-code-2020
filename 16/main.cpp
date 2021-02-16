#include "advent.h"

struct field_range {
    int min;
    int max;
};

#define FIELDS_PER_TICKET 20

struct ticket_field {
    char name[20];
    field_range ranges[2];
    bool validIndices[FIELDS_PER_TICKET];
};

#define LIST_TYPE ticket_field
#include "list.h"


struct plane_ticket {
    int fields[FIELDS_PER_TICKET];
};

#define LIST_TYPE plane_ticket
#include "list.h"

#define LIST_TYPE int
#include "list.h"

void readTicketInput (char *line, plane_ticket *ticket) {
    //void *inputs[] = {
    //    &ticket->fields[0], 
    //    &ticket->fields[1],
    //    &ticket->fields[2]
    //};
    //parseInput(line, "%d,%d,%d", inputs);

    void *inputs[] = {
        &ticket->fields[0], 
        &ticket->fields[1],
        &ticket->fields[2],
        &ticket->fields[3],
        &ticket->fields[4],
        &ticket->fields[5],
        &ticket->fields[6],
        &ticket->fields[7],
        &ticket->fields[8],
        &ticket->fields[9],
        &ticket->fields[10],
        &ticket->fields[11],
        &ticket->fields[12],
        &ticket->fields[13],
        &ticket->fields[14],
        &ticket->fields[15],
        &ticket->fields[16],
        &ticket->fields[17],
        &ticket->fields[18],
        &ticket->fields[19]
    };
    parseInput(line, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d", inputs);
}

bool orderForFieldIsValid (ticket_field_list *ticketFields, int fieldIndex, int depth, int *checkedFields, int *fieldOrder) {

    if (ticketFields->values[fieldIndex].validIndices[depth]) {
        // found valid ordergg
        if (depth == FIELDS_PER_TICKET - 1) {
            fieldOrder[depth] = fieldIndex;
            return true;
        }
        else {
            for (int i = 0; i < FIELDS_PER_TICKET; ++i) {
                if (checkedFields[i]) {
                    continue;
                }
                checkedFields[i] = true;
                bool fieldOrderIsValid = orderForFieldIsValid(ticketFields, i, depth+1, checkedFields, fieldOrder);
                if (fieldOrderIsValid) {
                    fieldOrder[depth] = fieldIndex;
                    return true;
                }
                checkedFields[i] = false;
            }
            return false;
        }
    }
    else {
        return false;
    }
} 

int main (int argc, char **argv) {
    memory_arena memory = {};
    memory.capacity = 1 * 1024 * 1024;
    memory.base = malloc(memory.capacity);

    const int lineLength = 100;
    char line[lineLength];

    ticket_field_list ticketFields = ticket_fieldListInit(&memory, 20);
    plane_ticket myTicket = {};
    plane_ticket_list nearbyTickets = plane_ticketListInit(&memory, 300);

    fgets(line, lineLength, stdin);
    char *lineCursor = line;
    while (*lineCursor != '\n') {
        ticket_field ticketField = {};
        lineCursor = readUntilCharacter(lineCursor, ticketField.name, ':');
        ++lineCursor;

        {
            void *inputs[] = {&ticketField.ranges[0].min, 
                              &ticketField.ranges[0].max,
                              &ticketField.ranges[1].min,
                              &ticketField.ranges[1].max};
            parseInput(lineCursor, " %d-%d or %d-%d", inputs);
        }
        listPush(&ticketFields, ticketField);

        fgets(line, lineLength, stdin);
        lineCursor = line;
    }

    fgets(line, lineLength, stdin);
    fgets(line, lineLength, stdin);

    readTicketInput(line, &myTicket);

    fgets(line, lineLength, stdin);
    fgets(line, lineLength, stdin);

    while (fgets(line, lineLength, stdin)) {
        plane_ticket nearbyTicket = {};
        readTicketInput(line, &nearbyTicket);
        listPush(&nearbyTickets, nearbyTicket);
    }

    int_list invalidFields = intListInit(&memory, FIELDS_PER_TICKET * nearbyTickets.numValues);
    for (int i = nearbyTickets.numValues - 1; i >= 0; --i) {
        plane_ticket *ticket = &nearbyTickets.values[i];
        bool anyFieldWasInvalid = false;

        for (int j = 0; j < FIELDS_PER_TICKET; ++j) {
            int field = ticket->fields[j];
            bool fieldIsValid = false;

            for (int k = 0; k < ticketFields.numValues; ++k) {
                ticket_field ticketField = ticketFields.values[k];
                if ((field >= ticketField.ranges[0].min && field <= ticketField.ranges[0].max) ||
                    (field >= ticketField.ranges[1].min && field <= ticketField.ranges[1].max))
                {
                    fieldIsValid = true;
                    break;
                }
            }

            if (!fieldIsValid) {
                listPush(&invalidFields, field);
                anyFieldWasInvalid = true;
            }
        }

        if (anyFieldWasInvalid) {
            listSplice(&nearbyTickets, i);
        }
    }

    int sum = 0;
    for (int i = 0; i < invalidFields.numValues; ++i) {
        sum += invalidFields.values[i];
    }

    printf("%d\n", sum);

    // part 2
    for (int fieldIndex = 0; fieldIndex < ticketFields.numValues; ++fieldIndex) {
        ticket_field *ticketField = &ticketFields.values[fieldIndex];
        for (int ticketFieldIndex = 0; ticketFieldIndex < FIELDS_PER_TICKET; ++ticketFieldIndex) {
            ticketField->validIndices[ticketFieldIndex] = true;
            for (int ticketIndex = 0; ticketIndex < nearbyTickets.numValues; ++ticketIndex) {
                int fieldValue = nearbyTickets.values[ticketIndex].fields[ticketFieldIndex];
                if ((fieldValue >= ticketField->ranges[0].min && fieldValue <= ticketField->ranges[0].max) ||
                    (fieldValue >= ticketField->ranges[1].min && fieldValue <= ticketField->ranges[1].max))
                {

                }
                else {
                    ticketField->validIndices[ticketFieldIndex] = false;
                    break;
                }
            }
        }
    }

    memory_arena tempMemory = {};
    tempMemory.capacity = 512 * 1024 * 1024;
    tempMemory.base = malloc(tempMemory.capacity);

    tempMemory.size = 0;

    int checkedFields[FIELDS_PER_TICKET] = {};
    int fieldOrder[FIELDS_PER_TICKET] = {};

    bool foundOrder = false;
    for (int i = 0; i < FIELDS_PER_TICKET; ++i) {
        checkedFields[i] = true;
        bool fieldOrderIsValid = orderForFieldIsValid(&ticketFields, i, 0, checkedFields, fieldOrder);
        if (fieldOrderIsValid) {
            foundOrder = true;
            break;
        }
        checkedFields[i] = false;
    }

    if (foundOrder) {
        uint64 answer = 1;
        for (int i = 0; i < FIELDS_PER_TICKET; ++i) {
            if (fieldOrder[i] <= 5) {
                answer *= (int64)(myTicket.fields[i]);
            }
        }
        printf("%lld\n", answer);
    }

    return 0;
}
