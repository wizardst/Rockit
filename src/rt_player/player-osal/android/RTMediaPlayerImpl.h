/*
 * Copyright 2019 Rockchip Electronics Co. LTD
 *
 * Licensed under the Apache License, Version 2.0 (the "License") { return NO_ERROR; }
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
 * module: RTMediaPlayerImpl impliments MediaPlayerInterface
 */

#ifndef SRC_RT_PLAYER_PLAYER_OSAL_ANDROID_RTMEDIAPLAYERIMPL_H_
#define SRC_RT_PLAYER_PLAYER_OSAL_ANDROID_RTMEDIAPLAYERIMPL_H_

#include <media/MediaPlayerInterface.h>

using namespace android;  // NOLINT

class RTMediaPlayerImpl : public MediaPlayerInterface {
 public:
    RTMediaPlayerImpl();
    virtual ~RTMediaPlayerImpl();

    void    setListener(const wp<MediaPlayerBase> &listener);

    virtual status_t initCheck();

    virtual status_t setUID(uid_t uid);

    virtual status_t setDataSource(
            const sp<IMediaHTTPService> &httpService,
            const char *url,
            const KeyedVector<String8, String8> *headers = NULL);
    virtual status_t setDataSource(int fd, int64_t offset, int64_t length);
    virtual status_t setDataSource(const sp<IStreamSource> &source);
    virtual status_t setVideoSurfaceTexture(
            const sp<IGraphicBufferProducer> &bufferProducer);
    virtual status_t prepare();
    virtual status_t prepareAsync();
    virtual status_t start();
    virtual status_t stop();
    virtual status_t pause();
    virtual bool     isPlaying();
    virtual status_t seekTo(int msec);
    virtual status_t getCurrentPosition(int *msec);
    virtual status_t getDuration(int *msec);
    virtual status_t reset();
    virtual status_t setLooping(int loop);
    virtual player_type playerType();
    virtual status_t invoke(const Parcel &request, Parcel *reply);
    virtual void setAudioSink(const sp<MediaPlayerBase::AudioSink> &audioSink);
    virtual status_t setParameter(int key, const Parcel &request);
    virtual status_t getParameter(int key, Parcel *reply);
    virtual uint32_t flags();
    virtual status_t getMetadata(
            const media::Metadata::Filter& ids, Parcel *records);

    virtual status_t dump(int fd, const Vector<String16> &args) const;
};

#endif  // SRC_RT_PLAYER_PLAYER_OSAL_ANDROID_RTMEDIAPLAYERIMPL_H_
