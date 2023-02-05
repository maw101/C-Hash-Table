#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "hash_table.h"

// define a deleted item to help prevent breaks in the collision chain
static hash_table_item HASH_TABLE_DELETED_ITEM = {NULL, NULL};

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
    hash_table_table * hash_table = malloc(sizeof(hash_table_table));

    // check result of the call to malloc
    if (hash_table != NULL) {
        hash_table->count = 0;
        hash_table->items = calloc((size_t) hash_table->size, sizeof(hash_table_item *));

        // check result of the items call to calloc
        if (hash_table->items != NULL) {
            hash_table->size = 50; // TODO: add resize ability in future
        } else {
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
        // get the hash for the new item
        int index = hash_table_dh_get_hash(item->key, hash_table->size, 0);

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
            index = hash_table_dh_get_hash(item->key, hash_table->size, attempt_count);
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
    int index = hash_table_dh_get_hash(key, hash_table->size, 0);
    hash_table_item * item_at_index = hash_table->items[index];

    int attempt_count = 1;
    // perform linear search while we're not at an empty index
    while (item_at_index != NULL) {
        // check item for a matching key
        if ((strcmp(item_at_index->key, key) == 0) && (item_at_index != &HASH_TABLE_DELETED_ITEM)) {
            return item_at_index->value;
        }

        // retrieve item for next attempt
        index = hash_table_dh_get_hash(key, hash_table->size, attempt_count);
        item_at_index = hash_table->items[index];
        attempt_count++;
    }

    // key not present in the hash table
    return NULL;
}

// table deletion function for given key
void hash_table_delete_key(hash_table_table * hash_table, const char* key) {
    int index = hash_table_dh_get_hash(key, hash_table->size, 0);
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
        index = hash_table_dh_get_hash(key, hash_table->size, attempt_count);
        item_at_index = hash_table->items[index];
        attempt_count++;
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
                if (item != NULL) {
                    hash_table_delete_item(item);
                }
            }
        }

        // free memory
        free(hash_table->items);
        free(hash_table);
    }
}
