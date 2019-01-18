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
 * author: rimon.xu@rock-chips.com
 *   date: 2019/01/16
 * module: gralloc allocator
 */

#ifdef OS_LINUX

#include <unistd.h>
#include <sys/mman.h>
#include "drm.h"                    // NOLINT

#endif  // OS_LINUX

#include "RTAllocatorDrm.h"         // NOLINT
#include "rt_drm.h"                 // NOLINT
#include "RTBuffer.h"               // NOLINT
#include "rt_metadata.h"            // NOLINT
#include "RTMediaMetaKeys.h"        // NOLINT

RT_BOOL RTAllocatorDrm::checkAvail() {
    #ifdef OS_LINUX
    if (!access(DRM_DEV_NAME, 0)) {
        return RT_TRUE;
    } else {
        return RT_FALSE;
    }
    #endif

    return RT_FALSE;
}

#ifdef OS_LINUX

RTAllocatorDrm::RTAllocatorDrm(RtMetaData *config) {
    init(config);
}

RTAllocatorDrm::~RTAllocatorDrm() {
    deinit();
}

RT_RET RTAllocatorDrm::newBuffer(UINT32 capacity, RTBuffer **buffer) {
    RT_RET ret = RT_OK;

    RTBuffer *buffer_impl = NULL;

    INT32 err = 0;
    UINT32 handle = 0;
    err = drm_alloc(mDrmFd, capacity, mAlign, &handle, mFlags);
    if (err) {
        RT_LOGE("drm alloc failed");
        ret = RT_ERR_UNKNOWN;
        return ret;
    }

    INT32 fd = 0;
    void *data = RT_NULL;
    err = drm_map(mDrmFd, handle, capacity, mUsage, mFlags, 0, &data, &fd);
    if (err) {
        RT_LOGE("drm map failed!");
        ret = RT_ERR_UNKNOWN;
        return ret;
    }

    buffer_impl = new RTBuffer(data, capacity, handle, fd);
    if (!buffer_impl) {
        RT_LOGE("new buffer failed");
        ret = RT_ERR_UNKNOWN;
        return ret;
    }

    *buffer = buffer_impl;

    return ret;
}

RT_RET RTAllocatorDrm::newBuffer(
        UINT32 width,
        UINT32 height,
        UINT32 format,
        RTBuffer **buffer) {
    // TODO(graphic buffer alloc):
    (void)width;
    (void)height;
    (void)format;
    (void)buffer;
    return RT_OK;
}

RT_RET RTAllocatorDrm::freeBuffer(RTBuffer **buffer) {
    INT32 handle = (*buffer)->getHandle();
    void *data = (*buffer)->getData();
    INT32 size = (*buffer)->getSize();
    INT32 fd = (*buffer)->getFd();
    INT32 err = 0;
    RT_RET ret = RT_OK;
    if (handle >= 0 && handle) {
        munmap(data, size);
        close(fd);
        err = drm_free(mDrmFd, handle);
        if (err) {
            RT_LOGE("drm free failed!");
            ret = RT_ERR_UNKNOWN;
            return ret;
        }
    } else {
        RT_LOGE("free buffer failed!, data: %p size: %d, fd: %d, handle: %d",
                    data, size, fd, handle);
        ret = RT_ERR_UNKNOWN;
        return ret;
    }

    if (*buffer) {
        delete *buffer;
        *buffer = NULL;
    }
    return ret;
}

RT_RET RTAllocatorDrm::init(RtMetaData *config) {
    RT_RET ret = RT_OK;

    mDrmFd = drm_open();
    if (mDrmFd < 0) {
        ret = RT_ERR_UNKNOWN;
        return ret;
    }

    if (!config->findInt32(kKeyMemAlign, &mAlign)) {
        mAlign = 4096;
    }
    if (!config->findInt32(kKeyMemUsage, &mUsage)) {
        mUsage = PROT_READ | PROT_WRITE;
    }
    if (!config->findInt32(kKeyMemFlags, &mFlags)) {
        mFlags = MAP_SHARED;
    }
    if (!config->findInt32(kKeyMemFlags, &mHeapMask)) {
        mHeapMask = ROCKCHIP_BO_CONTIG;
    }

    return ret;
}

RT_RET RTAllocatorDrm::deinit() {
    RT_RET ret = RT_OK;
    INT32 err = 0;
    if (mDrmFd >= 0) {
        err = drm_close(mDrmFd);
        if (err) {
            RT_LOGE("drm close failed");
            ret = RT_ERR_UNKNOWN;
            return ret;
        }
    }

    return ret;
}

#endif  // OS_LINUX
