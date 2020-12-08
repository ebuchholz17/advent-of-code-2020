#include "advent.h"

enum bag_pattern {
    BAG_PATTERN_WAVY,
    BAG_PATTERN_DRAB,
    BAG_PATTERN_PALE,
    BAG_PATTERN_VIBRANT,
    BAG_PATTERN_DIM,
    BAG_PATTERN_POSH,
    BAG_PATTERN_CLEAR,
    BAG_PATTERN_FADED,
    BAG_PATTERN_DOTTED,
    BAG_PATTERN_DARK,
    BAG_PATTERN_BRIGHT,
    BAG_PATTERN_LIGHT,
    BAG_PATTERN_SHINY,
    BAG_PATTERN_MIRRORED,
    BAG_PATTERN_DULL,
    BAG_PATTERN_MUTED,
    BAG_PATTERN_STRIPED,
    BAG_PATTERN_PLAID
};

enum bag_color {
    BAG_COLOR_MAROON,
    BAG_COLOR_GOLD,
    BAG_COLOR_CORAL,
    BAG_COLOR_TURQUOISE,
    BAG_COLOR_SALMON,
    BAG_COLOR_BLUE,
    BAG_COLOR_CRIMSON,
    BAG_COLOR_SILVER,
    BAG_COLOR_PURPLE,
    BAG_COLOR_CYAN,
    BAG_COLOR_TOMATO,
    BAG_COLOR_AQUA,
    BAG_COLOR_FUCHSIA,
    BAG_COLOR_BLACK,
    BAG_COLOR_OLIVE,
    BAG_COLOR_BEIGE,
    BAG_COLOR_MAGENTA,
    BAG_COLOR_BRONZE,
    BAG_COLOR_GREEN,
    BAG_COLOR_TEAL,
    BAG_COLOR_RED,
    BAG_COLOR_VIOLET,
    BAG_COLOR_LIME,
    BAG_COLOR_ORANGE,
    BAG_COLOR_LAVENDER,
    BAG_COLOR_INDIGO,
    BAG_COLOR_GRAY,
    BAG_COLOR_WHITE,
    BAG_COLOR_TAN,
    BAG_COLOR_YELLOW,
    BAG_COLOR_PLUM,
    BAG_COLOR_CHARTREUSE,
    BAG_COLOR_BROWN
};

unsigned int hashString (char *string) {
    unsigned int hash = 5381;
    int c;
    ASSERT(string);
    do {
        c = *string++;
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    } while (c);

    return hash;
}

uint char_ptrTobag_patternHashFunction (char *a) {
    return hashString(a);
}
bool char_ptrTobag_patternHashEquals (char *a, char *b) {
    return stringsAreEqual(a, b);
}
typedef char *char_ptr;
#define HASH_MAP_KEY_TYPE char_ptr
#define HASH_MAP_TYPE bag_pattern
#include "hash_map.h"

uint char_ptrTobag_colorHashFunction (char *a) {
    return hashString(a);
}
bool char_ptrTobag_colorHashEquals (char *a, char *b) {
    return stringsAreEqual(a, b);
}
#define HASH_MAP_KEY_TYPE char_ptr
#define HASH_MAP_TYPE bag_color
#include "hash_map.h"

struct bag_type {
    bag_pattern pattern;
    bag_color color;
};

struct contained_bag {
    bag_type type;
    int amountContained;
};

struct bag_node {
    bag_type type;
    contained_bag childBags[5];
    int numChildBags;
    bag_type parentBags[20];
    int numParentBags;
};

uint bag_typeTobag_nodeHashFunction (bag_type type) {
    return type.pattern * 27 + type.color * 131;
}
bool bag_typeTobag_nodeHashEquals (bag_type a, bag_type b) {
    return a.pattern == b.pattern && a.color == b.color;
}
#define HASH_MAP_KEY_TYPE bag_type
#define HASH_MAP_TYPE bag_node
#include "hash_map.h"

uint bag_typeToboolHashFunction (bag_type type) {
    return type.pattern * 27 + type.color * 131;
}
bool bag_typeToboolHashEquals (bag_type a, bag_type b) {
    return a.pattern == b.pattern && a.color == b.color;
}
#define HASH_MAP_KEY_TYPE bag_type
#define HASH_MAP_TYPE bool
#include "hash_map.h"

bag_type getPatternAndColorTypes (char *patternString, char *colorString, 
                                  char_ptr_to_bag_pattern_hash_map *bagPatterns,
                                  char_ptr_to_bag_color_hash_map *bagColors)
{
    bag_type result = {};
    result.pattern = getHashMapValue(bagPatterns, patternString);
    result.color = getHashMapValue(bagColors, colorString);
    return result;
}

