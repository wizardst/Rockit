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
 *   date: 2019/03/08
 */

#include "rt_header.h"          // NOLINT
#include "rt_media_tests.h"     // NOLINT
#include "RTMediaBufferPool.h"  // NOLINT
#include "RTAllocatorBase.h"    // NOLINT
#include "RTAllocatorStore.h"   // NOLINT
#include "rt_metadata.h"        // NOLINT
#include "RTMediaBuffer.h"      // NOLINT
#include "rt_thread.h"          // NOLINT

#define TEST_BUFFER_SIZE        1 * 1024 * 1024
#define TEST_BUFFER_COUNT       32

void *test_loop(void* param) {
    RTMediaBufferPool *pool = reinterpret_cast<RTMediaBufferPool *>(param);
    RTMediaBuffer *out = RT_NULL;
    while (!pool->hasBuffer()) {
        continue;
    }
    pool->acquireBuffer(&out, RT_TRUE);
    RT_LOGD("get buffer: %p", out);
    out->release();
    return RT_NULL;
}

RT_RET unit_test_mediabuffer_pool(INT32 index, INT32 total_index) {
    (void)index;
    (void)total_index;

    RT_RET               ret = RT_OK;
    RTAllocator         *allocator = NULL;
    RTAllocatorStore    *allocator_store = new RTAllocatorStore();
    RTMediaBuffer       *buffer[TEST_BUFFER_COUNT];
    RTMediaBuffer       *pool_buffer[TEST_BUFFER_COUNT];
    UINT8               *data[TEST_BUFFER_COUNT];
    RtMetaData          *config = new RtMetaData();
    RTMediaBufferPool   *pool = new RTMediaBufferPool(TEST_BUFFER_COUNT, TEST_BUFFER_SIZE);
    RTMediaBuffer       *tmp = new RTMediaBuffer(1024);
    RtThread            *thread;
    /*
     * media buffer pool test, buffer from allocator.
     */
    do {
        const char* name = "mediabuffer_pool_test";
        thread = new RtThread(test_loop, pool);
        thread->setName(name);

        allocator_store->fetchAllocator(RTAllocatorStore::RT_ALLOC_TYPE_DRM,
                                        config,
                                        &allocator);
        if (!allocator) {
            allocator_store->priorAvailLinearAllocator(config, &allocator);
            if (!allocator) {
                ret = RT_ERR_UNKNOWN;
                break;
            }
        }

        // malloc buffer from allocator
        INT32 i = 0;
        RTMediaBuffer *out = RT_NULL;
        for (i = 0; i < TEST_BUFFER_COUNT; i++) {
            allocator->newBuffer(TEST_BUFFER_SIZE, &(buffer[i]));
            data[i] = reinterpret_cast<UINT8 *>((buffer[i])->getData());
            data[i][0] = i;
        }

        CHECK_EQ(pool->acquireBuffer(&out, RT_TRUE), RT_ERR_LIST_EMPTY);

        // register buffer to mediabuffer pool
        for (i = 0; i < TEST_BUFFER_COUNT; i++) {
            CHECK_EQ(pool->registerBuffer(buffer[i]), RT_OK);
        }

        CHECK_EQ(pool->registerBuffer(tmp), RT_ERR_LIST_FULL);

        pool->acquireBuffer(&out, RT_FALSE, TEST_BUFFER_SIZE + 1);
        CHECK_EQ(out, RT_NULL);

        for (i = 0; i < TEST_BUFFER_COUNT; i++) {
            // RT_LOGE("acquireBuffer index: %d", i);
            pool->acquireBuffer(&(pool_buffer[i]), RT_FALSE);
            CHECK_UE(pool_buffer[i], RT_NULL);
        }
        pool->acquireBuffer(&out, RT_FALSE);
        CHECK_EQ(out, RT_NULL);

        thread->start();

        RT_LOGD("sleep 500 ms for wait buffer..");
        RtTime::sleepMs(500);

        pool_buffer[0]->release();

        thread->join();

        pool->acquireBuffer(&out, RT_TRUE);
        CHECK_UE(out, RT_NULL);
        out->release();

        for (i = 0; i < TEST_BUFFER_COUNT; i++) {
            pool_buffer[i]->release();
        }

        pool->releaseAllBuffers();

        pool->acquireBuffer(&out, RT_TRUE);
    } while (0);

__FAILED:
    rt_safe_delete(config);
    rt_safe_delete(pool);
    rt_safe_delete(tmp);
    rt_safe_delete(allocator);
    rt_safe_delete(allocator_store);
    rt_safe_delete(thread);

    return ret;
}

