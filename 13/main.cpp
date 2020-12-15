#include "advent.h"

struct bus_schedule {
    int64 ID;
    int64 timeStampOffset;
};

#define LIST_TYPE bus_schedule
#include "list.h"

int main (int argc, char **argv) {
    memory_arena memory = {};
    memory.capacity = 1 * 1024 * 1024;
    memory.base = malloc(memory.capacity);

    const int lineLength = 200;
    char line[lineLength];

    bus_schedule_list buses = bus_scheduleListInit(&memory, 10);

    fgets(line, lineLength, stdin);

    int timeStamp;
    void *inputs[] = {&timeStamp};
    parseInput(line, "%d", inputs);

    fgets(line, lineLength, stdin);
    char *lineCursor = line;
    char numberBuffer[6];
    int timeStampOffset = 0;
    while (*lineCursor != 0) {
        lineCursor = readUntilCharacter(lineCursor, numberBuffer, ',');
        ++lineCursor;

        if (numberBuffer[0] != 'x') {
            bus_schedule bus = {};
            bus.ID = stringToInt(numberBuffer);
            bus.timeStampOffset = timeStampOffset;
            listPush(&buses, bus);
        }
        ++timeStampOffset;
    }

    int64 shortestWait = 9999999;
    int64 soonestBusID = 0;
    for (int busIndex = 0; busIndex < buses.numValues; ++busIndex) {
        int64 busID = buses.values[busIndex].ID;
        int64 wait = busID - (timeStamp % busID);
        if (wait < shortestWait) {
            shortestWait = wait;
            soonestBusID = busID;
        }
    }

    printf("%llu\n", shortestWait * soonestBusID);

    int64 currentTime = 0;
    bus_schedule firstBus = buses.values[0];
    int64 delta = firstBus.ID;
    for (int i = 1; i < buses.numValues; ++i) {
        bus_schedule secondBus = buses.values[i];

        while (true) {
            int64 timeOffset = (currentTime + secondBus.timeStampOffset) % secondBus.ID;
            if (timeOffset == 0) {
                delta *= secondBus.ID;
                break;
            }

            currentTime += delta;
        }
    }

    printf("%lld\n", currentTime);

    return 0;
}
