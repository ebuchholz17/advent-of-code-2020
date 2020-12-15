#include "advent.h"

enum ferry_cardinal_direction {
    FERRY_CARDINAL_DIRECTION_EAST,
    FERRY_CARDINAL_DIRECTION_SOUTH,
    FERRY_CARDINAL_DIRECTION_WEST,
    FERRY_CARDINAL_DIRECTION_NORTH,
};

enum ferry_relative_direction {
    FERRY_RELATIVE_DIRECTION_FORWARD,
    FERRY_RELATIVE_DIRECTION_LEFT,
    FERRY_RELATIVE_DIRECTION_RIGHT
};

enum ferry_action_type {
    FERRY_ACTION_TYPE_CARDINAL_MOVE,
    FERRY_ACTION_TYPE_FORWARD_MOVE,
    FERRY_ACTION_TYPE_TURN
};

struct ferry_action {
    ferry_action_type type;
    union {
        ferry_cardinal_direction cardinalDirection;
        ferry_relative_direction relativeDirection;
    };
    int amount;
};

#define LIST_TYPE ferry_action
#include "list.h"

struct position {
    int x;
    int y;
};

struct ferry_position {
    position pos;
    ferry_cardinal_direction facing;
};

void blockUntilUpPressed (win_console *winConsole, win_console_input *input) {
    bool wait = true;
    while (wait) {
        getWinConsoleInput(winConsole, input);
        if (input->upKey.justPressed) {
            wait = false;
        }
        winConsoleResetInput(input);
        startWinConsoleFrame(winConsole);
        waitForEndOfWinConsoleFrame(winConsole);
    }
}

