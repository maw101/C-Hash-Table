#ifndef SRC_HASH_TABLE_H
#define SRC_HASH_TABLE_H

// item struct for key-value pairs
typedef struct {
    char * key;
    char * value;
} hash_table_item;

// hash table struct
typedef struct {
    int size;
    int count;
    hash_table_item ** items;
} hash_table_table;

// function prototypes
static hash_table_item * hash_table_new_item(const char * k, const char * v);
static void hash_table_delete_item(hash_table_item * item);

hash_table_table * hash_table_new();
void hash_table_insert(hash_table_table * hash_table, const char * key, const char * value);
char * hash_table_search(hash_table_table * hash_table, const char * key);
void hash_table_delete_table(hash_table_table * hash_table);

#endif //SRC_HASH_TABLE_H
