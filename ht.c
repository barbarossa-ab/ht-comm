/* ===========================================================================
 * HT COMM
 *
 * Author : Barbarossa
 * Description : Hash table implementation
 *		It strives to be generic.
 *
 * ===========================================================================
 */

#include "ht.h"
#include <assert.h>
#include <stdlib.h>

/*	Alloc / Dealloc funcs for hash entries	*/
static hashEntry_t* ht_entry_new(void* key, void* data) {
    hashEntry_t* ht_entry = malloc(sizeof (hashEntry_t));
    assert(ht_entry != NULL);

    ht_entry->key = key;
    ht_entry->data = data;
    ht_entry->next = NULL;

    return ht_entry;
}

static void ht_entry_delete(hashEntry_t* ht_entry) {
    free(ht_entry);
}

/*	Alloc space for new hash table + attach functions	*/
hashTable_t* ht_new(unsigned int size,
        unsigned int (*hash_fn)(void*, unsigned int),
        int (*key_cmp_fn)(void*, void*),
        char* (*key_to_string_fn)(void*),
        char* (*data_to_string_fn)(void*)) {
    hashTable_t* ht = malloc(sizeof (hashTable_t));
    assert(ht != NULL);

    assert(hash_fn != NULL);
    assert(key_cmp_fn != NULL);
    assert(key_to_string_fn != NULL);

    ht->hash_fn = hash_fn;
    ht->key_cmp_fn = key_cmp_fn;
    ht->key_to_string_fn = key_to_string_fn;
    ht->data_to_string_fn = data_to_string_fn;

    ht->buckets = calloc(size, sizeof (hashBucket_t));
    assert(ht->buckets != NULL);
    ht->size = size;

    return ht;
}

/*	Dealloc hash table; leaves the data pointers intact	*/
void ht_delete(hashTable_t* ht) {
    ht_clear(ht);
    free(ht->buckets);
    free(ht);
}

/*	Remove all elements from hash table	*/
void ht_clear(hashTable_t* ht) {
    // Remove all elements from buckets
    int i = 0;
    while (i < ht->size) {
        hashEntry_t* iter = ht->buckets[i].first;
        while (iter != NULL) {
            hashEntry_t* c_entry = iter;
            iter = iter->next;
            ht_entry_delete(c_entry);
        }
        ht->buckets[i].first = NULL; //might seem redundant, but isn't
        i++;
    }
}

/*	Resize the hash table	*/
void ht_resize(hashTable_t* ht, unsigned int newSize) {
    unsigned int oldSize = ht->size;
    hashBucket_t* old_buckets = ht->buckets;

    //	Redefine hash
    ht->buckets = calloc(newSize, sizeof (hashBucket_t));
    assert(ht->buckets != NULL);
    ht->size = newSize;

    int i = 0;
    while (i < oldSize) {
        hashEntry_t* iter = old_buckets[i].first;
        while (iter != NULL) {
            ht_entry_add(ht, iter->key, iter->data);
            iter = iter -> next;
        }
        i++;
    }

    //	Delete old bucket list
    free(old_buckets);
}

/*	Add an element to the hash table, return its bucket index	*/
unsigned int ht_entry_add(hashTable_t* ht, void* key, void* data) {
    assert(ht != NULL);
    assert(key != NULL);

    unsigned int bIndex = ht->hash_fn(key, ht->size);

    assert(bIndex < ht->size);

    // if bucket empty, define first element
    if (ht->buckets[bIndex].first == NULL) {
        //		dbg_printf(" bucket empty, element added");
        ht->buckets[bIndex].first = ht_entry_new(key, data);
        return bIndex;
    }

    //	see if elem already exists
    //	if not, add it to the bucket
    hashEntry_t* iter = ht->buckets[bIndex].first;
    while (iter != NULL) {
        if (KEYS_EQUAL(ht, iter->key, key)) {
            return -1;
        }
        if (iter->next != NULL)
            iter = iter->next;
        else
            break;
    }
    iter->next = ht_entry_new(key, data);

    return bIndex;
}

/*	Remove an element from the hash table, return data pointer */
void* ht_entry_remove(hashTable_t* ht, void* key) {
    assert(ht != NULL);
    assert(key != NULL);

    hashEntry_t* defunct;
    void* data;

    unsigned int bIndex = ht->hash_fn(key, ht->size);

    // if bucket empty, return
    if (ht->buckets[bIndex].first == NULL)
        return NULL;

    // if first elem in bucket
    if (KEYS_EQUAL(ht, key, ht->buckets[bIndex].first->key)) {
        data = ht->buckets[bIndex].first -> data;
        defunct = ht->buckets[bIndex].first;
        ht->buckets[bIndex].first = ht->buckets[bIndex].first -> next;
        free(defunct);
        return data;
    }

    //	if other elem in bucket
    hashEntry_t* iter = ht->buckets[bIndex].first;
    while (iter->next != NULL) {
        if (KEYS_EQUAL(ht, iter->next -> key, key)) {
            data = iter->next -> data;
            defunct = iter->next;
            iter->next = iter->next->next;
            free(defunct);
            return data;
        }
        iter = iter->next;
    }

    return NULL;
}

/*	Find an element in the hash table, return data pointer	*/
bool_t ht_entry_find(hashTable_t* ht, void* key, void** pData) {
    assert(ht != NULL);
    assert(key != NULL);

    unsigned int bIndex = ht->hash_fn(key, ht->size);

    hashEntry_t* iter = ht->buckets[bIndex].first;
    while (iter != NULL) {
        if (KEYS_EQUAL(ht, iter->key, key)) {
            if (pData != NULL)
                *pData = iter->data;
            return TRUE;
        }
        iter = iter->next;
    }

    if (pData != NULL)
        *pData = NULL;
    return FALSE;
}

/*	Print functions */
void ht_print(hashTable_t* ht, FILE* file) {
    assert(ht != NULL);
    assert(file != NULL);

    unsigned int bIndex = 0;
    fprintf(file,
            "\n------------------------------------------------------------------------" \
		"\nHASH TABLE :\n");

    while (bIndex < ht->size)
        ht_print_bucket(ht, bIndex++, TRUE, file);

}

void ht_print_bucket(hashTable_t* ht, unsigned int bIndex,
        bool_t just_keys, FILE* file) {
    assert(ht != NULL);

    hashEntry_t* iter = ht->buckets[bIndex].first;

    fprintf(file, "%2d : [", bIndex);
    while (iter != NULL) {
        fprintf(file, " %s", ht->key_to_string_fn(iter->key));
        if (!just_keys)
            fprintf(file, " = %s", ht->data_to_string_fn(iter->data));
        iter = iter->next;

        if ((iter != NULL) && (!just_keys))
            fprintf(file, ",");
    }
    fprintf(file, "]\n");
}



