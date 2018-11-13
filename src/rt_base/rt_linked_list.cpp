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

#include "rt_linked_list.h" // NOLINT
#include "rt_error.h" // NOLINT
#include "rt_mem.h" // NOLINT

RtLinkedList* linked_list_create() {
    RtLinkedList* alist = rt_malloc(RtLinkedList);
    RT_ASSERT(RT_NULL != alist);

    alist->head = alist->tail = RT_NULL;
    alist->size = 0;
    return alist;
}

INT8 linked_list_add(RtLinkedList* self,
                           void* entry,
                           RT_BOOL header /* =RT_FALSE */) {
    RT_ASSERT(RT_NULL != self);

    RtLinkedEntry* new_entry = rt_malloc(RtLinkedEntry);
    RT_ASSERT(RT_NULL != new_entry);
    rt_memset(new_entry, 0, sizeof(RtLinkedEntry));
    new_entry->data = entry;

    if (self->size == 0) {
        self->head = self->tail = new_entry;
    } else {
        if (RT_TRUE == header) {
            RtLinkedEntry* head = self->head;
            head->prev         = new_entry;
            new_entry->next    = head;
            self->head         = new_entry;
        } else {
            RtLinkedEntry* tail = self->tail;
            tail->next         = new_entry;
            new_entry->prev    = tail;
            self->tail         = new_entry;
        }
    }
    self->size++;
    return RT_OK;
}

INT8 linked_list_insert_at(RtLinkedList* self, UINT32 index, void* data) {
    RT_ASSERT(RT_NULL != self);

    RtLinkedEntry* dst_entry = linked_list_get_entry(self, index);
    if (RT_NULL != dst_entry) {
        RtLinkedEntry* new_entry = rt_malloc(RtLinkedEntry);
        RT_ASSERT(RT_NULL != new_entry);
        rt_memset(new_entry, 0, sizeof(RtLinkedEntry));
        new_entry->data = data;
        new_entry->prev = dst_entry->prev;
        new_entry->next = dst_entry;
        dst_entry->prev->next = new_entry;
        dst_entry->prev = new_entry;
        if (RT_NULL == dst_entry->prev) {
            self->head = new_entry;
        } else if (RT_NULL == dst_entry->next) {
            self->tail = new_entry;
        }
        self->size++;
        return RT_OK;
    }
    return RT_ERR_OUTOF_RANGE;
}

INT8 linked_list_remove(RtLinkedList* self, void* data) {
    RT_ASSERT(RT_NULL != self);

    RtLinkedEntry* entry = self->head;
    if (RT_NULL != self->head) {
        while (entry != RT_NULL) {
            if (entry->data == data) {
                break;
            }
            entry = entry->next;
        }
    }
    if (RT_NULL != entry) {
        if (RT_NULL != entry->prev) {
            entry->prev->next = entry->next;
        } else {
            self->head = entry->next;
        }
        if (RT_NULL != entry->next) {
            entry->next->prev = entry->prev;
        } else {
            self->tail = entry->prev;
        }
        rt_free(entry);
        self->size--;
    }  else {
        return RT_ERR_NULL_PTR;
    }

    return RT_OK;
}

INT8 linked_list_remove_at(RtLinkedList* self, UINT32 index) {
    RT_ASSERT(RT_NULL != self);

    RtLinkedEntry* entry = linked_list_get_entry(self, index);
    if (RT_NULL != entry) {
        RtLinkedEntry* temp = entry;
        if (RT_NULL != entry->prev) {
            entry->prev->next = entry->next;
        } else {
            self->head = entry->next;
        }
        if (RT_NULL != entry->next) {
            entry->next->prev = entry->prev;
        } else {
            self->tail = entry->prev;
        }
        rt_free(temp);
        self->size--;
    } else {
        return RT_ERR_NULL_PTR;
    }
    return RT_OK;
}

INT8 linked_list_remove_all(RtLinkedList* self) {
    RT_ASSERT(RT_NULL != self);

    RtLinkedEntry* entry = self->head;
    if (RT_NULL != self->head) {
        while (entry != RT_NULL) {
            RtLinkedEntry* temp = entry;
            entry = entry->next;
            rt_free(temp);
        }
    }
    self->head = self->tail = RT_NULL;
    self->size = 0;
    return RT_OK;
}

RT_BOOL linked_list_contains(RtLinkedList* self, void* data) {
    RT_ASSERT(RT_NULL != self);

    RtLinkedEntry* entry = self->head;
    if (RT_NULL != self->head) {
        while (entry != RT_NULL) {
            if (entry->data == data) {
                return RT_TRUE;
            }
            entry = entry->next;
        }
    }
    return RT_FALSE;
}

UINT32 linked_list_get_size(RtLinkedList* self) {
    RT_ASSERT(RT_NULL != self);

    return self->size;
}

RtLinkedEntry* linked_list_get_entry(RtLinkedList* self, size_t index) {
    RT_ASSERT(RT_NULL != self);

    if ((index < 0) || (index >= self->size)) {
        return RT_NULL;
    }

    RtLinkedEntry* entry = RT_NULL;
    if (index < (self->size / 2)) {
        entry = self->head;
        for (UINT32 i = 1; i <= index; i++) {
            entry = entry->next;
        }
    } else {
        entry = self->tail;
        for (UINT32 i = self->size - 1; i > index; i--) {
            entry = entry->prev;
        }
    }
    return entry;
}

void* linked_list_get_data(RtLinkedList* self, size_t index) {
    RT_ASSERT(RT_NULL != self);

    if ((index < 0) || (index >= self->size)) {
        return RT_NULL;
    }

    RtLinkedEntry* entry = RT_NULL;
    if (index < (self->size / 2)) {
        entry = self->head;
        for (UINT32 i = 1; i <= index; i++) {
            entry = entry->next;
        }
    } else {
        entry = self->tail;
        for (UINT32 i = self->size - 1; i > index; i--) {
            entry = entry->prev;
        }
    }
    return entry->data;
}


INT8 linked_list_set(RtLinkedList* self, size_t index, void* data) {
    RtLinkedEntry* entry = linked_list_get_entry(self, index);
    if (RT_NULL == entry) {
        return RT_ERR_OUTOF_RANGE;
    }
    entry->data = data;
    return RT_OK;
}

INT8 linked_list_destroy(RtLinkedList **self) {
    RT_ASSERT(RT_NULL != *self);
    linked_list_remove_all(*self);
    rt_free(*self);
    *self = RT_NULL;
    return RT_OK;
}
