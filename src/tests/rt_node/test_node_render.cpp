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
 *   date: 20190122
 */

#include "RTMediaData.h"
#include "RTSinkDisplay.h"        // NOLINT
#include "RTNodeSinkGLES.h"    // NOLINT
#include "RTMediaMetaKeys.h"      // NOLINT

UINT8* allocVideoBuf(INT32 width, INT32 height) {
    UINT8* pixels = reinterpret_cast<UINT8*>(rt_malloc_size(UINT8, width*height*3));
    RT_LOGE("pixels(%p) - %dx%d", pixels, width, width);
    return pixels;
}

UINT8* addVideNoise(UINT8* pixels, INT32 width, INT32 height) {
    if (pixels == NULL)
        return NULL;

    static INT32 factor = 0;
    for (INT32 y = 0; y < height; y++) {
        factor = (factor) % 20;
        for (INT32 x = 0; x < width; x++) {
            if (factor < 10) {
                pixels[(y * width + x) * 3 + 0] = 0;    // R
                pixels[(y * width + x) * 3 + 1] = 0;    // G
                pixels[(y * width + x) * 3 + 2] = 255;  // B
            } else {
                pixels[(y * width + x) * 3 + 0] = 0;
                pixels[(y * width + x) * 3 + 1] = 255;
                pixels[(y * width + x) * 3 + 2] = 0;
            }
        }
        factor = (factor > 100)?0:factor;
    }

    return pixels;
}

RT_RET unit_test_node_render_gles(INT32 index, INT32 total) {
    RT_RET         err       = RT_OK;
    UINT32         rt_count  = 0;
    RTFrame        rt_frame  = {0};
    RTMediaBuffer* media_buf = RT_NULL;
    RtMetaData*    node_meta = RT_NULL;
    UINT32         v_width   = 1280;
    UINT32         v_height  = 720;

    RT_LOGD("node_stub(ptr=%p)", &rt_sink_display_gles);

    node_meta = new RtMetaData();
    node_meta->setInt32(kKeyFrameW,   v_width);
    node_meta->setInt32(kKeyFrameH,   v_height);
    node_meta->setInt32(kKeyDisplayW, v_width);
    node_meta->setInt32(kKeyDisplayH, v_height);

    RTNodeStub* nodestub =  &rt_sink_display_gles;
    RTNode*       render =   nodestub->mCreateNode();

    RTNodeAdapter::init(render, node_meta);
    RTNodeAdapter::runCmd(render, RT_NODE_CMD_START, reinterpret_cast<RtMetaData *>(node_meta));

    /* faked random frame */
    rt_frame.mData = allocVideoBuf(v_width, v_height);
    rt_frame.mSize = v_width*v_height*3;
    rt_frame.mFrameW = rt_frame.mDisplayW = v_width;
    rt_frame.mFrameH = rt_frame.mDisplayH = v_height;
    media_buf = new RTMediaBuffer(rt_frame.mData, rt_frame.mSize);
    rt_mediabuf_from_frame(media_buf, &rt_frame);

    for (rt_count = 0; rt_count < 1000; rt_count++) {
        // change video buffer in frame
        addVideNoise(reinterpret_cast<UINT8*>(rt_frame.mData), v_width, v_height);
        // render frame
        do {
            err = render->pushBuffer(media_buf);
            RtTime::sleepMs(20);
        } while (RT_OK != err);
    }

    RTNodeAdapter::runCmd(render, RT_NODE_CMD_STOP, reinterpret_cast<RtMetaData *>(RT_NULL));
    RTNodeAdapter::release(render);

    delete render;
    render  =  RT_NULL;

    delete media_buf;
    media_buf =  RT_NULL;

    return RT_OK;
}

RT_RET unit_test_node_render_native(INT32 index, INT32 total) {
    return RT_OK;
}
