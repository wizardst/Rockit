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
 * Author: martin.cheng@rock-chips.com
 *   Date: 2019/03/10
 *   Task: build player streamline with node bus.
 */

#ifndef SRC_RT_PLAYER_RTNDKNODEPLAYER_H_
#define SRC_RT_PLAYER_RTNDKNODEPLAYER_H_

#include "RTNodeBus.h"       // NOLINT
#include "RTNDKMediaDef.h"   // NOLINT
#include "RTNode.h"          // NOLINT
#include "rt_header.h"       // NOLINT
#include "rt_msg_handler.h"  // NOLINT
#include "rt_msg_looper.h"   // NOLINT

class RTNodeBus;
struct NodePlayerContext;

class RTNDKNodePlayer : public RTMsgHandler {
 public:
    RTNDKNodePlayer();
    ~RTNDKNodePlayer();

    /* commit control operations */
    RT_RET    init();
    RT_RET    release();
    RT_RET    reset();
    RT_RET    setDataSource(RTMediaUri *mediaUri);
    RT_RET    prepare();
    RT_RET    start();
    RT_RET    pause();
    RT_RET    stop();
    RT_RET    wait();
    RT_RET    seekTo(INT64 usec);
    RT_RET    summary(INT32 fd);
    RT_RET    setCurState(UINT32 newState);
    UINT32    getCurState();

    /* looper functions or callback of thread */
    void      onMessageReceived(struct RTMessage* msg);
    RT_RET    startDataLooper();
    RT_RET    startAudioPlayerProc();

 private:
    RT_RET    postSeekIfNecessary();
    RT_RET    onSeekTo(INT64 usec);
    RT_RET    onPlaybackDone();

 private:
    struct NodePlayerContext* mPlayerCtx;
    RTNodeBus*                mNodeBus;
};

#endif  // SRC_RT_PLAYER_RTNDKNODEPLAYER_H_
