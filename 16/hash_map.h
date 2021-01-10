#ifndef HASH_MAP_H 
#define HASH_MAP_H

#define HASH_MAP_TYPE_STRUCT(x, y) HASH_MAP_TYPE_STRUCT_(x, y)
#define HASH_MAP_TYPE_STRUCT_(x, y) x##_to_##y##_hash_map
#define HASH_MAP_ENTRY_STRUCT(x, y) HASH_MAP_ENTRY_STRUCT_(x, y)
#define HASH_MAP_ENTRY_STRUCT_(x, y) x##_to_##y##_hash_map_value
#define HASH_MAP_INIT(x, y) HASH_MAP_INIT_(x, y)
#define HASH_MAP_INIT_(x, y) x##To##y##HashMapInit
#define HASH_MAP_HASH_FUNCTION(x, y) HASH_MAP_HASH_FUNCTION_(x, y)
#define HASH_MAP_HASH_FUNCTION_(x, y) x##To##y##HashFunction
#define HASH_MAP_EQUALITY_FUNCTION(x, y) HASH_MAP_EQUALITY_FUNCTION_(x, y)
#define HASH_MAP_EQUALITY_FUNCTION_(x, y) x##To##y##HashEquals

#endif

// TODO(ebuchholz): generate names from type+key type, not just type
struct HASH_MAP_ENTRY_STRUCT(HASH_MAP_KEY_TYPE, HASH_MAP_TYPE) {
    HASH_MAP_KEY_TYPE key;
    HASH_MAP_TYPE value;
    bool enteredIntoHashmap;
};

struct HASH_MAP_TYPE_STRUCT(HASH_MAP_KEY_TYPE, HASH_MAP_TYPE) {
    HASH_MAP_ENTRY_STRUCT(HASH_MAP_KEY_TYPE, HASH_MAP_TYPE) *entries;
    unsigned int capacity;
};

void storeHashMapValue (HASH_MAP_TYPE_STRUCT(HASH_MAP_KEY_TYPE, HASH_MAP_TYPE) *hashMap, HASH_MAP_TYPE value, HASH_MAP_KEY_TYPE key) {
    unsigned int hash = HASH_MAP_HASH_FUNCTION(HASH_MAP_KEY_TYPE, HASH_MAP_TYPE)(key);
    unsigned int mapIndex = hash % hashMap->capacity;
    unsigned int originalMapIndex = mapIndex; // check that we actually find it after looping all the way around
    HASH_MAP_ENTRY_STRUCT(HASH_MAP_KEY_TYPE, HASH_MAP_TYPE) *existingEntry = &hashMap->entries[mapIndex];

    bool replacedValue = false;
    while (existingEntry->enteredIntoHashmap) {
        if (HASH_MAP_EQUALITY_FUNCTION(HASH_MAP_KEY_TYPE, HASH_MAP_TYPE)(key, existingEntry->key)) {
            existingEntry->value = value;
            replacedValue = true;
            break;
        }
        else {
            mapIndex = (mapIndex + 1) % hashMap->capacity;
            existingEntry = &hashMap->entries[mapIndex];
            ASSERT(mapIndex != originalMapIndex);
        }
    }
    if (!replacedValue) {
        HASH_MAP_ENTRY_STRUCT(HASH_MAP_KEY_TYPE, HASH_MAP_TYPE) entry = {};
        entry.key = key;
        entry.value = value;
        entry.enteredIntoHashmap = true;
        hashMap->entries[mapIndex] = entry;
    }
}

HASH_MAP_TYPE *getHashMapValuePointer (HASH_MAP_TYPE_STRUCT(HASH_MAP_KEY_TYPE, HASH_MAP_TYPE) *hashMap, HASH_MAP_KEY_TYPE key) {
    HASH_MAP_TYPE *result;

    unsigned int hash = HASH_MAP_HASH_FUNCTION(HASH_MAP_KEY_TYPE, HASH_MAP_TYPE)(key);
    unsigned int mapIndex = hash % hashMap->capacity;
    unsigned int originalMapIndex = mapIndex; // check that we actually find it after looping all the way around

    while (true) {
        HASH_MAP_ENTRY_STRUCT(HASH_MAP_KEY_TYPE, HASH_MAP_TYPE) *entry = &hashMap->entries[mapIndex];
        if (entry->enteredIntoHashmap) {
            if (HASH_MAP_EQUALITY_FUNCTION(HASH_MAP_KEY_TYPE, HASH_MAP_TYPE)(key, entry->key)) {
                result = &entry->value;
                break;
            }
            else {
                mapIndex = (mapIndex + 1) % hashMap->capacity;
                if (mapIndex == originalMapIndex) {
                    return 0;
                }
            }
        }
        else {
            return 0;
        }
    }

    return result;
}

HASH_MAP_TYPE getHashMapValue (HASH_MAP_TYPE_STRUCT(HASH_MAP_KEY_TYPE, HASH_MAP_TYPE) *hashMap, HASH_MAP_KEY_TYPE key) {
    HASH_MAP_TYPE *result = getHashMapValuePointer(hashMap, key);
    ASSERT(result != 0);
    return *result;
}

bool keyExistsInHashMap (HASH_MAP_TYPE_STRUCT(HASH_MAP_KEY_TYPE, HASH_MAP_TYPE) *hashMap, HASH_MAP_KEY_TYPE key) {
    HASH_MAP_TYPE *result = getHashMapValuePointer(hashMap, key);
    return result != 0;
}

bool getHashMapValueIfExists (HASH_MAP_TYPE_STRUCT(HASH_MAP_KEY_TYPE, HASH_MAP_TYPE) *hashMap, HASH_MAP_KEY_TYPE key, HASH_MAP_TYPE *outValue) {
    HASH_MAP_TYPE *result = getHashMapValuePointer(hashMap, key);
    if(result != 0) {
        *outValue = *result;
    };
    return result != 0;
}

void clearHashMap (HASH_MAP_TYPE_STRUCT(HASH_MAP_KEY_TYPE, HASH_MAP_TYPE) *hashMap) {
    for (unsigned int i = 0; i < hashMap->capacity; ++i) {
        hashMap->entries[i].enteredIntoHashmap = false;
    }
}

HASH_MAP_TYPE_STRUCT(HASH_MAP_KEY_TYPE, HASH_MAP_TYPE) HASH_MAP_INIT(HASH_MAP_KEY_TYPE, HASH_MAP_TYPE) (memory_arena *memory, int capacity) {
    HASH_MAP_TYPE_STRUCT(HASH_MAP_KEY_TYPE, HASH_MAP_TYPE) result = {};
    result.capacity = capacity;
    result.entries = (HASH_MAP_ENTRY_STRUCT(HASH_MAP_KEY_TYPE, HASH_MAP_TYPE) *)allocateSize(memory, capacity * sizeof(HASH_MAP_ENTRY_STRUCT(HASH_MAP_KEY_TYPE, HASH_MAP_TYPE)));
    for (int i = 0; i < capacity; ++i) {
        result.entries[i] = {};
    }
    return result;
}

#undef HASH_MAP_TYPE
#undef HASH_MAP_KEY_TYPE
