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

#ifndef INCLUDE_RT_DEBUG_H_
#define INCLUDE_RT_DEBUG_H_

#include <assert.h>

#if RT_DEBUG
    #define RT_ASSERT(cond)            static_cast<void>(0)
    #define RT_ASSERT_IF(cond, fmt, ...) static_cast<void>(0)
    #define RT_DEBUGFAIL(messge)
    #define RT_ASSERT_RESULT(cond)      if (cond) {} do {} while (false)
#else
    #define RT_ASSERT(cond)
    #define RT_ASSERT_IF(cond, fmt, ...)
    #define RT_DEBUGFAIL(message)
    #define RT_ASSERT_RESULT(cond)
#endif

#endif  // INCLUDE_RT_DEBUG_H_
