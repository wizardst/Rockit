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

#ifndef SRC_RT_BASE_INCLUDE_RT_DRM_H_
#define SRC_RT_BASE_INCLUDE_RT_DRM_H_

#include "rt_header.h"  // NOLINT

static const char *DRM_DEV_NAME = "/dev/dri/card0";

INT32 drm_open();
INT32 drm_close(INT32 fd);
INT32 drm_ioctl(INT32 fd, INT32 req, void* arg);
INT32 drm_get_phys(int fd, UINT32 handle, UINT32 *phy, UINT32 heaps);
INT32 drm_handle_to_fd(INT32 fd, UINT32 handle, INT32 *map_fd, UINT32 flags);
INT32 drm_fd_to_handle(INT32 fd, INT32 map_fd, UINT32 *handle, UINT32 flags);
INT32 drm_alloc(INT32  fd, UINT32 len, UINT32 align, UINT32 *handle, UINT32 flags, UINT32 heaps);
INT32 drm_free(INT32  fd, UINT32 handle);

#ifdef OS_LINUX
void *drm_mmap(void *addr, UINT32 length, INT32  prot, INT32  flags, INT32  fd, loff_t offset);
#endif
INT32 drm_map(INT32  fd,
               UINT32 handle,
               UINT32 length,
               INT32  prot,
               INT32  flags,
               INT32  offset,
               void **ptr,
               INT32 *map_fd,
               UINT32 heaps);

#endif  // SRC_RT_BASE_INCLUDE_RT_DRM_H_
