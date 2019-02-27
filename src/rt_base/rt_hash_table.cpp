/*
 * Copyright 2018 Rockchip Electronics Co. LTD
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * author: rimon.xu@rock-chips.com
 *   date: 20181102
 *
 * author: martin.cheng@rock-chips.com
 *   date: 20181107
 * update: replaced by universal hash table api.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rt_hash_table.h" // NOLINT
#include "rt_mem.h" // NOLINT

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "rt_hash_table"

struct RtHashTable {
    rt_hash_func      hash;
    rt_hash_comp_func compare;

    unsigned num_buckets;
    struct rt_hash_node buckets[1];
};

struct RtHashTable *rt_hash_table_create(UINT32 num_buckets,
                                rt_hash_func hash, rt_hash_comp_func compare) {
    struct RtHashTable *ht;
    unsigned i;

    if (num_buckets < 16) {
        num_buckets = 16;
    }

    ht = rt_malloc_size(struct RtHashTable, sizeof(*ht) + ((num_buckets - 1)
                                  * sizeof(ht->buckets[0])));
    if (ht != NULL) {
        ht->hash        = hash;
        ht->compare     = compare;
        ht->num_buckets = num_buckets;

        for (i = 0; i < num_buckets; i++) {
            do {
                struct rt_hash_node *node = & ht->buckets[i];
                node->next = RT_NULL;
            }while(0);
        }
    }

    return ht;
}

void rt_hash_table_destory(struct RtHashTable *ht, RT_BOOL free_data) {
    rt_hash_table_clear(ht, free_data);
    rt_safe_free(ht);
}

#define foreach_list(ptr, list)     \
        for (ptr = (list)->next; (ptr != RT_NULL); ptr = (ptr)->next)

#define is_empty_list(list)  ((list)->next == (RT_NULL))

void insert_at_head(rt_hash_node* list, rt_hash_node* elem) {
    do {
        elem->next = list->next;
        list->next = elem;
    } while (0);
}

void remove_at_head(rt_hash_node* list, rt_hash_node* elem) {
    do {
        list->next = elem->next;
    } while (0);
}

void rt_hash_table_clear(struct RtHashTable *ht, RT_BOOL free_data) {
    struct rt_hash_node *list, *node, *next;

    for (UINT32 bucket = 0; bucket < rt_hash_table_get_num_buckets(ht); bucket++) {
        list = rt_hash_table_get_bucket(ht, bucket);
        for (node = list->next; node != RT_NULL; node = next) {
            next = node->next;
            remove_at_head(list, node);
            if (free_data && (NULL != node->data)) {
                rt_free(node->data);
            }
            rt_free(node);
        }

        RT_ASSERT(is_empty_list(& ht->buckets[bucket]));
    }
}

void rt_hash_table_dump(struct RtHashTable *ht) {
    RT_LOGE("hash=%p; buckets=%02d", ht, ht->num_buckets);
    UINT32 bucket = 0;
    UINT32 idx    = 0;
    struct rt_hash_node *list, *node;

    for (bucket = 0; bucket < ht->num_buckets; bucket++) {
        idx    = 0;
        list = &(ht->buckets[bucket]);
        for (node = list->next; node != RT_NULL; node = node->next) {
            RT_LOGE("buckets[%02d:%02d]: %p, node->key:%p, node->data:%p",
                         bucket, idx++, node, node->key, node->data);
        }
    }
}

static struct rt_hash_node * get_node(struct RtHashTable *ht, const void *key) {
    const UINT32 hash_value = (*ht->hash)(ht->num_buckets, key);
    const UINT32 bucket     = hash_value % ht->num_buckets;
    struct rt_hash_node *list, *node;

    list = &(ht->buckets[bucket]);
    for (node = list->next; node != RT_NULL; node = node->next) {
       if ((*ht->compare)(node->key, key) == 0) {
           return node;
       }
    }

    return NULL;
}

void *rt_hash_table_find(struct RtHashTable *ht, const void *key) {
    struct rt_hash_node *hn = get_node(ht, key);

    return (hn == NULL) ? NULL : hn->data;
}

void rt_hash_table_insert(
        struct RtHashTable *ht,
        const void *key,
        void *data) {
    const UINT32 hash_value = (*ht->hash)(ht->num_buckets, key);
    const UINT32 bucket     = hash_value % ht->num_buckets;

    struct rt_hash_node *node;
    node = rt_malloc(struct rt_hash_node);
    node->data = data;
    node->key = key;

    insert_at_head(& ht->buckets[bucket], node);
}

RT_BOOL rt_hash_table_replace(
        struct RtHashTable *ht,
        const void *key,
        void *data) {
    struct rt_hash_node *node = get_node(ht, key);
    if (RT_NULL != node) {
        node->data = data;
        return RT_TRUE;
    } else {
        RT_LOGE("Fail to find_node with key(%d)，so insert", key);
        rt_hash_table_insert(ht, key, data);
    }
    return RT_TRUE;
}

RT_BOOL rt_hash_table_remove(struct RtHashTable *ht, const void *key, RT_BOOL free_data) {
    const UINT32 hash_value   = (*ht->hash)(ht->num_buckets, key);
    const UINT32 bucket       = hash_value % ht->num_buckets;
    struct rt_hash_node* list = &(ht->buckets[bucket]);
    struct rt_hash_node* node_cur = RT_NULL;
    struct rt_hash_node* node_old = list;

    for (node_cur = list->next; (node_cur != RT_NULL); node_cur = node_cur->next) {
       if ((*ht->compare)(node_cur->key, key) == 0) {
           node_old->next = node_cur->next;
           node_cur->next = RT_NULL;
           if (free_data && NULL != node_cur->data) {
               rt_free(node_cur->data);
           }
           rt_free(node_cur);
           return RT_TRUE;
       } else {
           node_old = node_cur;
       }
    }
    RT_LOGE("don't find node from the key!");
    return RT_FALSE;
}

UINT32 rt_hash_table_get_num_buckets(struct RtHashTable *hash) {
    return hash->num_buckets;
}

struct rt_hash_node* rt_hash_table_get_bucket(struct RtHashTable *hash, UINT32 idx) {
    if (idx < hash->num_buckets) {
        return & hash->buckets[idx];
    }
    return RT_NULL;
}

UINT32 hash_string_func(const void *key) {
    const char *str = (const char *) key;
    UINT32 hash = 5381;

    while (*str != '\0') {
        hash = (hash * 33) + *str;
        str++;
    }

    return hash;
}

UINT32 hash_ptr_func(UINT32 bucktes, const void *key) {
    // return *((UINT32 *)(key)) % bucktes;
    // return (UINT32)((uintptr_t) key / sizeof(void *));
    UINT32 iKey = reinterpret_cast<uintptr_t>(key);
    return (iKey % bucktes);
}

UINT32 hash_ptr_compare(const void *key1, const void *key2) {
    return key1 == key2 ? 0 : 1;
}
