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

#ifndef SRC_RT_BASE_INCLUDE_RT_LINKED_LIST_H_
#define SRC_RT_BASE_INCLUDE_RT_LINKED_LIST_H_

#include "rt_header.h" // NOLINT

typedef struct rt_linked_entry {
    void                   *data;
    struct rt_linked_entry *next;
    struct rt_linked_entry *prev;
    UINT16                  heat;
} RtLinkedEntry;

typedef struct rt_linked_list {
    UINT32                  size;
    RtLinkedEntry          *head;
    RtLinkedEntry          *tail;
} RtLinkedList;

RtLinkedList*   linked_list_create();
INT8            linked_list_add(RtLinkedList* self,
                                      void* entry,
                                      RT_BOOL header = RT_FALSE);
INT8            linked_list_insert_at(RtLinkedList* self,
                                               UINT32 index,
                                               void* data);
INT8            linked_list_remove(RtLinkedList* self, void* data);
INT8            linked_list_remove_at(RtLinkedList* self, UINT32 index);
INT8            linked_list_remove_all(RtLinkedList* self);
RT_BOOL         linked_list_contains(RtLinkedList* self, void* data);
UINT32          linked_list_get_size(RtLinkedList* self);
RtLinkedEntry*  linked_list_get_entry(RtLinkedList* self, size_t index);
void*           linked_list_get_data(RtLinkedList* self, size_t index);
INT8            linked_list_set(RtLinkedList* self, size_t index, void* data);
INT8            linked_list_destroy(RtLinkedList **self);

#endif  // SRC_RT_BASE_INCLUDE_RT_LINKED_LIST_H_
