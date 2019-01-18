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

#ifndef SRC_RT_MEDIA_INCLUDE_RTALLOCATORION_H_
#define SRC_RT_MEDIA_INCLUDE_RTALLOCATORION_H_

#include "RTAllocatorBase.h"

class RTAllocatorIon : public RTAllocator {
 public:
    RTAllocatorIon();
    ~RTAllocatorIon();
};

#endif  // SRC_RT_MEDIA_INCLUDE_RTALLOCATORION_H_

