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
 *   date: 20180704
 */

#include "rt_header.h"
#include "rt_mem.h"
#include "rt_log.h"
#include "rt_os_mem.h"
#include "rt_mem_service.h"
#include <string.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "rt_mem"

#define MEM_DEBUG_EN            (0x00000001)
#define MEM_RUNTIME_LOG         (0x00000002)
#define MEM_NODE_LOG            (0x00000004)
#define MEM_EXT_ROOM            (0x00000010)
#define MEM_POISON              (0x00000020)

// default memory align size is set to 32
#define MEM_MAX_INDEX           (0x7fffffff)
#define MEM_ALIGN_MASK          (MEM_ALIGN - 1)
#define MEM_ALIGNED(x)          (((x) + MEM_ALIGN) & (~MEM_ALIGN_MASK))
#define MEM_HEAD_ROOM(debug)    ((debug & MEM_EXT_ROOM) ? (MEM_ALIGN) : (0))

static UINT8 debug = MEM_EXT_ROOM;
static rt_mem_service mem_records;

void *rt_mem_malloc (const char *caller, UINT32 size) {
    //AutoMutex auto_lock(&service.lock);

    UINT32 size_align = MEM_ALIGNED(size);
    UINT32 size_real = (debug & MEM_EXT_ROOM) ? (size_align + 2 * MEM_ALIGN) :
                       (size_align);
    void *ptr;

    rt_os_malloc(&ptr, MEM_ALIGN, size_real);

    // TODO : debug memory
    mem_records.add_node(caller, ptr, size);

    return ptr;
}

void *rt_mem_calloc (const char *caller, UINT32 size) {
    void *ptr = rt_mem_malloc(caller, size);
    if (ptr)
        memset(ptr, 0, size);
    return ptr;
}

void *rt_mem_realloc(const char *caller, void *ptr, UINT32 size) {
    //AutoMutex auto_lock(&service.lock);
    void *ret;

    if (NULL == ptr)
        return rt_mem_malloc(caller, size);

    if (0 == size) {
        RT_LOGE("warning: realloc %p to zero size\n", ptr);
        return NULL;
    }

    size_t size_align = MEM_ALIGNED(size);
    //size_t size_real = (debug & MEM_EXT_ROOM) ? (size_align + 2 * MEM_ALIGN) :
    //                   (size_align);
    void *ptr_real = (UINT8 *)ptr - MEM_HEAD_ROOM(debug);

    rt_os_realloc(ptr_real, &ret, MEM_ALIGN, size_align);

    // TODO : debug memory
    // mem_service::dump(args)

    return ret;
}

void rt_mem_free(const char *caller, void *ptr) {
    //AutoMutex auto_lock(&service.lock);
    if (NULL == ptr)
        return;
    if (ptr < (void*)0xFFF) {
        RT_LOGE("rt_mem_free caller=%s, ptr=%p", caller, ptr);
        return ;
    }

    rt_os_free(ptr);

    // TODO : debug memory
    UINT32 size;
    mem_records.remove_node(ptr, &size);

    return ;
}

void *rt_memset(void *str, int c, size_t n) {
    return memset(str, c, n);
}

void rt_mem_record_dump() {
    // TODO : debug memory
    mem_records.dump();

    return ;
}

void rt_mem_record_reset() {
    // TODO : debug memory
    mem_records.reset();

    return ;
}
