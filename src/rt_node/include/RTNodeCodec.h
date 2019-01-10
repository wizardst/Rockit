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
 *   date: 2018/07/05
 */

#ifndef SRC_RT_NODE_INCLUDE_RTNODECODEC_H_
#define SRC_RT_NODE_INCLUDE_RTNODECODEC_H_

#include "RTNode.h" // NOLINT

#ifdef  __cplusplus
extern "C" {
#endif

struct RTMessage;

class RTNodeCodec : public RTNode {
 protected:
    /*
     * command for callback,
     * callback setwhat must set the following.
     */
    enum {
        CB_INPUT_AVAILABLE              = 1,
        CB_OUTPUT_AVAILABLE             = 2,
        CB_ERROR                        = 3,
        CB_OUTPUT_FORMAT_CHANGED        = 4,
        CB_RESOURCE_RECLAIMED           = 5,
    };

    RTNodeCodec() {}
    ~RTNodeCodec() {}

 public:
    // borrow and return buffer for pool buffer
    virtual RT_RET dequeBuffer(RTMediaBuffer** data, RTPortType port) = 0;
    virtual RT_RET queueBuffer(RTMediaBuffer*  data, RTPortType port) = 0;
};

#ifdef  __cplusplus
}
#endif

#endif  // SRC_RT_NODE_INCLUDE_RTNODECODEC_H_