int amountOfParentBags (bag_node *node, bag_type_to_bag_node_hash_map *bagNodes, bag_type_to_bool_hash_map *seenTypes) {
    int result = 0;

    storeHashMapValue(seenTypes, true, node->type);
    for (int i = 0; i < node->numParentBags; ++i) {
        bag_type parentType = node->parentBags[i];
        bag_node *parentNode = getHashMapValuePointer(bagNodes, parentType);
        if (!keyExistsInHashMap(seenTypes, parentNode->type)) {
            result += 1 + amountOfParentBags(parentNode, bagNodes, seenTypes);
        }
    }
    return result;
}

int amountOfBagsContained (bag_node *node, bag_type_to_bag_node_hash_map *bagNodes) {
    int result = 0;

    for (int i = 0; i < node->numChildBags; ++i) {
        contained_bag containedBag = node->childBags[i];
        bag_node *childNode = getHashMapValuePointer(bagNodes, containedBag.type);
        result += containedBag.amountContained * (1 + amountOfBagsContained(childNode, bagNodes));
    }
    return result;
}

int main (int argc, char **argv) {
    memory_arena memory = {};
    memory.capacity = 1 * 1024 * 1024;
    memory.base = malloc(memory.capacity);

    const int lineLength = 200;
    char line[lineLength];

    char_ptr_to_bag_pattern_hash_map bagPatterns = char_ptrTobag_patternHashMapInit(&memory, 200);
    storeHashMapValue(&bagPatterns, BAG_PATTERN_WAVY, "wavy");
    storeHashMapValue(&bagPatterns, BAG_PATTERN_DRAB, "drab");
    storeHashMapValue(&bagPatterns, BAG_PATTERN_PALE, "pale");
    storeHashMapValue(&bagPatterns, BAG_PATTERN_VIBRANT, "vibrant");
    storeHashMapValue(&bagPatterns, BAG_PATTERN_DIM, "dim");
    storeHashMapValue(&bagPatterns, BAG_PATTERN_POSH, "posh");
    storeHashMapValue(&bagPatterns, BAG_PATTERN_CLEAR, "clear");
    storeHashMapValue(&bagPatterns, BAG_PATTERN_FADED, "faded");
    storeHashMapValue(&bagPatterns, BAG_PATTERN_DOTTED, "dotted");
    storeHashMapValue(&bagPatterns, BAG_PATTERN_DARK, "dark");
    storeHashMapValue(&bagPatterns, BAG_PATTERN_BRIGHT, "bright");
    storeHashMapValue(&bagPatterns, BAG_PATTERN_LIGHT, "light");
    storeHashMapValue(&bagPatterns, BAG_PATTERN_SHINY, "shiny");
    storeHashMapValue(&bagPatterns, BAG_PATTERN_MIRRORED, "mirrored");
    storeHashMapValue(&bagPatterns, BAG_PATTERN_DULL, "dull");
    storeHashMapValue(&bagPatterns, BAG_PATTERN_MUTED, "muted");
    storeHashMapValue(&bagPatterns, BAG_PATTERN_STRIPED, "striped");
    storeHashMapValue(&bagPatterns, BAG_PATTERN_PLAID, "plaid");

    char_ptr_to_bag_color_hash_map bagColors = char_ptrTobag_colorHashMapInit(&memory, 200);
    storeHashMapValue(&bagColors, BAG_COLOR_MAROON, "maroon");
    storeHashMapValue(&bagColors, BAG_COLOR_GOLD, "gold");
    storeHashMapValue(&bagColors, BAG_COLOR_CORAL, "coral");
    storeHashMapValue(&bagColors, BAG_COLOR_TURQUOISE, "turquoise");
    storeHashMapValue(&bagColors, BAG_COLOR_SALMON, "salmon");
    storeHashMapValue(&bagColors, BAG_COLOR_BLUE, "blue");
    storeHashMapValue(&bagColors, BAG_COLOR_CRIMSON, "crimson");
    storeHashMapValue(&bagColors, BAG_COLOR_SILVER, "silver");
    storeHashMapValue(&bagColors, BAG_COLOR_PURPLE, "purple");
    storeHashMapValue(&bagColors, BAG_COLOR_CYAN, "cyan");
    storeHashMapValue(&bagColors, BAG_COLOR_TOMATO, "tomato");
    storeHashMapValue(&bagColors, BAG_COLOR_AQUA, "aqua");
    storeHashMapValue(&bagColors, BAG_COLOR_FUCHSIA, "fuchsia");
    storeHashMapValue(&bagColors, BAG_COLOR_BLACK, "black");
    storeHashMapValue(&bagColors, BAG_COLOR_OLIVE, "olive");
    storeHashMapValue(&bagColors, BAG_COLOR_BEIGE, "beige");
    storeHashMapValue(&bagColors, BAG_COLOR_MAGENTA, "magenta");
    storeHashMapValue(&bagColors, BAG_COLOR_BRONZE, "bronze");
    storeHashMapValue(&bagColors, BAG_COLOR_GREEN, "green");
    storeHashMapValue(&bagColors, BAG_COLOR_TEAL, "teal");
    storeHashMapValue(&bagColors, BAG_COLOR_RED, "red");
    storeHashMapValue(&bagColors, BAG_COLOR_VIOLET, "violet");
    storeHashMapValue(&bagColors, BAG_COLOR_LIME, "lime");
    storeHashMapValue(&bagColors, BAG_COLOR_ORANGE, "orange");
    storeHashMapValue(&bagColors, BAG_COLOR_LAVENDER, "lavender");
    storeHashMapValue(&bagColors, BAG_COLOR_INDIGO, "indigo");
    storeHashMapValue(&bagColors, BAG_COLOR_GRAY, "gray");
    storeHashMapValue(&bagColors, BAG_COLOR_WHITE, "white");
    storeHashMapValue(&bagColors, BAG_COLOR_TAN, "tan");
    storeHashMapValue(&bagColors, BAG_COLOR_YELLOW, "yellow");
    storeHashMapValue(&bagColors, BAG_COLOR_PLUM, "plum");
    storeHashMapValue(&bagColors, BAG_COLOR_CHARTREUSE, "chartreuse");
    storeHashMapValue(&bagColors, BAG_COLOR_BROWN, "brown");

    bag_type_to_bag_node_hash_map bagNodes = bag_typeTobag_nodeHashMapInit(&memory, 1000);

    while (fgets(line, lineLength, stdin)) {
        bag_node bagNode = {};
        contained_bag containedBag = {};
        char patternString[20];
        char colorString[20];
        char containPattern[20];
        char containColor[20];
        char *lineCursor = line;
        void *inputs[] = {patternString, colorString};
        lineCursor = parseInput(lineCursor, "%s %s bags contain ", inputs);
        bagNode.type = getPatternAndColorTypes(patternString, colorString, &bagPatterns, &bagColors);

        if (*lineCursor == 'n') {
            // no contained bags
        }
        else {
            while (true) {
                void *nextInputs[] = {&containedBag.amountContained, containPattern, containColor };
                lineCursor = parseInput(lineCursor, "%d %s %s bag", nextInputs);
                if (*lineCursor == 's') {
                    ++lineCursor;
                }

                containedBag.type = getPatternAndColorTypes(containPattern, containColor, &bagPatterns, &bagColors);
                bagNode.childBags[bagNode.numChildBags] = containedBag;
                ++bagNode.numChildBags;

                if (*lineCursor == '.') {
                    break;
                }
                lineCursor += 2;
            }
        }
        storeHashMapValue(&bagNodes, bagNode, bagNode.type);
    }

    // construct reverse graph
    bag_node *shinyGoldBag = 0;
    for (uint nodeIndex = 0; nodeIndex < bagNodes.capacity; ++nodeIndex) {
        bag_type_to_bag_node_hash_map_value *nodeValue = &bagNodes.entries[nodeIndex];
        if (nodeValue->enteredIntoHashmap) {
            bag_node *node = &nodeValue->value;

            if (node->type.pattern == BAG_PATTERN_SHINY && node->type.color == BAG_COLOR_GOLD) {
                shinyGoldBag = node;
            }

            for (int i = 0; i < node->numChildBags; ++i) {
                contained_bag containedBag = node->childBags[i];
                bag_node *childNode = getHashMapValuePointer(&bagNodes, containedBag.type);

                bool alreadyHaveParentBagType = false;
                for (int j = 0; j < childNode->numParentBags; ++j) {
                    bag_type parentType = childNode->parentBags[j];
                    if (parentType.pattern == node->type.pattern && parentType.color == node->type.color) {
                        alreadyHaveParentBagType = true;
                        break;
                    }
                }
                if (!alreadyHaveParentBagType) {
                    childNode->parentBags[childNode->numParentBags++] = node->type;
                    ASSERT(childNode->numParentBags <= 20);
                }
            }
        }
    }

    bag_type_to_bool_hash_map seenTypes = bag_typeToboolHashMapInit(&memory, 1000);

    int numGoldContainingBags = amountOfParentBags(shinyGoldBag, &bagNodes, &seenTypes);
    printf("%d\n", numGoldContainingBags);

    int numContainedBags = amountOfBagsContained(shinyGoldBag, &bagNodes);
    printf("%d\n", numContainedBags);

    return 0;
}
