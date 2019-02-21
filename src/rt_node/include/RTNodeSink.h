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
 * author: shika.zhou@rock-chips.com
 *   date: 2019/02/21
 */

#ifndef SRC_RT_NODE_INCLUDE_RTNODESINK_H_
#define SRC_RT_NODE_INCLUDE_RTNODESINK_H_

#include "RTNode.h"  // NOLINT
#include "rt_type.h" // NOLINT

#ifdef  __cplusplus
extern "C" {
#endif

typedef void (*AudioQueueCodecBuffer) (RTNode* pNode, RTMediaBuffer* data);

class RTNodeSink : public RTNode {
 public:
    RTNode  *callback_ptr;
    AudioQueueCodecBuffer  queueCodecBuffer;
};

#ifdef  __cplusplus
}
#endif

#endif  // SRC_RT_NODE_INCLUDE_RTNODESINK_H_

