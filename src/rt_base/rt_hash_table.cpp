#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rt_hash_table.h"
#include "rt_mem.h"

typedef struct rt_hash_node {
    void                *key;
    void                *data;
    struct rt_hash_node *prev;
    struct rt_hash_node *next;
}RtHashNode;

struct rt_hash {
    unsigned int        buckets;   // bucket count
    hash_func_t         hash_func; // hash function ptr
    RtHashNode        **nodes;
};

RtHashNode **rt_hash_get_buckets(RtHash *hash, void *key);
RtHashNode * rt_hash_get_node_by_key(RtHash *hash, void *key, unsigned int key_size);

RtHash *rt_hash_alloc(UINT32 buckets, hash_func_t hash_func)
{
    RtHash *hash = rt_malloc(RtHash);
    hash->buckets = buckets;
    hash->hash_func = hash_func;
    UINT32 size = buckets * sizeof(RtHashNode *);
    hash->nodes = rt_malloc_array(RtHashNode *, buckets);
    memset(hash->nodes, 0x00, size);
    return hash;
}

void* rt_hash_lookup(RtHash *hash, void *key, UINT32 key_size)
{
    RtHashNode *node = rt_hash_get_node_by_key(hash, key, key_size);
    if (RT_NULL == node)
        return RT_NULL;
    return node->data;
}

void rt_hash_add(RtHash *hash,
                      void *key,
                      UINT32 key_size,
                      void *data,
                      UINT32 data_size)
{
    /* First, we need to find out if this data item exists. 
     * If it exists, it indicates that it is duplicated and
     * needs to be returned.
     */
    if (rt_hash_lookup(hash, key, key_size)) {
        printf("duplicate hash key\n");
        return;
    }

    RtHashNode *node = (RtHashNode *)rt_malloc(RtHashNode);
    node->next = RT_NULL;
    node->prev = RT_NULL;
    node->key = rt_malloc_size(void, key_size);
    memcpy(node->key, key, key_size);
    node->data = rt_malloc_size(void, data_size);
    memcpy(node->data, data, data_size);

    /* Insert the node into the hash table by head
     * insertion method.
     * first get bucket
     */
    RtHashNode **bucket = rt_hash_get_buckets(hash, key);

    if (*bucket == RT_NULL) {
        *bucket = node;
    } else {
        node->next = *bucket;
        (*bucket)->prev = node;
        *bucket = node;
    }
}

void rt_hash_free(RtHash *hash, void *key, UINT32 key_size)
{
    RtHashNode *node = rt_hash_get_node_by_key(hash, key, key_size);
    if (node == RT_NULL) return;
    if (node->prev) {
        node->prev->next = node->next;
    }
    else{
        RtHashNode **bucket = rt_hash_get_buckets(hash,key);
        *bucket = node->next;
    }
    if (node->next) {
        node->next->prev = node->prev;
    }
    rt_free(node->key);
    rt_free(node->data);
    rt_free(node);
}

RtHashNode **rt_hash_get_buckets(RtHash *hash, void *key)
{
    UINT32 buckets = hash->hash_func(hash->buckets, key);
    if (buckets >= hash->buckets){
        printf("bad buckets loockup\n");
    }
    return &(hash->nodes[buckets]);
}

RtHashNode *rt_hash_get_node_by_key(RtHash *hash, void *key, UINT32 key_size) 
{
    RtHashNode **buckets = rt_hash_get_buckets(hash, key);
    RtHashNode *node = *buckets;
    if (node == RT_NULL)
        return RT_NULL;

    while (node != RT_NULL && memcmp(node->key, key, key_size) != 0) {
        node = node->next;
    }
    return node;
} 


