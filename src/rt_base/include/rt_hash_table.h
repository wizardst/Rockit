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

#ifndef _RT_HASH_TABLE_H
#define _RT_HASH_TABLE_H

#include "rt_header.h"

struct RtHashTable;

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
struct RtHashTable *rt_hash_table_init(UINT32 num_buckets, rt_hash_func hash, rt_hash_comp_func compare);
void   rt_hash_table_destory(struct RtHashTable *hash);
void   rt_hash_table_dump   (struct RtHashTable *hash);
void   rt_hash_table_clear  (struct RtHashTable *hash);
void  *rt_hash_table_find   (struct RtHashTable *hash, const void *key);
void   rt_hash_table_insert (struct RtHashTable *hash, const void *key, void *data);
bool   rt_hash_table_replace(struct RtHashTable *hash, const void *key, void *data);
void   rt_hash_table_remove (struct RtHashTable *hash, const void *key);
UINT32 rt_hash_table_string_hash(const void *key);

#endif
