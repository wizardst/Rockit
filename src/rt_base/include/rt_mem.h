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
 *   date: 20180704
 */

#ifndef SRC_RT_BASE_INCLUDE_RT_MEM_H_
#define SRC_RT_BASE_INCLUDE_RT_MEM_H_

#include <stdlib.h>

#define rt_malloc(type)  \
    reinterpret_cast<type *>( \
        rt_mem_malloc(__FUNCTION__, sizeof(type)))

#define rt_malloc_array(type, count)  \
    reinterpret_cast<type *>( \
        rt_mem_malloc(__FUNCTION__, sizeof(type) * (count)))

#define rt_malloc_size(type, size)  \
    reinterpret_cast<type *>( \
        rt_mem_malloc(__FUNCTION__, size))

#define rt_calloc_size(type, size)  \
    reinterpret_cast<type *>( \
        rt_mem_calloc(__FUNCTION__, size))

#define rt_calloc(type, count)  \
    reinterpret_cast<type *>( \
        rt_mem_calloc(__FUNCTION__, sizeof(type) * (count)))

#define rt_realloc(ptr, type, count) \
    reinterpret_cast<type *>( \
        rt_mem_realloc(__FUNCTION__, ptr, sizeof(type) * (count)))

#define rt_free(ptr) \
    rt_mem_free(__FUNCTION__, ptr)

#define rt_safe_free(ptr) \
    rt_mem_safe_free( \
        __FUNCTION__, reinterpret_cast<void **>(&(ptr)))

#ifndef rt_safe_delete
#define rt_safe_delete(p) { if (p) {delete(p); (p)=NULL;} }
#endif

#ifdef __cplusplus
extern "C" {
#endif

void *rt_mem_malloc(const char *caller, size_t size);
void *rt_mem_calloc(const char *caller, size_t size);
void *rt_mem_realloc(const char *caller, void *ptr, size_t size);
void      rt_mem_free(const char *caller, void *ptr);
void rt_mem_safe_free(const char *caller, void **ptr);
void      *rt_memset(void *ptr, int c, size_t n);
void      *rt_memcpy(void *dst, void *src, size_t n);

void rt_mem_record_dump();
void rt_mem_record_reset();

#ifdef __cplusplus
}
#endif

#endif  // SRC_RT_BASE_INCLUDE_RT_MEM_H_
