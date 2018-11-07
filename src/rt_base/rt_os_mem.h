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

#ifndef __RT_OS_MEM_H__
#define __RT_OS_MEM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "rt_type.h"

#define MEM_ALIGN               32

int  rt_os_malloc(void **memptr, UINT32 alignment, UINT32 size);
int  rt_os_realloc(void *src, void **dst, UINT32 alignment, UINT32 size);
void rt_os_free(void *ptr);

#ifdef __cplusplus
}
#endif

#endif /*__RT_OS_MEM_H__*/


