#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "hash_table.h"
#include "hash_functions.h"
#include "prime.h"

// define a deleted item to help prevent breaks in the collision chain
static hash_table_item HASH_TABLE_DELETED_ITEM = {NULL, NULL};
const int HASH_TABLE_DEFAULT_BASE_SIZE = 2; // TODO: set to larger value for normal use

// item initialisation function
static hash_table_item * hash_table_new_item(const char * k, const char * v) {
    hash_table_item * item = malloc(sizeof(hash_table_item));

    // check result of the call to malloc
    if (item != NULL) {
        item->key = strdup(k); // duplicate key
        item->value = strdup(v); // duplicate value
    } else {
        printf("%s \n", "Malloc failed when creating hash table item");
    }

    return item;
}

// item deletion function
static void hash_table_delete_item(hash_table_item * item) {
    free(item->key);
    free(item->value);
    free(item);
}

// table initialisation function
hash_table_table * hash_table_new() {
    return hash_table_new_given_size(HASH_TABLE_DEFAULT_BASE_SIZE);
}

// table initialisation function for a given base size
hash_table_table * hash_table_new_given_size(const int base_size) {
    hash_table_table * hash_table = malloc(sizeof(hash_table_table));

    // check result of the call to malloc
    if (hash_table != NULL) {
        hash_table->count = 0;
        hash_table->size = get_next_prime(base_size);

        hash_table->items = calloc((size_t) hash_table->size, sizeof(hash_table_item *));

        // check result of the items call to calloc
        if (hash_table->items == NULL) {
            printf("%s \n", "Calloc failed when creating hash table items - hash table size set to 0");
            hash_table->size = 0;
        }
    } else {
        printf("%s \n", "Malloc failed when creating hash table");
    }

    return hash_table;
}

// table insert function
void hash_table_insert(hash_table_table * hash_table, const char * key, const char * value) {
    // create pointer to a new item
    hash_table_item * item = hash_table_new_item(key, value);

    if (item != NULL) {
        // check load of the hash table and determine if we should resize upwards
        const int hash_table_load = hash_table->count * 100 / hash_table->size;
        if (hash_table_load > 70) {
            hash_table_resize_grow(hash_table);
        }

        // get the hash for the new item
        int index = get_double_hashing_hash(item->key, hash_table->size, 0);

        // determine if there is an item already at this hash
        hash_table_item * item_at_index = hash_table->items[index];
        int attempt_count = 1;

        // while we're not at an empty index keep generating a new index
        while (item_at_index != NULL) {
            // check item for a matching key
            if (strcmp(item_at_index->key, key) == 0) {
                // replace item with new value
                hash_table_delete_item(item_at_index);
                hash_table->items[index] = item;
                return;
            }

            // retrieve item for next attempt
            index = get_double_hashing_hash(item->key, hash_table->size, attempt_count);
            item_at_index = hash_table->items[index];
            attempt_count++;
        }

        // store the item in the free index
        hash_table->items[index] = item;
        hash_table->count++;
    } else {
        printf("%s \n", "Item could not be inserted as there was not enough memory to store it");
    }
}

// define table search function
char * hash_table_search(hash_table_table * hash_table, const char * key) {
    int index = get_double_hashing_hash(key, hash_table->size, 0);
    hash_table_item * item_at_index = hash_table->items[index];

    int attempt_count = 1;
    // perform linear search while we're not at an empty index
    while (item_at_index != NULL) {
        // check item for a matching key
        if ((item_at_index != &HASH_TABLE_DELETED_ITEM) && (strcmp(item_at_index->key, key) == 0)) {
            return item_at_index->value;
        }

        // retrieve item for next attempt
        index = get_double_hashing_hash(key, hash_table->size, attempt_count);
        item_at_index = hash_table->items[index];
        attempt_count++;
    }

    // key not present in the hash table
    return NULL;
}

// table deletion function for given key
void hash_table_delete_key(hash_table_table * hash_table, const char* key) {
    int index = get_double_hashing_hash(key, hash_table->size, 0);
    hash_table_item * item_at_index = hash_table->items[index];

    int attempt_count = 1;
    // perform linear search while we're not at an empty index or deleted item
    while ((item_at_index != NULL) && (item_at_index != &HASH_TABLE_DELETED_ITEM)) {
        // ensure we're not at a deleted item
        if (item_at_index != &HASH_TABLE_DELETED_ITEM) {
            // check item for a matching key
            if (strcmp(item_at_index->key, key) == 0) {
                hash_table_delete_item(item_at_index);

                // change item index to point to the deleted item
                // do not wish to break the collision chain so need to know item has been deleted
                hash_table->items[index] = &HASH_TABLE_DELETED_ITEM;

                hash_table->count--;
            }
        }

        // retrieve item for next attempt
        index = get_double_hashing_hash(key, hash_table->size, attempt_count);
        item_at_index = hash_table->items[index];
        attempt_count++;
    }

    // check load of the hash table and determine if we should resize downwards (shrink)
    const int hash_table_load = hash_table->count * 100 / hash_table->size;
    if (hash_table_load < 10) {
        hash_table_resize_shrink(hash_table);
    }
}

// table deletion function
void hash_table_delete_table(hash_table_table * hash_table) {
    // ensure hash table pointer is not null
    if (hash_table != NULL) {
        // check if we have any items to delete
        if (hash_table->count != 0) {
            // delete all items in the table
            for (int item_index = 0; item_index < hash_table->size; item_index++) {
                hash_table_item *item = hash_table->items[item_index];
                // determine if we must delete the item
                if ((item != NULL) && (item != &HASH_TABLE_DELETED_ITEM)) {
                    hash_table_delete_item(item);
                }
            }
        }

        // free memory
        free(hash_table->items);
        free(hash_table);
    }
}

// table resize function
static void hash_table_resize(hash_table_table * hash_table, const int base_size) {
    if ((base_size < HASH_TABLE_DEFAULT_BASE_SIZE) || (base_size < hash_table->count)) {
        return;
    }

    hash_table_table * new_hash_table = hash_table_new_given_size(base_size);

    // copy across all items
    for (int index = 0; index < hash_table->size; index++) {
        hash_table_item * current_item = hash_table->items[index];
        if ((current_item != NULL) && (current_item != &HASH_TABLE_DELETED_ITEM)) {
            hash_table_insert(new_hash_table, current_item->key, current_item->value);
        }
    }

    // swap hash_table and new_hash_table, so new table is stored within hash_table

    hash_table->count = new_hash_table->count;

    const int current_hash_table_size = hash_table->size;
    hash_table->size = new_hash_table->size;
    new_hash_table->size = current_hash_table_size;

    hash_table_item ** current_hash_table_items = hash_table->items;
    hash_table->items = new_hash_table->items;
    new_hash_table->items = current_hash_table_items;

    hash_table_delete_table(new_hash_table);
}

// table resize upwards (growth) function
static void hash_table_resize_grow(hash_table_table * hash_table) {
    const int new_size = hash_table->size * 2;
    hash_table_resize(hash_table, new_size);
}

// table resize downwards (shrink) function
static void hash_table_resize_shrink(hash_table_table * hash_table) {
    const int new_size = hash_table->size / 2;
    hash_table_resize(hash_table, new_size);
}
