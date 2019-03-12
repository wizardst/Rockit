/*
 * Copyright 2019 Rockchip Electronics Co. LTD
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
 *   date: 2019/03/13
 * module: default malloc allocator
 */

#include "RTMediaBuffer.h"          // NOLINT
#include "rt_metadata.h"            // NOLINT
#include "RTAllocatorMalloc.h"      // NOLINT

RTAllocatorMalloc::RTAllocatorMalloc(RtMetaData *config) {
    init(config);
}

RTAllocatorMalloc::~RTAllocatorMalloc() {
    deinit();
}

RT_BOOL RTAllocatorMalloc::checkAvail() {
    return RT_TRUE;
}

RT_RET RTAllocatorMalloc::newBuffer(UINT32 capacity, RTMediaBuffer **buffer) {
    void* pData = rt_mem_malloc(__FUNCTION__, capacity);
    if (RT_NULL != pData) {
        *buffer = new RTMediaBuffer(pData, capacity, 0, 0, this);
    } else {
        *buffer = RT_NULL;
        return RT_ERR_NOMEM;
    }
    return RT_OK;
}

RT_RET RTAllocatorMalloc::newBuffer(UINT32 width,
                             UINT32 height,
                             UINT32 format,
                             RTMediaBuffer **buffer) {
    // YUV420 Default
    UINT32 capacity = width*height*3/2;
    return newBuffer(capacity, buffer);
}

RT_RET RTAllocatorMalloc::freeBuffer(RTMediaBuffer **buffer) {
    if (RT_NULL != *buffer) {
        // allocator_malloc knows how to release buffer
        rt_free((*buffer)->getData());
        (*buffer)->setData(RT_NULL, 0);

        // delete buffer wrapper
        rt_safe_delete(*buffer);
        *buffer = RT_NULL;
    }
    return RT_OK;
}

RT_RET RTAllocatorMalloc::init(RtMetaData *meta) {
    return RT_OK;
}

RT_RET RTAllocatorMalloc::deinit() {
    return RT_OK;
}



