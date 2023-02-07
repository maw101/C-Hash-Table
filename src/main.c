#include <stdio.h>

#include "hash_table.h"

int main() {
    hash_table_table * hash_table = hash_table_new();
    printf("Created hash table. Maximum Capacity: %i\n", hash_table->size);
    
    // processing here...

    hash_table_insert(hash_table, "Key1", "Value1");
    printf("Inserted 'Key1' and 'Value1' into table. Size: %i, Maximum Capacity: %i.\n", hash_table->count, hash_table->size);
    hash_table_insert(hash_table, "Key2", "Value2");
    printf("Inserted 'Key2' and 'Value2' into table. Size: %i, Maximum Capacity: %i.\n", hash_table->count, hash_table->size);
    hash_table_insert(hash_table, "Key3", "Value3");
    printf("Inserted 'Key3' and 'Value3' into table. Size: %i, Maximum Capacity: %i.\n", hash_table->count, hash_table->size);
    hash_table_insert(hash_table, "Key4", "Value4");
    printf("Inserted 'Key4' and 'Value4' into table. Size: %i, Maximum Capacity: %i.\n", hash_table->count, hash_table->size);

    char * lookup = hash_table_search(hash_table, "Key1");
    printf("Search for 'Key1' returned value: %s\n", lookup);

    hash_table_delete_key(hash_table, "Key1");
    printf("Deletion method called for 'Key1'\n");

    lookup = hash_table_search(hash_table, "Key1");
    printf("Search for 'Key1' after deletion returned value: %s\n", lookup);

    lookup = hash_table_search(hash_table, "Fake Key");
    printf("Search for 'Fake Key' returned value: %s\n", lookup);

    printf("Hash Table Size: %i\n", hash_table->count);
    printf("Hash Table Maximum Capacity: %i\n", hash_table->size);

    hash_table_delete_table(hash_table);
}
