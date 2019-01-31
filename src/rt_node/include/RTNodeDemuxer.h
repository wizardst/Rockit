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

#ifndef SRC_RT_NODE_INCLUDE_RTNODEDEMUXER_H_
#define SRC_RT_NODE_INCLUDE_RTNODEDEMUXER_H_

#include "RTNode.h"  // NOLINT
#include "rt_type.h" // NOLINT
#include "RTMediaData.h" // NOLINT

#ifdef  __cplusplus
extern "C" {
#endif

class RTNodeDemuxer : public RTNode {
 public:
    virtual INT32       countTracks(RTTrackType tType) = 0;
    virtual INT32       selectTrack(INT32 index, RTTrackType tType) = 0;
    virtual RtMetaData* queryTrackMeta(UINT32 index, RTTrackType tType) = 0;
    virtual INT32       queryTrackUsed(RTTrackType tType) = 0;
    virtual INT32       queryDrmFlag()  { return 0; }
    virtual INT32       querySeekFlag() { return 0; }
};

#ifdef  __cplusplus
}
#endif

#endif  // SRC_RT_NODE_INCLUDE_RTNODEDEMUXER_H_

