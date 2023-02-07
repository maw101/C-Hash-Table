#include <math.h>
#include <string.h>

#include "hash_functions.h"

// choose primes larger than our alphabet size - ie >128
int PRIME_1 = 131;
int PRIME_2 = 137;

// hashing function
static int hash_table_hash(const char * key, const int prime, const int hash_table_size) {
    long hash_value = 0;
    const int key_length = strlen(key);

    for (int i = 0; i < key_length; i++) {
        hash_value += (long) pow(prime, key_length - (i + 1)) * key[i];
        hash_value = hash_value % hash_table_size;
    }

    return (int) hash_value;
}

// double hashing function
int get_double_hashing_hash(const char * s, const int hash_table_size, const int attempt_number) {
    const int hash_a = hash_table_hash(s, PRIME_1, hash_table_size);
    const int hash_b = hash_table_hash(s, PRIME_2, hash_table_size);

    // mitigate risk of hashing to same bucket by adding 1 to hash_b
    return (hash_a + (attempt_number * (hash_b + 1))) % hash_table_size;
}
