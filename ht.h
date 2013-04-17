/* ===========================================================================
 * HT COMM
 *
 * Author : Barbarossa
 * Description : Hash table interface
 *		It strives to be generic.
 *
 * ===========================================================================
 */
#ifndef _HASH_TABLE_H_ 
#define _HASH_TABLE_H_

#include <stdio.h>

typedef unsigned char bool_t;
#define 	TRUE	(unsigned char)0xFF
#define		FALSE	(unsigned char)0x00

/* ------------------------------------------------------------------------ */

/*	Hash table util structs		*/

typedef
struct hashEntry {
    void* key;
    void* data;
    struct hashEntry* next;
} hashEntry_t;

typedef
struct hashBucket {
    int nrEntries;
    hashEntry_t* first;
} hashBucket_t;

typedef
struct hashTable {
    /*	Size and bucket list */
    unsigned int size;
    hashBucket_t* buckets;

    /* Hash func */
    unsigned int (*hash_fn)(void*, unsigned int);

    /* Key comparation func */
    int (*key_cmp_fn)(void* key1, void* key2);

    /* ToString func to be used during printing */
    char* (*key_to_string_fn)(void* key);
    char* (*data_to_string_fn)(void* data);
} hashTable_t;

typedef
struct hashTableStats {
    unsigned int totalNrElems;
    unsigned int maxNrBucket;
    unsigned int minNrBucket;
    unsigned int dispersion;
} hashTableStats_t;

#define FIRST_BUCKET_ENTRY(ht,bi) ((ht)->buckets[(bi)].first)
#define KEYS_EQUAL(ht,k1,k2) ((ht)->key_cmp_fn((k1),(k2)) == 0)

/* ------------------------------------------------------------------------ */
/*	Hash table util funcs		*/

/*	Alloc space for new hash table + attach functions	*/
hashTable_t* ht_new(unsigned int size,
        unsigned int (*hash_fn)(void*, unsigned int),
        int (*key_cmp_fn)(void*, void*),
        char* (*key_to_string_fn)(void*),
        char* (*data_to_string_fn)(void*));

/*	Dealloc hash table; leaves the data pointers intact	*/
void ht_delete(hashTable_t* ht);

/*	Remove all elements from hash table	*/
void ht_clear(hashTable_t* ht);

/*	Resize the hash table	*/
void ht_resize(hashTable_t* ht, unsigned int newSize);

/*	Add an element to the hash table, return its bucket index	*/
unsigned int ht_entry_add(hashTable_t* ht, void* key, void* data);

/*	Remove an element from the hash table, return data pointer */
void* ht_entry_remove(hashTable_t* ht, void* key);

/*	Find an element in the hash table, return true/false + write pointer to data */
bool_t ht_entry_find(hashTable_t* ht, void* key, void** pData);


/*	Print functions */
void ht_print(hashTable_t* ht, FILE* file);
void ht_print_bucket(hashTable_t* ht,
        unsigned int bucket_index,
        bool_t just_keys,
        FILE* file);



#endif /* _HASH_TABLE_H_ */
