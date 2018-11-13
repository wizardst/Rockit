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
 *   date: 2018/11/05
 */

#include "rt_node.h" // NOLINT
#include "rt_node_header.h" // NOLINT
#include "rt_node_tests.h" // NOLINT

RT_RET unit_test_demuxer() {
    RT_LOGE("%s ptr = %p", __FUNCTION__, &ff_node_parser);
    RT_Node* demuxer =  &ff_node_parser;
    demuxer->init();

    void *data   = RT_NULL;
    UINT32 *size = 0;
    demuxer->pull(data, size);
    demuxer->push(data, size);
    demuxer->release();
    return RT_OK;
}

RT_RET unit_test_muxer() {
    RT_LOGE("%s ptr = %p", __FUNCTION__, &ff_node_parser);
    RT_Node* muxer =  &ff_node_parser;
    muxer->init();

    void *data   = RT_NULL;
    UINT32 *size = 0;
    muxer->pull(data, size);
    muxer->push(data, size);
    muxer->release();
    return RT_OK;
}

RT_RET unit_test_node_basic(INT32 index, INT32 total) {
    unit_test_demuxer();
    return unit_test_muxer();
}


