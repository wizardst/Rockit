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
 * module: RTMetadataRetriever impliments android MediaMetadataRetrieverInterface
 */

#ifndef SRC_RT_PLAYER_PLAYER_OSAL_ANDROID_RTMETADATARETRIEVER_H_
#define SRC_RT_PLAYER_PLAYER_OSAL_ANDROID_RTMETADATARETRIEVER_H_

#include <utils/KeyedVector.h>
#include <media/MediaMetadataRetrieverInterface.h>
#include <media/IMediaHTTPService.h>

using namespace android;  // NOLINT

class RTMetadataRetrieverImpl;
class RTMetadataRetriever: public MediaMetadataRetrieverInterface {
 public:
    RTMetadataRetriever();
    virtual ~RTMetadataRetriever();
    virtual status_t       setDataSource(const sp<IMediaHTTPService> &httpService,
                                         const char *url, const KeyedVector<String8, String8> *headers);
    virtual status_t       setDataSource(int fd, int64_t offset, int64_t length);
    virtual VideoFrame*    getFrameAtTime(int64_t timeUs, int option);
    virtual MediaAlbumArt* extractAlbumArt();
    virtual const char*    extractMetadata(int keyCode);

 private:
    RTMetadataRetrieverImpl* mRetrieverImpl;
};

#endif  // SRC_RT_PLAYER_PLAYER_OSAL_ANDROID_RTMETADATARETRIEVER_H_
