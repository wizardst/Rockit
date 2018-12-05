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

#ifndef INCLUDE_RT_TYPE_H_
#define INCLUDE_RT_TYPE_H_

#include <stdint.h>

#define RT_NULL 0

#define TO_DO_FLAG 0

#define RT_BOOL  char
#define RT_TRUE  1
#define RT_FALSE 0

typedef int8_t       INT8;
typedef int16_t      INT16;
typedef int32_t      INT32;
typedef int64_t      INT64;

typedef uint8_t      UINT8;
typedef uint16_t     UINT16;
typedef uint32_t     UINT32;
typedef uint64_t     UINT64;

typedef void *       RT_PTR;

#define RtToBool(cond)  ((cond) != 0)
#define RtToS8(x)     ((INT8)(x))
#define RtToU8(x)    ((UINT8)(x))
#define RtToS16(x)   ((INT16)(x))
#define RtToU16(x)  ((UINT16)(x))
#define RtToS32(x)   ((INT32)(x))
#define RtToU32(x)  ((UINT32)(x))

#define RT_MaxS16   32767
#define RT_MinS16   -32767
#define RT_MaxU16   0xFFFF
#define RT_MinU16   0
#define RT_MaxS32   0x7FFFFFFF
#define RT_MinS32   -SK_MaxS32
#define RT_MaxU32   0xFFFFFFFF
#define RT_MinU32   0
#define RT_NaN32    (1 << 31)

#define RtAlign2(x)     (((x) + 1) >> 1 << 1)
#define RtIsAlign2(x)   (0 == ((x) & 1))

#define RtAlign4(x)     (((x) + 3) >> 2 << 2)
#define RtIsAlign4(x)   (0 == ((x) & 3))

#define RtAlign8(x)     (((x) + 7) >> 3 << 3)
#define RtIsAlign8(x)   (0 == ((x) & 7))

#define MKTAG(a, b, c, d) ((a) | ((b) << 8) | ((c) << 16) | ((unsigned)(d) << 24))

#endif  // INCLUDE_RT_TYPE_H_
