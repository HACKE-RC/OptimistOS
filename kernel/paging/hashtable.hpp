#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stddef.h>

#define TABLE_SIZE 100

// Structure for the key-value pair
typedef struct {
    size_t key;
    int value;
} KeyValuePair;

// Structure for the hash table
typedef struct {
    KeyValuePair table[TABLE_SIZE];
} HashTable;

// Hash function
int hash(void* key);

// Function to insert a key-value pair into the hash table
void insert(HashTable* ht, size_t key, int value);

// Function to search for a key in the hash table
int search(HashTable* ht, size_t key);

// Function to delete a key from the hash table
void deleteKey(HashTable* ht, size_t key);

#endif /* HASHTABLE_H */

