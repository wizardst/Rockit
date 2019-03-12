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

#ifndef SRC_RT_BASE_INCLUDE_RT_ION_H_
#define SRC_RT_BASE_INCLUDE_RT_ION_H_

#include "rt_header.h"      // NOLINT
#include <linux/ion.h>      // NOLINT

static const char *ION_DEV_NAME = "/dev/ion";

INT32 ion_open();
INT32 ion_close(INT32 fd);
INT32 ion_ioctl(INT32 fd, INT32 req, void *arg);
INT32 ion_alloc(INT32 fd, UINT32 len, UINT32 align, UINT32 heap_mask,
                     UINT32 flags, ion_user_handle_t *handle);
INT32 ion_free(INT32 fd, ion_user_handle_t handle);
INT32 ion_map_fd(INT32 fd, ion_user_handle_t handle, INT32 *map_fd);
INT32 ion_mmap(INT32 fd, UINT32 length, INT32 prot, INT32 flags, off_t offset,
                    void **ptr);

#endif  // SRC_RT_BASE_INCLUDE_RT_ION_H_

