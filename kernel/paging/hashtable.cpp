#include <stdio.h>
#include <string.h>
#include "hashtable.hpp"
#define TABLE_SIZE 100

// Structure for the key-value pair

// Hash function
int hash(void* key) {
    size_t hashValue = *(size_t*)key;
    return (int)(hashValue % TABLE_SIZE);
}

// Function to insert a key-value pair into the hash table
void insert(HashTable* ht, size_t key, int value) {
    int index = hash(&key);
    while (ht->table[index].value != -1) {
        index = (index + 1) % TABLE_SIZE; // Linear probing for collision resolution
    }
    ht->table[index].key = key;
    ht->table[index].value = value;
}

// Function to search for a key in the hash table
int search(HashTable* ht, size_t key) {
    int index = hash(&key);
    int originalIndex = index;
    while (ht->table[index].key != key || ht->table[index].value == -1) {
        index = (index + 1) % TABLE_SIZE; // Linear probing
        if (index == originalIndex) {
            return -1; // Key not found
        }
    }
    return ht->table[index].value;
}

// Function to delete a key from the hash table
void deleteKey(HashTable* ht, size_t key) {
    int index = hash(&key);
    int originalIndex = index;
    while (ht->table[index].key != key || ht->table[index].value == -1) {
        index = (index + 1) % TABLE_SIZE; // Linear probing
        if (index == originalIndex) {
            return; // Key not found
        }
    }
    ht->table[index].value = -1; // Marking as deleted
}

