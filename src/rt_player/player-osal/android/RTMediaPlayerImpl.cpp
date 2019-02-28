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
 * module: RTMediaPlayerImpl
 */

#include "RTMediaPlayerImpl.h"  // NOLINT
#include "RTNDKIPTVPlayer.h"    // NOLINT

#define LOG_NDEBUG 0
#define LOG_TAG "RTMediaPlayerImpl"
#include <utils/Log.h>
#include <media/Metadata.h>
#include <media/stagefright/MediaExtractor.h>

RTMediaPlayerImpl::RTMediaPlayerImpl() {
    ALOGE("+Constructor %p", this);
    ALOGE("+Constructor %p @Done", this);
}

RTMediaPlayerImpl::~RTMediaPlayerImpl() {
    ALOGE("~Destructor %p", this);
    ALOGE("~Destructor %p @Done", this);
}

status_t RTMediaPlayerImpl::initCheck() {
    ALOGE("initCheck, @TODO");
    return NO_ERROR;
}

status_t RTMediaPlayerImpl::setUID(uid_t uid) {
    ALOGE("setUID, @TODO");
    return NO_ERROR;
}

status_t RTMediaPlayerImpl::setDataSource(const sp<IMediaHTTPService> &httpService,
           const char *url, const KeyedVector<String8, String8> *headers = NULL) {
    ALOGV("setDataSource, uri = %s", url);
    return NO_ERROR;
}

status_t RTMediaPlayerImpl::setDataSource(int fd, int64_t offset, int64_t length) {
    ALOGV("setDataSource(%d, %lld, %lld)", fd, offset, length);
    return NO_ERROR;
}

status_t RTMediaPlayerImpl::setDataSource(const sp<IStreamSource> &source) {
    ALOGV("setDataSource with source");
    return NO_ERROR;
}
status_t RTMediaPlayerImpl::setVideoSurfaceTexture(
        const sp<IGraphicBufferProducer> &bufferProducer) {
    ALOGV("setVideoSurfaceTexture");
    return NO_ERROR;
}

status_t RTMediaPlayerImpl::prepare() {
    ALOGV("prepare");
    return NO_ERROR;
}

status_t RTMediaPlayerImpl::prepareAsync() {
    ALOGV("prepareAsync");
    return NO_ERROR;
}

status_t RTMediaPlayerImpl::start() {
    ALOGV("start");
    return NO_ERROR;
}

status_t RTMediaPlayerImpl::stop()  {
    ALOGV("stop");
    return NO_ERROR;
}

status_t RTMediaPlayerImpl::pause()  {
    ALOGV("pause");
    return NO_ERROR;
}

bool RTMediaPlayerImpl::isPlaying() {
    ALOGV("isPlaying");
    return false;
}

status_t RTMediaPlayerImpl::seekTo(int msec) {
    ALOGV("seekTo %.2f secs", msec / 1E3);
    return NO_ERROR;
}

status_t RTMediaPlayerImpl::getCurrentPosition(int *msec) {
    ALOGV("getCurrentPosition");
    *msec = 0;
    return NO_ERROR;
}

status_t RTMediaPlayerImpl::getDuration(int *msec) {
    ALOGV("getDuration");
    return NO_ERROR;
}

status_t RTMediaPlayerImpl::reset() {
    ALOGV("reset");
    return NO_ERROR;
}

status_t RTMediaPlayerImpl::setLooping(int loop) {
    ALOGV("setLooping");
    return NO_ERROR;
}

player_type RTMediaPlayerImpl::playerType() {
    ALOGV("playerType");
    return FF_PLAYER;
}

status_t RTMediaPlayerImpl::invoke(const Parcel &request, Parcel *reply) {
    ALOGV("invoke");
    return NO_ERROR;
}

void RTMediaPlayerImpl::setAudioSink(const sp<MediaPlayerBase::AudioSink> &audioSink) {
    ALOGV("setAudioSink");
}

status_t RTMediaPlayerImpl::setParameter(int key, const Parcel &request) {
    ALOGV("setParameter");
    return NO_ERROR;
}

status_t RTMediaPlayerImpl::getParameter(int key, Parcel *reply) {
    ALOGV("getParameter");
    return NO_ERROR;
}

uint32_t RTMediaPlayerImpl::flags() {
    ALOGV("flags");
    return 0;
}

status_t RTMediaPlayerImpl::getMetadata(
        const media::Metadata::Filter& ids, Parcel *records) {
    return NO_ERROR;
}

status_t RTMediaPlayerImpl::dump(int fd, const Vector<String16> &args) const {
    ALOGD("\r\n.........................");
    ALOGD("IDLE Player....");
    return NO_ERROR;
}
