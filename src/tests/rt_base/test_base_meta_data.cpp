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
 * author: rimon.xu@rock-chips.com
 *   date: 20181031
 */

#include <string.h>
#include "rt_base_tests.h" // NOLINT
#include "rt_metadata.h" // NOLINT

enum {
    kKeyTestInt32   = MKTAG('t', 'i', '3', '2'),
    kKeyTestInt64   = MKTAG('t', 'i', '6', '4'),
    kKeyTestString  = MKTAG('t', 's', 't', 'r'),
    kKeyTestFloat   = MKTAG('t', 'f', 'l', 't'),
    kKeyTestPtr     = MKTAG('t', 'p', 't', 'r'),
    kKeyTestNone    = MKTAG('t', 'n', 'o', 'e'),
};

enum {
    kKeyCodecBitrates     = MKTAG('c', 'b', 'i', 't'),     // INT64
    kKeyCodecCLayouts     = MKTAG('a', 'c', 'l', 'a'),     // INT64
    kKeyCodecChannels     = MKTAG('a', 'c', 'h', 'a'),
    kKeyCodecSampleRate   = MKTAG('a', 's', 'r', 'a'),
    kKeyCodecPointer      = MKTAG('a', 'p', 't', 'r'),
};

struct TrackInfo {
    INT64 mChannelLayout;
    INT32 mChannels;
    INT64 mBitrate;
    INT32 mSampleRate;
    void* mThis;
};

RT_RET unit_test_metadata_more(INT32 index, INT32 total_index) {
    struct TrackInfo track_info;
    RtMetaData*  metadata   = new RtMetaData();
    rt_memset(&track_info, 0, sizeof(struct TrackInfo));
    track_info.mBitrate       = 20000000;
    track_info.mChannelLayout = 2000;
    track_info.mChannels       = 2;
    track_info.mSampleRate    = 44100;
    track_info.mThis          = &track_info;

    metadata->setInt64(kKeyCodecCLayouts,   track_info.mChannelLayout);
    metadata->setInt64(kKeyCodecBitrates,   track_info.mBitrate);
    metadata->setInt32(kKeyCodecChannels,   track_info.mChannels);
    metadata->setInt32(kKeyCodecSampleRate, track_info.mSampleRate);
    metadata->setPointer(kKeyCodecPointer,  track_info.mThis);

    metadata->findInt64(kKeyCodecCLayouts,   &(track_info.mChannelLayout));
    metadata->findInt64(kKeyCodecBitrates,   &(track_info.mBitrate));
    metadata->findInt32(kKeyCodecChannels,   &(track_info.mChannels));
    metadata->findInt32(kKeyCodecSampleRate, &(track_info.mSampleRate));
    metadata->findPointer(kKeyCodecPointer,  &(track_info.mThis));

    struct TrackInfo* binder = (struct TrackInfo*)(track_info.mThis);
    RT_LOGD("bitrate=%lld, layouts=%lld, channels=%d, sample_rate=%d",
             binder->mBitrate,  binder->mChannelLayout,
             binder->mChannels, binder->mSampleRate);

    delete metadata;
    metadata = NULL;

    return RT_OK;
}

RT_RET unit_test_metadata(INT32 index, INT32 total_index) {
    RtMetaData *metadata = NULL;

    metadata = new RtMetaData();

    {
        /********* base test *************/
        INT32 test1;
        INT64 test2;
        float test3;
        const char *test4;
        void *test5;
        metadata->setInt32(kKeyTestInt32, 123);
        metadata->setInt64(kKeyTestInt64, 123456ll);
        metadata->setCString(kKeyTestString, "string");
        metadata->setFloat(kKeyTestFloat, 1.23f);
        metadata->setPointer(kKeyTestPtr, metadata);

        CHECK_EQ(metadata->findInt32(kKeyTestInt32, &test1), RT_TRUE);
        CHECK_EQ(metadata->findInt64(kKeyTestInt64, &test2), RT_TRUE);
        CHECK_EQ(metadata->findFloat(kKeyTestFloat, &test3), RT_TRUE);
        CHECK_EQ(metadata->findCString(kKeyTestString, &test4), RT_TRUE);
        CHECK_EQ(metadata->findPointer(kKeyTestPtr, &test5), RT_TRUE);

        CHECK_EQ(test1, 123);
        CHECK_EQ(test2, 123456ll);
        CHECK_EQ(test3, 1.23f);
        CHECK_EQ(strncmp(test4, "string", 5), 0);
        CHECK_EQ(test5, metadata);
        /********** end ***************/
    }

    {
        /********** return test ********/
        INT32 test1 = 0;
        CHECK_EQ(metadata->findInt32(kKeyTestNone, &test1), RT_FALSE);
        CHECK_EQ(metadata->hasData(kKeyTestNone), RT_FALSE);
        CHECK_EQ(metadata->setInt32(kKeyTestNone, 1), RT_FALSE);
        CHECK_EQ(metadata->hasData(kKeyTestNone), RT_TRUE);
        CHECK_EQ(metadata->setInt32(kKeyTestNone, 2), RT_TRUE);
        CHECK_EQ(metadata->setInt32(kKeyTestNone, 3), RT_TRUE);
        CHECK_EQ(metadata->setInt32(kKeyTestNone, 4), RT_TRUE);
        CHECK_EQ(metadata->hasData(kKeyTestNone), RT_TRUE);
        CHECK_EQ(metadata->findInt32(kKeyTestNone, &test1), RT_TRUE);
        CHECK_EQ(test1, 4);
        /********** end ***************/
    }

    {
        /********* remove test ********/
        INT32 test1 = 0;
        CHECK_EQ(metadata->remove(kKeyTestNone), RT_TRUE);
        CHECK_EQ(metadata->findInt32(kKeyTestNone, &test1), RT_FALSE);
        /********** end ***************/
    }

    {
        /********* clear test *********/
        metadata->clear();
        CHECK_EQ(metadata->hasData(kKeyTestInt32), RT_FALSE);
        CHECK_EQ(metadata->hasData(kKeyTestInt64), RT_FALSE);
        CHECK_EQ(metadata->hasData(kKeyTestFloat), RT_FALSE);
        CHECK_EQ(metadata->hasData(kKeyTestString), RT_FALSE);
        CHECK_EQ(metadata->hasData(kKeyTestPtr), RT_FALSE);
        CHECK_EQ(metadata->hasData(kKeyTestNone), RT_FALSE);
        /********** end ***************/
    }

    delete metadata;
    metadata = NULL;

    return RT_OK;
__FAILED:
    delete metadata;
    metadata = NULL;
    return RT_ERR_UNKNOWN;
}

