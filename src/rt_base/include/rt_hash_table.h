/*
 * Copyright 2018 The Rockit Open Source Project
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
 * author: martin.cheng
 *   date: 20180903
 */

#ifndef _RT_HASH_TABLE_H
#define _RT_HASH_TABLE_H

#include "rt_header.h"

typedef struct rt_hash RtHash;
typedef unsigned int(*hash_func_t)(unsigned int,void *);

RtHash *    rt_hash_alloc(UINT32 buckets, hash_func_t hash_func);
void  *     rt_hash_lookup(RtHash *hash, void *key, UINT32 key_size);
void        rt_hash_add(RtHash *hash,
                             void *key,
                             UINT32 key_size,
                             void *data,
                             UINT32 data_size);
void        rt_hash_free(RtHash *hash, void *key,UINT32 key_size);

#endif
