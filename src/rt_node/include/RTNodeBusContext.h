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
 *   date: 20180714
 */

#ifndef SRC_RT_NODE_INCLUDE_RTNODEBUSCONTEXT_H_
#define SRC_RT_NODE_INCLUDE_RTNODEBUSCONTEXT_H_

#include "RTMediaMetaKeys.h"

class RtMetaData;
typedef struct _RTNodeBusContext {
    RtMetaData* mMetaFormat;
    RtMetaData* mMetaVideo;
    RtMetaData* mMetaAudio;
    RtMetaData* mMetaSubtitle;
    RtMetaData* mDeviceDisplay;
    RtMetaData* mDeviceAudio;
} RTNodeBusContext;

#endif  // SRC_RT_NODE_INCLUDE_RTNODEBUSCONTEXT_H_
