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
 * author: rimon.xu@rock-chips.com
 *   date: 2019/01/16
 * module: allocator test
 */

#include "rt_media_tests.h"         // NOLINT
#include "RTAllocatorStore.h"       // NOLINT
#include "RTAllocatorBase.h"        // NOLINT
#include "RTMediaBuffer.h"          // NOLINT
#include "rt_metadata.h"            // NOLINT
#include "rt_header.h"              // NOLINT

#define TEST_BUFFER_SIZE            1 * 1024 * 1024

RT_RET unit_test_allocator(INT32 index, INT32 total_index) {
    (void)index;
    (void)total_index;

    RT_RET ret = RT_OK;
    RTAllocator *allocator = NULL;
    RTAllocatorStore *allocator_store = new RTAllocatorStore();
    RTMediaBuffer *buffer[16];
    UINT8 *data[16];
    RtMetaData *config = new RtMetaData();

    /*
     * drm allocator test
     */
    do {
        allocator_store->fetchAllocator(RTAllocatorStore::RT_ALLOC_TYPE_DRM,
                                        config,
                                        &allocator);
        if (!allocator) {
            RT_LOGE("drm allocator fetch failed");
            break;
        }

        {
            /*
             * test for drm buffer avail
             */
            INT32 i = 0, j = 0;
            for (i = 0; i < 16; i++) {
                allocator->newBuffer(TEST_BUFFER_SIZE, &(buffer[i]));
                data[i] = reinterpret_cast<UINT8 *>((buffer[i])->getData());
            }

            UINT8 *data_tmp = rt_malloc_size(UINT8, TEST_BUFFER_SIZE);

            for (i = 0; i < 256; i++) {
                data_tmp[i] = i;
            }
            for (i = 0; i < 16; i++) {
                rt_memcpy(data[i], data_tmp, TEST_BUFFER_SIZE);
                for (j = 0; j < 256; j++) {
                    CHECK_EQ(data[i][j], j);
                }
            }

            for (i = 0; i < 16; i++) {
                allocator->freeBuffer(&(buffer[i]));
            }

            if (data_tmp) {
                rt_free(data_tmp);
                data_tmp = NULL;
            }
        }
    } while (0);

__FAILED:
    if (config) {
        delete (config);
        config = NULL;
    }

    return ret;
}

