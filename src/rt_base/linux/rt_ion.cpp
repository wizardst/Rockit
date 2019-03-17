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
 *   date: 2019/03/12
 * module: ion native api
 */

#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>

#include "rt_ion.h"     // NOLINT

INT32 ion_open() {
    INT32 fd = open(ION_DEV_NAME, O_RDWR);
    if (fd < 0) {
        RT_LOGE("open %s failed!\n", ION_DEV_NAME);
    }

    return fd;
}

INT32 ion_close(INT32 fd) {
    INT32 ret = close(fd);
    if (ret < 0) {
        return -errno;
    }

    return ret;
}

INT32 ion_ioctl(INT32 fd, INT32 req, void *arg) {
    INT32 ret = ioctl(fd, req, arg);
    if (ret < 0) {
        RT_LOGE("ion_ioctl %x failed with code %d: %s\n", req,
                ret, strerror(errno));
        return -errno;
    }
    return ret;
}

INT32 ion_alloc(
        INT32 fd,
        UINT32 len,
        UINT32 align,
        UINT32 heap_mask,
        UINT32 flags,
        ion_user_handle_t *handle) {
    INT32 ret = -EINVAL;
    struct ion_allocation_data data = {
        .len = len,
        .align = align,
        .heap_id_mask = heap_mask,
        .flags = flags,
    };

    RT_LOGT("enter: fd %d len %d align %d heap_mask %x flags %x",
                 fd, len, align, heap_mask, flags);

    if (handle) {
        ret = ion_ioctl(fd, ION_IOC_ALLOC, &data);
        if (ret >= 0)
            *handle = data.handle;
    }

    return ret;
}

INT32 ion_free(INT32 fd, ion_user_handle_t handle) {
    INT32 ret;
    struct ion_handle_data data = {
        .handle = handle,
    };

    ret = ion_ioctl(fd, ION_IOC_FREE, &data);
    return ret;
}

INT32 ion_map_fd(INT32 fd, ion_user_handle_t handle, INT32 *map_fd) {
    INT32 ret;
    struct ion_fd_data data = {
        .handle = handle,
    };

    if (map_fd == NULL)
        return -EINVAL;

    ret = ion_ioctl(fd, ION_IOC_MAP, &data);
    if (ret < 0)
        return ret;

    *map_fd = data.fd;
    if (*map_fd < 0) {
        RT_LOGE("map ioctl returned negative fd\n");
        return -EINVAL;
    }

    return 0;
}

INT32 ion_mmap(
        INT32 fd,
        UINT32 length,
        INT32 prot,
        INT32 flags,
        off_t offset,
        void **ptr) {
    static UINT32 pagesize_mask = 0;
    if (ptr == NULL)
        return -EINVAL;

    if (!pagesize_mask)
        pagesize_mask = sysconf(_SC_PAGESIZE) - 1;
    offset = offset & (~pagesize_mask);

    RT_LOGT("enter: fd %d len %d align %d flags %x",
                 fd, length, pagesize_mask, flags);

    *ptr = mmap(NULL, length, prot, flags, fd, offset);
    if (*ptr == MAP_FAILED) {
        RT_LOGE("mmap failed: %s\n", strerror(errno));
        *ptr = NULL;
        return -errno;
    }
    return 0;
}


