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
 *   date: 2018/09/28
 */

#ifndef __TEST_BASE_CONTAINER_H__
#define __TEST_BASE_CONTAINER_H__

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "test_base_container"

#include <stdio.h>
#include "rt_header.h"
#include "rt_kernel_list.h"
#include "rt_array_list.h"
#include "rt_linked_list.h"
#include "rt_dequeue.h"

void unit_test_link_list();
void unit_test_kernel_link_list();
void unit_test_kernel_hash_list();

void unit_test_link_list() {
   RtLinkedList* list = linked_list_create();

    for(int i = 0; i < 10; i++) {
        linked_list_add(list, (void*)(RT_NULL+i), RT_FALSE);
        //linked_list_remove_at(list, 0);
    }

    RtLinkedEntry* entry = list->head;
    if(RT_NULL != list->head) {
        while(entry != RT_NULL) {
            RT_LOGE("RtLinkedEntry = %p heat = %d", entry->data, entry->heat);
            entry = entry->next;
        }
    }

    for(int i = 0; i < linked_list_get_size(list); i++) {
        //RtLinkedEntry* entry = linked_list_get(list, i);
        //RT_LOGE("linked_list_get[%d] = %p heat = %d", i, entry->data, entry->heat);
    }
    linked_list_remove_all(list);
    linked_list_destroy(list);
}

typedef struct _mem_record{
    const char* caller;
    void* ptr;
    INT32 size;
    struct list_head list;
} mem_record;

void unit_test_kernel_link_list(){
    LIST_HEAD(head);

    mem_record *entry, *entry_tmp;
    struct list_head *pos, *tmp;

    for(int i = 0; i < 10; i++) {
        mem_record* record = (mem_record*)rt_mem_malloc(__FUNCTION__, sizeof(mem_record));
        record->caller = __FUNCTION__;
        record->size   = sizeof(mem_record)+i;
        record->ptr    = (void*)record;
        list_add_tail(&(record->list), &head);
    }

    list_for_each_safe(pos, tmp, &head) {
        entry = list_entry(pos, mem_record, list);
        RT_LOGE("caller:%s, pointer=%p; size=%d",
                entry->caller, entry->ptr, entry->size);
        // warning: list_del() can be only used with safe iterator.
        list_del(pos);
        rt_mem_free(__FUNCTION__, entry);
    }

    for(int i = 100; i < 110; i++) {
        mem_record* record = (mem_record*)rt_mem_malloc(__FUNCTION__, sizeof(mem_record));
        record->caller = __FUNCTION__;
        record->size   = sizeof(mem_record)+i;
        record->ptr    = (void*)record;
        list_add_tail(&(record->list), &head);
    }

    list_for_each_entry_safe(entry, entry_tmp, &head, list) {
        RT_LOGE("caller:%s, pointer=%p; size=%d",
                entry->caller, entry->ptr, entry->size);
        // warning: list_del() can be only used with safe iterator.
        list_del(&(entry->list));
        rt_mem_free(__FUNCTION__, entry);
    }
    RT_LOGE("unit_test_kernel_list() done!");
}

void unit_test_kernel_hash_list() {
}

#endif
