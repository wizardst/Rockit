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
 */

#include "rt_dequeue.h" // NOLINT
#include "rt_header.h" // NOLINT
#include "rt_mem.h" // NOLINT

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "rt_dequeue"

RT_Deque* deque_create() {
    RT_Deque* list = rt_malloc(RT_Deque);
    if (RT_NULL != list) {
        list->size = 0;
        list->head = RT_NULL;
        list->tail = RT_NULL;
    }
    list->entries   = RT_NULL;
    list->max_size = 0;
    return list;
}

RT_Deque* deque_create(UINT8 max_size) {
    RT_Deque* list  = deque_create();
    RT_ASSERT(RT_NULL != list);
    list->max_size = max_size;
    list->entries   = rt_malloc_array(RT_DequeEntry, max_size);
    RT_ASSERT(RT_NULL != list->entries);
    for (UINT32 idx = 0; idx < max_size; idx++) {
        RT_DequeEntry entry = list->entries[idx];
        rt_memset(&entry, RT_NULL, sizeof(RT_DequeEntry));
    }
    return list;
}

void deque_destory(RT_Deque **list) {
    RT_ASSERT(RT_NULL != *list);

    if ((*list)->entries) {
        rt_free((*list)->entries);
        (*list)->entries = NULL;
    } else {
        RT_DequeEntry* entry = (*list)->head;
        RT_DequeEntry* next;
        while (entry) {
            next = entry->next;
            rt_free(entry);
            entry = next;
        }
    }
    rt_memset(*list, 0, sizeof(RT_Deque));

    rt_free(*list);
    *list = RT_NULL;
}

RT_DequeEntry* deque_entry_malloc(RT_Deque *list) {
    RT_ASSERT(RT_NULL != list);
    RT_DequeEntry *entry = RT_NULL;
    if (RT_NULL != list->entries) {
        if (list->size < list->max_size) {
            for (UINT32 idx = 0; idx < list->max_size; idx++) {
                entry = &(list->entries[idx]);
                // found entry in unused pre-malloc entries
                if ((RT_NULL == entry->data)
                        && (ENTRY_FLAG_UNUSE == entry->flag)) {
                    break;
                }
            }
        } else {
            entry = RT_NULL;
        }
    } else {
        entry = rt_malloc(RT_DequeEntry);
    }
    return entry;
}

RT_DequeEntry deque_pop(RT_Deque *list) {
    RT_ASSERT(RT_NULL != list);
    RT_DequeEntry entry;
    rt_memset(&entry, 0, sizeof(RT_DequeEntry));
    if (deque_size(list) > 0) {
        rt_memcpy(&entry, list->head, sizeof(RT_DequeEntry));
        if (!list->entries) {
            rt_free(list->head);
        } else {
            rt_memset(list->head, 0, sizeof(RT_DequeEntry));
        }
        list->head = entry.next;
        list->size = (list->size > 0) ? (list->size - 1) : 0;
    }
    return entry;
}

RT_RET deque_push(RT_Deque *list,
                     const void *data,
                     RT_BOOL header /* =RT_FALSE */) {
    RT_ASSERT(RT_NULL != list);

    RT_DequeEntry *entry = deque_entry_malloc(list);
    if (RT_NULL == entry)
       return RT_ERR_BAD;

    entry->data = const_cast<void *> (data);
    entry->flag = ENTRY_FLAG_USE;
    entry->next = RT_NULL;
    if (deque_size(list) == 0) {
        // insert header, when list is RT_NULL
        entry->prev = RT_NULL;
        list->head = entry;
        list->tail = entry;
    } else {
        if (RT_TRUE == header) {
            RT_DequeEntry* head = list->head;
            head->prev         = entry;
            entry->next        = head;
            list->head         = entry;
        } else {
            RT_DequeEntry* tail = list->tail;
            tail->next         = entry;
            entry->prev        = tail;
            list->tail         = entry;
        }
    }
    list->size++;
    return RT_OK;
}

RT_RET deque_insert(RT_Deque *list, RT_DequeEntry* entry, const void *data) {
    RT_ASSERT(RT_NULL != list);

    RT_DequeEntry *new_entry = deque_entry_malloc(list);
    if (RT_NULL == new_entry)
       return RT_ERR_BAD;

    new_entry->data = const_cast<void *> (data);
    new_entry->flag = ENTRY_FLAG_USE;
    new_entry->next = entry->next;
    new_entry->prev = entry;
    list->size++;

    return RT_OK;
}

RT_RET deque_push_tail(RT_Deque *list, const void *data) {
    RT_BOOL header =  RT_FALSE;
    return deque_push(list, data, header);
}

RT_RET deque_push_head(RT_Deque *list, const void *data) {
    RT_BOOL header =  RT_TRUE;
    return deque_push(list, data, header);
}

void*  deque_get(RT_Deque *list, int index) {
    RT_DequeEntry* entry = list->head;
    while (RT_NULL != entry) {
        if (index == 0) break;
        index--;
        entry = entry->next;
    }
    if (RT_NULL == entry) {
        return RT_NULL;
    }
    return entry->data;
}
