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
 * author: martin.cheng@rock-chips.com
 *   date: 20180903
 */

#ifndef SRC_RT_BASE_INCLUDE_RT_HASH_TABLE_H_
#define SRC_RT_BASE_INCLUDE_RT_HASH_TABLE_H_

#include "rt_header.h" // NOLINT

struct RtHashTable;

struct rt_hash_node {
    struct rt_hash_node *next;
    // struct rt_hash_node *prev;
    const void  *key;
    void        *data;
};

// ! Callbacks for RT_HASH_TABLE
typedef UINT32 (*rt_hash_func)(UINT32 num_buckets, const void *key);
typedef UINT32 (*rt_hash_comp_func)(const void *key1, const void *key2);

// ! Hash Function for pointer
UINT32 hash_ptr_func(UINT32 bucktes, const void *key);
UINT32 hash_ptr_compare(const void *key1, const void *key2);
// ! Hash Function for String
UINT32 hash_string_func(UINT32 bucktes, const void *key);
UINT32 hash_string_compare(const void *key1, const void *key2);

// ! Operations for RT_HASH_TABLE
struct RtHashTable *rt_hash_table_create(UINT32 num_buckets,
                                               rt_hash_func hash,
                                               rt_hash_comp_func compare);
void   rt_hash_table_destory(struct RtHashTable *hash, RT_BOOL free_data = RT_FALSE);
void   rt_hash_table_dump(struct RtHashTable *hash);
void   rt_hash_table_clear(struct RtHashTable *hash, RT_BOOL free_data = RT_FALSE);
void  *rt_hash_table_find(struct RtHashTable *hash,
                                 const void *key);
void   rt_hash_table_insert(struct RtHashTable *hash,
                                    const void *key,
                                    void *data);
RT_BOOL   rt_hash_table_replace(struct RtHashTable *hash,
                                      const void *key, void *data);
RT_BOOL   rt_hash_table_remove(struct RtHashTable *hash,
                                    const void *key,
                                    RT_BOOL free_data = RT_FALSE);
UINT32 rt_hash_table_get_num_buckets(struct RtHashTable *hash);
struct rt_hash_node* rt_hash_table_get_bucket(struct RtHashTable *hash, UINT32 idx);
UINT32 rt_hash_table_string_hash(const void *key);

#endif  // SRC_RT_BASE_INCLUDE_RT_HASH_TABLE_H_
