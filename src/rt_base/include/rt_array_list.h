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

#ifndef SRC_RT_BASE_INCLUDE_RT_ARRAY_LIST_H_
#define SRC_RT_BASE_INCLUDE_RT_ARRAY_LIST_H_

#include "rt_header.h" // NOLINT

typedef struct rt_array_list_entry {
    void        *data;
    UINT8        heat;
} RtArrayListEntry;

typedef struct rt_array_list {
    UINT32      size;
    UINT32      capacity;
    UINT32      min_capacity;
    RtArrayListEntry *entries;
} RtArrayList;

RtArrayList*        array_list_create();
RtArrayList*        array_list_create_with_capacity(UINT32 min_capacity);
INT8                array_list_add(RtArrayList* self, void* element);
INT8                array_list_insert_at(RtArrayList* self,
                                                 UINT32 index,
                                                 void* element);
INT8                array_list_remove(RtArrayList* self, void* element);
INT8                array_list_remove_at(RtArrayList* self, UINT32 index);
INT8                array_list_remove_all(RtArrayList* self);
RT_BOOL             array_list_contains(RtArrayList* self, void* element);
UINT32              array_list_get_size(RtArrayList* self);
void*               array_list_get_data(RtArrayList* self, size_t index);
RtArrayListEntry*   array_list_get_entry(RtArrayList* self, size_t index);
INT8                array_list_set(RtArrayList* self,
                                         size_t index,
                                         void* element);
INT8                array_list_destroy(RtArrayList *self);

#endif  // SRC_RT_BASE_INCLUDE_RT_ARRAY_LIST_H_
