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
 * author: martin.cheng@rock-chips.com
 *   date: 2019/02/27
 * module: RTNDKMediaPlayer
 */

#ifndef SRC_RT_PLAYER_RTNDKMEDIAPLAYER_H_
#define SRC_RT_PLAYER_RTNDKMEDIAPLAYER_H_

#ifdef  __cplusplus
extern "C" {
#endif

#include <sys/types.h>
#include <inttypes.h>
#include "RTMediaError.h"

typedef int32_t rt_status;

#ifndef uid_t
#define uid_t int32_t
#endif

class  RTParcel;
struct RTNDKPlayerContext;
class RTNDKMediaPlayer {
 public:
    RTNDKMediaPlayer();
    ~RTNDKMediaPlayer();

    /*
     * basic control operations
     */
    rt_status setUID(uid_t uid);
    rt_status setDataSource(const char *url, const char *headers);
    rt_status setDataSource(int fd, int64_t offset, int64_t length);
    rt_status setLooping(int loop);
    rt_status setVideoSurfaceTexture(void* bufferProducer);
    rt_status setVideoSurface(void* surface);

    /*
     * basic operations
     */
    rt_status initCheck();
    rt_status prepare();
    rt_status prepareAsync();
    rt_status seekTo(int64_t usec);
    rt_status start();
    rt_status stop();
    rt_status pause();
    rt_status reset();

    /*
     * basic query operations
     */
    rt_status getState();
    rt_status getCurrentPosition(int64_t *usec);
    rt_status getDuration(int64_t *usec);
    rt_status dump(int fd, const char* args);

    /*
     * advanced query/cotrol operations
     *   Parcel is defined in /android/frameworks/native/libs/binder/Parcel.cpp
     * RTParcel is defined in /src/rt_media/sink-osal/RTParcel.cpp
     */
    rt_status invoke(const RTParcel &request, RTParcel *reply);
    rt_status setParameter(int key, const RTParcel &request);
    rt_status getParameter(int key, RTParcel *reply);

    /*
     * basic operations for audiotrack
     * attachAuxEffect: attaches an auxiliary effect to the audio track
     */
    rt_status setAudioSink(const void* audioSink);
    rt_status setVolume(float leftVolume, float rightVolume);
    rt_status setAuxEffectSendLevel(float level);
    rt_status attachAuxEffect(int effectId);

 private:
    struct RTNDKPlayerContext* mPlayerCtx;
};

enum RTMediaInfo {
    // 0xx
    RTM_INFO_UNKNOWN = 1,
    // The player was started because it was used as the next player for another
    // player, which just completed playback
    RTM_INFO_STARTED_AS_NEXT = 2,
    // The player just pushed the very first video frame for rendering
    RTM_INFO_RENDERING_START = 3,

    // 7xx
    // The video is too complex for the decoder: it can't decode frames fast
    // enough. Possibly only the audio plays fine at this stage.
    RTM_INFO_VIDEO_TRACK_LAGGING = 700,
    // pause playback internally in order to buffer more data.
    RTM_INFO_BUFFERING_START = 701,
    // resume playback after filling enough buffers.
    RTM_INFO_BUFFERING_END = 702,
    // Bandwidth in recent past
    RTM_INFO_NETWORK_BANDWIDTH = 703,
    // buffering percent of demuxer
    RTM_INFO_BUFFERING_PERCENT = 705,
    // loading percent of demuxer
    RTM_INFO_LOADING_PERCENT = 706,

    // 8xx
    // Bad interleaving means that a media has been improperly interleaved or not
    // interleaved at all, e.g has all the video samples first then all the audio
    // ones. Video is playing but a lot of disk seek may be happening.
    RTM_INFO_BAD_INTERLEAVING = 800,
    // The media is not seekable (e.g live stream).
    RTM_INFO_NOT_SEEKABLE     = 801,
    // New media metadata is available.
    RTM_INFO_METADATA_UPDATE  = 802,

    // 9xx
    RTM_INFO_TIMED_TEXT_ERROR = 900,
    RTM_INFO_PLAYING_START    = 901,
};

#ifdef  __cplusplus
}
#endif

#endif  // SRC_RT_PLAYER_RTNDKMEDIAPLAYER_H_
