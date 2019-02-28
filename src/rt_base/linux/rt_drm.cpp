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
 * module: drm native api
 */

#ifdef OS_LINUX

#include <sys/mman.h>
#include <fcntl.h>

#include "rt_drm.h"         // NOLINT
#include "drm.h"            // NOLINT
#include "drm_mode.h"       // NOLINT

INT32 drm_open() {
    INT32 fd = open(DRM_DEV_NAME, O_RDWR);
    if (fd < 0) {
        RT_LOGE("open %s failed!\n", DRM_DEV_NAME);
    }

    return fd;
}

INT32 drm_close(INT32 fd) {
    INT32 ret = close(fd);
    if (ret < 0) {
        return -errno;
    }

    return ret;
}


int drm_get_phys(int fd, UINT32 handle, UINT32 *phy, UINT32 heaps) {
    struct drm_rockchip_gem_phys phys_arg;
    phys_arg.handle = handle;
    int ret = drm_ioctl(fd, DRM_IOCTL_ROCKCHIP_GEM_GET_PHYS, &phys_arg);
    if (ret < 0) {
        if (heaps != ROCKCHIP_BO_SECURE || heaps != ROCKCHIP_BO_CONTIG) {
            RT_LOGD("not have phys addr if not cma or secure buffer");
            *phy = 0;
            return RT_OK;
        } else {
            RT_LOGE("drm_get_phys failed ret = %d", ret);
            return ret;
        }
    } else {
        *phy = phys_arg.phy_addr;
    }
    return ret;
}

int drm_ioctl(INT32 fd, INT32 req, void* arg) {
    INT32 ret = ioctl(fd, req, arg);
    if (ret < 0) {
        RT_LOGE("fd: %d ioctl %x failed with code %d: %s\n", fd, req, ret, strerror(errno));
        return -errno;
    }
    return ret;
}

INT32 drm_handle_to_fd(INT32 fd, UINT32 handle, int *map_fd, UINT32 flags) {
    INT32 ret;
    struct drm_prime_handle dph;
    memset(&dph, 0, sizeof(struct drm_prime_handle));
    dph.handle = handle;
    dph.fd = -1;
    dph.flags = flags;

    if (map_fd == NULL)
        return -EINVAL;

    ret = drm_ioctl(fd, DRM_IOCTL_PRIME_HANDLE_TO_FD, &dph);
    if (ret < 0)
        return ret;

    *map_fd = dph.fd;

    RT_LOGD("get fd %d", *map_fd);

    if (*map_fd < 0) {
        RT_LOGE("map ioctl returned negative fd\n");
        return -EINVAL;
    }

    return ret;
}

INT32 drm_fd_to_handle(
        INT32 fd,
        INT32 map_fd,
        UINT32 *handle,
        UINT32 flags) {
    INT32 ret;
    struct drm_prime_handle dph;

    dph.fd = map_fd;
    dph.flags = flags;

    ret = drm_ioctl(fd, DRM_IOCTL_PRIME_FD_TO_HANDLE, &dph);
    if (ret < 0) {
        RT_LOGE("DRM_IOCTL_PRIME_FD_TO_HANDLE failed!");
        return ret;
    }

    *handle = dph.handle;

    RT_LOGD("get handle %d", *handle);

    return ret;
}

INT32 drm_alloc(
        INT32 fd,
        UINT32 len,
        UINT32 align,
        UINT32 *handle,
        UINT32 flags,
        UINT32 heaps) {
    (void)flags;
    INT32 ret;
    struct drm_mode_create_dumb dmcb;

    RT_LOGD("len %zu aligned %zu\n", len, align);

    memset(&dmcb, 0, sizeof(struct drm_mode_create_dumb));
    dmcb.bpp = 8;
    dmcb.width = (len + align - 1) & (~(align - 1));
    dmcb.height = 1;
    dmcb.flags = heaps;

    if (handle == NULL) {
        RT_LOGE("drm_alloc: handle is NULL");
        return -EINVAL;
    }

    ret = drm_ioctl(fd, DRM_IOCTL_MODE_CREATE_DUMB, &dmcb);
    if (ret < 0) {
        RT_LOGE("drm_alloc failed: %s\n", strerror(errno));
        return ret;
    }

    RT_LOGD("fd %d aligned %d size %llu\n", fd, align, dmcb.size);
    *handle = dmcb.handle;

    RT_LOGD("get handle %d size %llu", *handle, dmcb.size);

    return ret;
}

INT32 drm_free(INT32 fd, UINT32 handle) {
    struct drm_mode_destroy_dumb data = {
        .handle = handle,
    };
    return drm_ioctl(fd, DRM_IOCTL_MODE_DESTROY_DUMB, &data);
}

#if 1
#include <errno.h> /* for EINVAL */

inline void *drm_mmap(void *addr, UINT32 length, INT32 prot, INT32 flags,
                             INT32 fd, loff_t offset) {
    /* offset must be aligned to 4096 (not necessarily the page size) */
    if (offset & 4095) {
        errno = EINVAL;
        return MAP_FAILED;
    }

    return mmap64(addr, length, prot, flags, fd, offset);
}

#define drm_munmap(addr, length) \
              munmap(addr, length)

#else

/* assume large file support exists */
#  define drm_mmap(addr, length, prot, flags, fd, offset) \
              mmap(addr, length, prot, flags, fd, offset)

#  define drm_munmap(addr, length) \
              munmap(addr, length)

#endif

INT32 drm_map(INT32 fd, UINT32 handle, UINT32 length, INT32 prot,
                   INT32 flags, INT32 offset, void **ptr, INT32 *map_fd, UINT32 heaps) {
    INT32 ret;
    struct drm_mode_map_dumb dmmd;
    static UINT32 pagesize_mask = 0;
    memset(&dmmd, 0, sizeof(dmmd));
    dmmd.handle = handle;

    (void)offset;
    (void)flags;

    if (map_fd == NULL)
        return -EINVAL;
    if (ptr == NULL)
        return -EINVAL;

    ret = drm_handle_to_fd(fd, handle, map_fd, 0);
    RT_LOGD("drm_map fd %d", *map_fd);
    if (ret < 0)
        return ret;

    ret = drm_ioctl(fd, DRM_IOCTL_MODE_MAP_DUMB, &dmmd);
    if (ret < 0) {
        RT_LOGE("drm_map FAIL");
        close(*map_fd);
        return ret;
    }

    if (!pagesize_mask)
        pagesize_mask = sysconf(_SC_PAGESIZE) - 1;

    RT_LOGD("dev fd %d length %d pagesize_mask: 0x%x", fd, length, pagesize_mask);

    length = (length + pagesize_mask) & ~pagesize_mask;

    *ptr = drm_mmap(NULL, length, prot, MAP_SHARED, fd, dmmd.offset);
    if (*ptr == MAP_FAILED) {
        if (heaps == ROCKCHIP_BO_SECURE) {
            RT_LOGD("not have phys addr if not cma or secure buffer");
            close(*map_fd);
            *map_fd = -1;
            return RT_OK;
        } else {
            close(*map_fd);
            *map_fd = -1;
            RT_LOGE("mmap failed: %s length: %d\n", strerror(errno), length);
            return -errno;
        }
    }

    return ret;
}
#endif  // OS_LINUX
