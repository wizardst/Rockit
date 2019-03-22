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
 * module: RTNDKIPTVPlayer
 */

#include <utils/Log.h>          // NOLINT
#include <media/Metadata.h>     // NOLINT
#include <media/stagefright/MediaExtractor.h>

#include "RTMediaPlayer.h"      // NOLINT
#include "RTMediaPlayerImpl.h"  // NOLINT

// #define LOG_NDEBUG 0
#define LOG_TAG "RTMediaPlayer"

RTMediaPlayer::RTMediaPlayer()
    : mPlayer(new RTMediaPlayerImpl) {
    ALOGE("+Constructor %p", mPlayer);
    // mPlayer->setListener(this);
}

RTMediaPlayer::~RTMediaPlayer() {
    ALOGE("~Destructor %p", mPlayer);
    delete mPlayer;
    mPlayer = NULL;
}

status_t RTMediaPlayer::initCheck() {
    ALOGD("initCheck, @TODO");
    return NO_ERROR;
}

status_t RTMediaPlayer::setUID(uid_t uid) {
    ALOGD("setUID, @TODO");
    return NO_ERROR;
}

status_t RTMediaPlayer::setDataSource(const sp<IMediaHTTPService> &httpService,
           const char *url, const KeyedVector<String8, String8> *headers) {
    ALOGV("setDataSource, uri = %128.128s", url);
    return mPlayer->setDataSource(httpService, url, headers);
}

// Warning: The filedescriptor passed into this method will only be valid until
// the method returns, if you want to keep it, dup it!
status_t RTMediaPlayer::setDataSource(int fd, int64_t offset, int64_t length) {
    ALOGV("setDataSource(%d, %lld, %lld)", fd, offset, length);
    return mPlayer->setDataSource(dup(fd), offset, length);
}

status_t RTMediaPlayer::setDataSource(const sp<IStreamSource> &source) {
    ALOGV("setDataSource with source");
    return NO_ERROR;
}

status_t RTMediaPlayer::setVideoSurfaceTexture(
        const sp<IGraphicBufferProducer> &bufferProducer) {
    ALOGV("setVideoSurfaceTexture");
    return mPlayer->setVideoSurfaceTexture(bufferProducer);
}

status_t RTMediaPlayer::prepare() {
    ALOGV("prepare");
    return mPlayer->prepare();
}

status_t RTMediaPlayer::prepareAsync() {
    ALOGV("prepareAsync");
    return mPlayer->prepareAsync();
}

status_t RTMediaPlayer::start() {
    ALOGV("start");
    return mPlayer->start();
}

status_t RTMediaPlayer::stop() {
    ALOGV("stop");
    return mPlayer->stop();
}

status_t RTMediaPlayer::pause() {
    ALOGV("pause");
    return mPlayer->pause();
}

bool RTMediaPlayer::isPlaying() {
    ALOGV("isPlaying");
    return mPlayer->isPlaying();
}

status_t RTMediaPlayer::seekTo(int msec) {
    ALOGV("seekTo %.2f secs", msec / 1E3);

    return mPlayer->seekTo(msec);
}

status_t RTMediaPlayer::getCurrentPosition(int *msec) {
    ALOGV("getCurrentPosition");

    return mPlayer->getCurrentPosition(msec);
}

status_t RTMediaPlayer::getDuration(int *msec) {
    ALOGV("getDuration");

    return mPlayer->getDuration(msec);
}

status_t RTMediaPlayer::reset() {
    ALOGV("reset");

    return mPlayer->reset();
}

status_t RTMediaPlayer::setLooping(int loop) {
    ALOGV("setLooping");
    return mPlayer->setLooping(loop);
}

player_type RTMediaPlayer::playerType() {
    ALOGV("playerType");
    return FF_PLAYER;
}

status_t RTMediaPlayer::invoke(const Parcel &request, Parcel *reply) {
    ALOGV("invoke()");
    return mPlayer->invoke(request, reply);
}

void RTMediaPlayer::setAudioSink(const sp<AudioSink> &audioSink) {
    MediaPlayerInterface::setAudioSink(audioSink);
    mPlayer->setAudioSink(audioSink);
}

status_t RTMediaPlayer::setParameter(int key, const Parcel &request) {
    ALOGV("setParameter(key=%d)", key);

    return mPlayer->setParameter(key, request);
}

status_t RTMediaPlayer::getParameter(int key, Parcel *reply) {
    ALOGV("getParameter");
    return mPlayer->getParameter(key, reply);
}

status_t RTMediaPlayer::getMetadata(
        const media::Metadata::Filter& ids, Parcel *records) {
        ALOGV("getMetadata");
    using media::Metadata;

    uint32_t flags = mPlayer->flags();

    Metadata metadata(records);

    metadata.appendBool(
            Metadata::kPauseAvailable,
            flags & MediaExtractor::CAN_PAUSE);

    metadata.appendBool(
            Metadata::kSeekBackwardAvailable,
            flags & MediaExtractor::CAN_SEEK_BACKWARD);

    metadata.appendBool(
            Metadata::kSeekForwardAvailable,
            flags & MediaExtractor::CAN_SEEK_FORWARD);

    metadata.appendBool(
            Metadata::kSeekAvailable,
            flags & MediaExtractor::CAN_SEEK);

    return NO_ERROR;
}

status_t RTMediaPlayer::dump(int fd, const Vector<String16> &args) const {
    return mPlayer->dump(fd, args);
}
