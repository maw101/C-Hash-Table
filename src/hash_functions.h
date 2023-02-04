#ifndef SRC_HASH_FUNCTIONS_H
#define SRC_HASH_FUNCTIONS_H

static int hash_table_hash(const char * key, const int prime, const int hash_table_size);
static int get_double_hashing_hash(const char * s, const int hash_table_size, const int attempt_number);

#endif //SRC_HASH_FUNCTIONS_H