int main (int argc, char **argv) {
    memory_arena memory = {};
    memory.capacity = 1 * 1024 * 1024;
    memory.base = malloc(memory.capacity);

    const int lineLength = 10;
    char line[lineLength];

    ferry_action_list ferryActions = ferry_actionListInit(&memory, 1000);

    while (fgets(line, lineLength, stdin)) {
        ferry_action ferryAction = {};
        char dir = ' ';
        void *inputs[] = {&dir, &ferryAction.amount};
        parseInput(line, "%c%d", inputs);
        switch (dir) {
            case 'N': {
                ferryAction.type = FERRY_ACTION_TYPE_CARDINAL_MOVE;
                ferryAction.cardinalDirection = FERRY_CARDINAL_DIRECTION_NORTH;
            } break;
            case 'S': {
                ferryAction.type = FERRY_ACTION_TYPE_CARDINAL_MOVE;
                ferryAction.cardinalDirection = FERRY_CARDINAL_DIRECTION_SOUTH;
            } break;
            case 'E': {
                ferryAction.type = FERRY_ACTION_TYPE_CARDINAL_MOVE;
                ferryAction.cardinalDirection = FERRY_CARDINAL_DIRECTION_EAST;
            } break;
            case 'W': {
                ferryAction.type = FERRY_ACTION_TYPE_CARDINAL_MOVE;
                ferryAction.cardinalDirection = FERRY_CARDINAL_DIRECTION_WEST;
            } break;
            case 'F': {
                ferryAction.type = FERRY_ACTION_TYPE_FORWARD_MOVE;
                ferryAction.relativeDirection = FERRY_RELATIVE_DIRECTION_FORWARD;
            } break;
            case 'L': {
                ferryAction.type = FERRY_ACTION_TYPE_TURN;
                ferryAction.relativeDirection = FERRY_RELATIVE_DIRECTION_LEFT;
            } break;
            case 'R': {
                ferryAction.type = FERRY_ACTION_TYPE_TURN;
                ferryAction.relativeDirection = FERRY_RELATIVE_DIRECTION_RIGHT;
            } break;
        }
        listPush(&ferryActions, ferryAction);
    }

    // part 1
    ferry_position ferry = {};
    ferry.facing = FERRY_CARDINAL_DIRECTION_EAST;

    for (int actionIndex = 0; actionIndex < ferryActions.numValues; ++actionIndex) {
        ferry_action action = ferryActions.values[actionIndex];

        switch (action.type) {
            case FERRY_ACTION_TYPE_CARDINAL_MOVE: {
                switch (action.cardinalDirection) {
                    case FERRY_CARDINAL_DIRECTION_NORTH: {
                        ferry.pos.y += action.amount;
                    } break;
                    case FERRY_CARDINAL_DIRECTION_SOUTH: {
                        ferry.pos.y -= action.amount;
                    } break;
                    case FERRY_CARDINAL_DIRECTION_EAST: {
                        ferry.pos.x += action.amount;
                    } break;
                    case FERRY_CARDINAL_DIRECTION_WEST: {
                        ferry.pos.x -= action.amount;
                    } break;
                    default: {
                        ASSERT(0);
                    } break;
                }
            } break;
            case FERRY_ACTION_TYPE_FORWARD_MOVE: {
                switch (ferry.facing) {
                    case FERRY_CARDINAL_DIRECTION_NORTH: {
                        ferry.pos.y += action.amount;
                    } break;
                    case FERRY_CARDINAL_DIRECTION_SOUTH: {
                        ferry.pos.y -= action.amount;
                    } break;
                    case FERRY_CARDINAL_DIRECTION_EAST: {
                        ferry.pos.x += action.amount;
                    } break;
                    case FERRY_CARDINAL_DIRECTION_WEST: {
                        ferry.pos.x -= action.amount;
                    } break;
                    default: {
                        ASSERT(0);
                    } break;
                }
            } break;
            case FERRY_ACTION_TYPE_TURN: {
                int facingOffset = 0;
                switch (action.amount) {
                    case 90: {
                        facingOffset = 1;
                    } break;
                    case 180: {
                        facingOffset = 2;
                    } break;
                    case 270: {
                        facingOffset = 3;
                    } break;
                    default: {
                        ASSERT(0);
                    } break;
                }
                if (action.relativeDirection == FERRY_RELATIVE_DIRECTION_LEFT) {
                    ferry.facing = (ferry_cardinal_direction)(ferry.facing - facingOffset);
                    while (ferry.facing < 0) {
                        ferry.facing = (ferry_cardinal_direction)(ferry.facing + 4);
                    }
                }
                else if (action.relativeDirection == FERRY_RELATIVE_DIRECTION_RIGHT) {
                    ferry.facing = (ferry_cardinal_direction)(ferry.facing + facingOffset);
                    while (ferry.facing >= 4) {
                        ferry.facing = (ferry_cardinal_direction)(ferry.facing - 4);
                    }
                }
                else {
                    ASSERT(0);
                }
            } break;
            default: {
                ASSERT(0);
            } break;
        }
    }

    int absFerryX = ferry.pos.x < 0 ? -ferry.pos.x : ferry.pos.x;
    int absFerryY = ferry.pos.y < 0 ? -ferry.pos.y : ferry.pos.y;
    printf("x: %d, y: %d, manhattan distance: %d\n", ferry.pos.x, ferry.pos.y, absFerryX + absFerryY);

    // part 2
    position ferryPos = {};
    position wayPointPos = {};
    wayPointPos.x = 10;
    wayPointPos.y = 1;

    for (int actionIndex = 0; actionIndex < ferryActions.numValues; ++actionIndex) {
        ferry_action action = ferryActions.values[actionIndex];

        switch (action.type) {
            case FERRY_ACTION_TYPE_CARDINAL_MOVE: {
                switch (action.cardinalDirection) {
                    case FERRY_CARDINAL_DIRECTION_NORTH: {
                        wayPointPos.y += action.amount;
                    } break;
                    case FERRY_CARDINAL_DIRECTION_SOUTH: {
                        wayPointPos.y -= action.amount;
                    } break;
                    case FERRY_CARDINAL_DIRECTION_EAST: {
                        wayPointPos.x += action.amount;
                    } break;
                    case FERRY_CARDINAL_DIRECTION_WEST: {
                        wayPointPos.x -= action.amount;
                    } break;
                    default: {
                        ASSERT(0);
                    } break;
                }
            } break;
            case FERRY_ACTION_TYPE_FORWARD_MOVE: {
                ferryPos.x += wayPointPos.x * action.amount;
                ferryPos.y += wayPointPos.y * action.amount;
            } break;
            case FERRY_ACTION_TYPE_TURN: {
                int numRotations = 0;
                switch (action.amount) {
                    case 90: {
                        numRotations = 1;
                    } break;
                    case 180: {
                        numRotations = 2;
                    } break;
                    case 270: {
                        numRotations = 3;
                    } break;
                    default: {
                        ASSERT(0);
                    } break;
                }
                if (action.relativeDirection == FERRY_RELATIVE_DIRECTION_LEFT) {
                    for (int rotIndex = 0; rotIndex < numRotations; ++rotIndex) {
                        int temp = wayPointPos.x;
                        wayPointPos.x = -wayPointPos.y;
                        wayPointPos.y = temp;
                    }
                }
                else if (action.relativeDirection == FERRY_RELATIVE_DIRECTION_RIGHT) {
                    for (int rotIndex = 0; rotIndex < numRotations; ++rotIndex) {
                        int temp = wayPointPos.x;
                        wayPointPos.x = wayPointPos.y;
                        wayPointPos.y = -temp;
                    }
                }
                else {
                    ASSERT(0);
                }
            } break;
            default: {
                ASSERT(0);
            } break;
        }
    }

    absFerryX = ferryPos.x < 0 ? -ferryPos.x : ferryPos.x;
    absFerryY = ferryPos.y < 0 ? -ferryPos.y : ferryPos.y;
    printf("x: %d, y: %d, manhattan distance: %d\n", ferryPos.x, ferryPos.y, absFerryX + absFerryY);

    return 0;
}
