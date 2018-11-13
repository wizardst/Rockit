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
 *   date: 2018/07/19
 */

#ifdef OS_WINDOWS
#include <malloc.h>
#include "../rt_os_mem.h"

#define WITH_ALIGNED_MEM 0
int rt_os_malloc(void **memptr, size_t alignment, size_t size) {
    #if WITH_ALIGNED_MEM
    *memptr = _aligned_malloc(size, alignment);
    #else
    *memptr = malloc(size);
    #endif
    return (*memptr) ? (0) : (-1);
}

int rt_os_realloc(void *src, void **dst, size_t alignment, size_t size) {
    #if WITH_ALIGNED_MEM
    *dst = _aligned_realloc(src, size, alignment);
    #else
    *dst = realloc(src, size);
    #endif

    return (*dst) ? (0) : (-1);
}

void rt_os_free(void *ptr) {
    #if WITH_ALIGNED_MEM
    _aligned_free(ptr);
    #else
    free(ptr);
    #endif
}

#endif
