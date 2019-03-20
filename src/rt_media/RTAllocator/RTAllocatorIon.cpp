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
 * module: ion allocator
 */

#include <unistd.h>
#include <sys/mman.h>

#include "RTAllocatorIon.h"         // NOLINT
#include "rt_ion.h"                 // NOLINT
#include "RTMediaBuffer.h"          // NOLINT
#include "rt_metadata.h"            // NOLINT
#include "RTMediaMetaKeys.h"        // NOLINT

RTAllocatorIon::RTAllocatorIon(RtMetaData *config)
    : mAlign(4096),
      mHeapMask(0),
      mFlags(0),
      mUsage(0),
      mIonFd(0) {
    init(config);
}

RTAllocatorIon::~RTAllocatorIon() {
    deinit();
}


RT_BOOL RTAllocatorIon::checkAvail() {
    if (!access(ION_DEV_NAME, 0)) {
        return RT_TRUE;
    } else {
        return RT_FALSE;
    }

    return RT_FALSE;
}

RT_RET RTAllocatorIon::newBuffer(UINT32 capacity, RTMediaBuffer **buffer) {
    RT_RET ret = RT_OK;

    RTMediaBuffer *buffer_impl = NULL;

    INT32 err = 0;
    ion_user_handle_t hnd = -1;
    err = ion_alloc(mIonFd, capacity, mAlign, mHeapMask, mFlags, &hnd);
    if (err) {
        RT_LOGE("drm alloc failed");
        ret = RT_ERR_UNKNOWN;
        return ret;
    }

    INT32 fd = 0;
    err = ion_map_fd(mIonFd, hnd, &fd);
    if (err) {
        RT_LOGE("ion_map_fd failed ret %d\n", err);
    }

    void *data = RT_NULL;
    err = ion_mmap(fd, capacity, mUsage, MAP_SHARED, mFlags, &data);
    if (err) {
        RT_LOGE("ion map failed!");
        ret = RT_ERR_UNKNOWN;
        return ret;
    }

    buffer_impl = new RTMediaBuffer(data, capacity, hnd, fd, this);
    if (!buffer_impl) {
        RT_LOGE("new buffer failed");
        ret = RT_ERR_UNKNOWN;
        return ret;
    }

    *buffer = buffer_impl;
    return ret;
}

RT_RET RTAllocatorIon::newBuffer(
        UINT32 width,
        UINT32 height,
        UINT32 format,
        RTMediaBuffer **buffer) {
    // TODO(graphic buffer alloc):
    (void)width;
    (void)height;
    (void)format;
    (void)buffer;
    return RT_OK;
}

RT_RET RTAllocatorIon::freeBuffer(RTMediaBuffer **buffer) {
    INT32 handle = (*buffer)->getHandle();
    void *data = (*buffer)->getData();
    INT32 size = (*buffer)->getSize();
    INT32 fd = (*buffer)->getFd();
    INT32 err = 0;
    RT_RET ret = RT_OK;

    RT_LOGT("freeBuffer buffer: %p, fd: %d, handle: %d, fd: %d, data: %p",
             *buffer, mIonFd, handle, fd, data);
    if (data) {
        munmap(data, size);
    }

    if (fd > 0) {
        close(fd);
    }

    if (handle) {
        err = ion_free(mIonFd, handle);
        if (err) {
            RT_LOGE("drm free failed!");
            ret = RT_ERR_UNKNOWN;
            return ret;
        }
    }

    rt_safe_delete(*buffer);
    return ret;
}

RT_RET RTAllocatorIon::init(RtMetaData *config) {
    RT_RET ret = RT_OK;

    mIonFd = ion_open();
    if (mIonFd < 0) {
        ret = RT_ERR_UNKNOWN;
        return ret;
    }

    if (!config->findInt32(kKeyMemAlign, &mAlign)) {
        mAlign = 4096;
    }

    if (!config->findInt32(kKeyMemUsage, &mUsage)) {
        mUsage = PROT_READ | PROT_WRITE;
    }

    if (!config->findInt32(kKeyMemHeapMask, &mHeapMask)) {
        mHeapMask = 1 << ION_HEAP_TYPE_SYSTEM;
    }

    if (!config->findInt32(kKeyMemFlags, &mFlags)) {
        mFlags = 0;
    }

    return ret;
}

RT_RET RTAllocatorIon::deinit() {
    RT_RET ret = RT_OK;
    INT32 err = 0;
    if (mIonFd >= 0) {
        err = ion_close(mIonFd);
        if (err) {
            RT_LOGE("ion close failed");
            ret = RT_ERR_UNKNOWN;
            return ret;
        }
    }

    return ret;
}

