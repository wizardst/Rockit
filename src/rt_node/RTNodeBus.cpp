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
 * Author: martin.cheng@rock-chips.com
 *   Date: 2018/11/03
 *   Task: construct and manage pipeline of media node
 */

#include "RTNodeBus.h"        // NOLINT
#include "RTNodeDemuxer.h"    // NOLINT
#include "RTNodeHeader.h"     // NOLINT
#include "FFNodeDecoder.h"    // NOLINT
#include "FFNodeEncoder.h"    // NOLINT
#include "FFNodeDemuxer.h"    // NOLINT
#include "HWNodeMpiDecoder.h" // NOLINT
#include "RTMediaMetaKeys.h"  // NOLINT

#define TEMP_WIDTH           608
#define TEMP_HEIGHT          1080

#define DEC_TEST_FILE    "dec_output.bin"
#define ENC_TEST_FILE    "enc_output.bin"

#ifdef OS_WINDOWS
#define TEMP_URI "E:\\CloudSync\\low-used\\videos\\h264-1080p.mp4"
#else
#define TEMP_URI "airplay.mp4"
#endif

struct NodeBusContext {
    RtArrayList *node_bus;
    RtHashTable *node_all;
} NodeBusContext;

UINT32  node_hash_func(UINT32 bucktes, const void *key) {
    void *tmp_key = const_cast<void *>(key);
    return *(reinterpret_cast<UINT32 *>(tmp_key)) % (bucktes);
}

// ! life cycles of node_bus
struct NodeBusContext* rt_node_bus_create() {
    struct NodeBusContext* bus = rt_malloc(struct NodeBusContext);
    bus->node_bus = array_list_create();
    bus->node_all = rt_hash_table_create((RT_NODE_TYPE_MAX - RT_NODE_TYPE_BASE),
                                       hash_ptr_func, hash_ptr_compare);
    return bus;
}

INT32 rt_node_bus_destory(struct NodeBusContext* bus) {
    array_list_destroy(bus->node_bus);
    rt_hash_table_destory(bus->node_all);

    RT_ASSERT(RT_NULL == bus->node_bus);
    RT_ASSERT(RT_NULL == bus->node_all);

    rt_safe_free(bus);

    return RT_OK;
}

INT32 rt_node_bus_build(struct NodeBusContext* bus) {
    return RT_OK;
}

INT32 rt_node_bus_summary(struct NodeBusContext* bus, RT_BOOL full) {
    RT_LOGE("bus = %p full =%d", bus, full);
    // rt_hash_table_dump(bus->node_all);

    UINT32 num_buckets = rt_hash_table_get_num_buckets(bus->node_all);
    for (UINT32 idx = 0; idx < num_buckets; idx++) {
        struct rt_hash_node* node = RT_NULL;
        UINT32 num_plugin = 0;
        struct rt_hash_node* root = rt_hash_table_get_bucket(bus->node_all, idx);
        for (node = root->next; node != root; node = node->next, num_plugin++) {
            RTNode* plugin = reinterpret_cast<RTNode*>(node->data);
            /*
            RT_LOGE("buckets[%02d:%02d]: name:%-10s, ptr:%p",
                     idx, num_plugin, plugin->queryInfo()->mNodeName, node->data);*/
        }
    }
    RT_LOGE("done\r\n");
    return RT_OK;
}

INT32 rt_node_bus_register_all(struct NodeBusContext* bus) {
    #ifdef RK_HW_CODEC
    rt_node_bus_register(bus, &hw_node_mpi_decoder);
    #endif
    rt_node_bus_register(bus, &ff_node_demuxer);
    rt_node_bus_register(bus, &ff_node_video_decoder);
    rt_node_bus_register(bus, &ff_node_video_encoder);
    return RT_OK;
}

INT32 rt_node_bus_register(struct NodeBusContext* bus, RTNodeStub* node_info) {
    INT32 node_type = node_info->mNodeType;
    rt_hash_table_insert(bus->node_all, reinterpret_cast<void*>(node_type), node_info);
    return RT_OK;
}

RTNodeStub* rt_node_bus_find(struct NodeBusContext* bus, RT_NODE_TYPE node_type, UINT8 node_id) {
    void* data = rt_hash_table_find(bus->node_all,
                     reinterpret_cast<void *>(node_type));
    if (RT_NULL != data) {
        return reinterpret_cast<RTNodeStub*>(data);
    }
    return RT_NULL;
}

INT32 rt_plugin_autobuild(struct NodeBusContext* bus) {
    RT_ASSERT(bus != NULL);
    rt_node_bus_build(bus);
    UINT32 num_buckets = rt_hash_table_get_num_buckets(bus->node_all);
    RT_LOGE("rt_node_bus_autobuild() and the num is %d", num_buckets);
    rt_hash_table_dump(bus->node_all);
    return RT_OK;
}

RT_RET rt_plugin_init(struct NodeBusContext* bus, RtMetaData *nodeMeta) {
    RT_ASSERT(bus != NULL && nodeMeta!= NULL);
    UINT32 num_buckets = rt_hash_table_get_num_buckets(bus->node_all);
    struct rt_hash_node* node = RT_NULL;
    INT32 idx = 0;
    RT_NODE_TYPE node_type = RT_NODE_TYPE_EXTRACTOR;
    while (node_type < RT_NODE_TYPE_MAX && idx < num_buckets) {
        RTNodeStub* stub     = rt_node_bus_find(bus, node_type , 0);
        if (stub != NULL) {
            RTNode*     node     = stub->mCreateNode();

            if (node_type == RT_NODE_TYPE_DEMUXER) {
                nodeMeta->setCString(kKeyFormatUri, TEMP_URI);
            }
            if (node_type == RT_NODE_TYPE_DECODER) {
                nodeMeta->setInt32(kKeyVCodecWidth,  TEMP_WIDTH);
                nodeMeta->setInt32(kKeyVCodecHeight, TEMP_HEIGHT);
                nodeMeta->setInt32(kKeyCodecID,      RT_VIDEO_CodingAVC);
            }
            if (node_type == RT_NODE_TYPE_ENCODER) {
                nodeMeta->setInt32(kKeyVCodecWidth,  TEMP_WIDTH);
                nodeMeta->setInt32(kKeyVCodecHeight, TEMP_HEIGHT);
                nodeMeta->setInt32(kKeyCodecID,      RT_VIDEO_CodingMPEG4);
                nodeMeta->setInt32(kKeyCodecBitrate, 5000000);
            }
            RTNodeAdapter::init(node, nodeMeta);
        }
        node_type = (RT_NODE_TYPE)(node_type + 1);
        idx++;
    }
        return RT_OK;
}

RT_RET rt_plugin_pull(struct NodeBusContext* bus, RT_NODE_TYPE node_type, RTMediaBuffer* media_buf) {
    RT_ASSERT(bus != NULL);
    UINT32 num_buckets = rt_hash_table_get_num_buckets(bus->node_all);

    RTNode* node;
    INT32 idx = 0;
    RTPacket       rt_pkt    = {0};

    void* data = rt_hash_table_find(bus->node_all, reinterpret_cast<void *>(node_type));
    if (RT_NULL != data) {
        node = reinterpret_cast<RTNode*>(data);
    }
    if (node != NULL) {
        if (node_type == RT_NODE_TYPE_DEMUXER) {
            media_buf = new RTMediaBuffer(RT_NULL, 0);
            media_buf->reset();
            RTNodeAdapter::pullBuffer(node, &media_buf);
            if (media_buf->getSize() > 0) {
                rt_mediabuf_from_packet(media_buf, &rt_pkt);
                rt_utils_packet_free(&rt_pkt);
            }
        } else {
            RTNodeAdapter::pullBuffer(node, &media_buf);
        }
    }

    return RT_OK;
}


RT_RET rt_plugin_push(struct NodeBusContext* bus, RT_NODE_TYPE node_type, RTMediaBuffer* media_buf) {
    RT_ASSERT(bus != NULL);
    UINT32 num_buckets = rt_hash_table_get_num_buckets(bus->node_all);

    INT32 idx = 0;
    RTNode* node;

    void* data = rt_hash_table_find(bus->node_all, reinterpret_cast<void *>(node_type));
    if (RT_NULL != data) {
        node = reinterpret_cast<RTNode*>(data);
    }
    if (node != NULL && (node_type == RT_NODE_TYPE_DECODER || node_type == RT_NODE_TYPE_ENCODER)) {
        RTNodeAdapter::pushBuffer(node, media_buf);
    }

    return RT_OK;
}

INT32 rt_plugin_run_cmd(struct NodeBusContext* bus, RT_NODE_CMD node_cmd) {
    RT_ASSERT(bus != NULL);
    UINT32 num_buckets = rt_hash_table_get_num_buckets(bus->node_all);
    RT_NODE_TYPE node_type = RT_NODE_TYPE_EXTRACTOR;

    INT32 idx = 0;
    RTNode* node;
    while (node_type < RT_NODE_TYPE_MAX && idx < num_buckets) {
        void* data = rt_hash_table_find(bus->node_all, reinterpret_cast<void *>(node_type));
        if (RT_NULL != data) {
            node = reinterpret_cast<RTNode*>(data);
        }
        if (node != NULL) {
            RTNodeAdapter::runCmd(node, node_cmd, NULL);
        }
        node_type = (RT_NODE_TYPE)(node_type + 1);
        idx++;
    }
    return RT_OK;
}

INT32 rt_plugin_release(struct NodeBusContext* bus, RT_NODE_TYPE node_type) {
    RT_ASSERT(bus != NULL);
    UINT32 num_buckets = rt_hash_table_get_num_buckets(bus->node_all);

    INT32 idx = 0;
    RTNode* node;

    void* data = rt_hash_table_find(bus->node_all, reinterpret_cast<void *>(node_type));
    if (RT_NULL != data) {
        node = reinterpret_cast<RTNode*>(data);
    }
    if (node != NULL) {
        RTNodeAdapter::release(node);
    }

    return RT_OK;
}

RT_RET unit_test_rt_plugin_manage_test(RT_NODE_TYPE node_type) {
    RT_RET ret = RT_OK;
    RTMediaBuffer *frame = RT_NULL;
    RTMediaBuffer *pkt = RT_NULL;
    struct NodeBusContext* bus = rt_node_bus_create();
    rt_node_bus_register_all(bus);
    rt_plugin_autobuild(bus);
    RtMetaData *nodeMeta = new RtMetaData();
    rt_plugin_init(bus, nodeMeta);
    rt_plugin_run_cmd(bus, RT_NODE_CMD_START);
    RTNode* node;
    RT_RET err = RT_OK;

    FILE *write_fd = fopen(ENC_TEST_FILE, "wb");
    if (!write_fd) {
        RT_LOGE("%s open failed!!", ENC_TEST_FILE);
        return RT_ERR_UNKNOWN;
    }

    FILE *read_fd = fopen(DEC_TEST_FILE, "rb");
    if (!read_fd) {
        RT_LOGE("%s open failed!!", DEC_TEST_FILE);
        return RT_ERR_UNKNOWN;
    }

    switch (node_type) {
        case RT_NODE_TYPE_ENCODER: {
            void* data = rt_hash_table_find(bus->node_all, reinterpret_cast<void *>(node_type));
            if (RT_NULL != data) {
                node = reinterpret_cast<RTNode*>(data);
            }
            do {
                // deqeue buffer from object pool
                if (!frame) {
                    err = RTNodeAdapter::dequeCodecBuffer(node, &frame, RT_PORT_INPUT);
                }
                if (err == RT_OK && frame) {
                    if (fread(frame->getData(), TEMP_HEIGHT * TEMP_WIDTH * 3 / 2, 1, read_fd) < 1) {
                        RT_LOGD("read eof, break");
                        break;
                    }
                    fflush(read_fd);
                    if (rt_plugin_push(bus, node_type, frame) == RT_OK) {
                        frame = NULL;
                    }
                    err = rt_plugin_pull(bus, node_type, pkt);
                    if (err == RT_OK && pkt) {
                        RT_LOGD("encode complete pkt data: %p length: %d",
                                     pkt->getData(), pkt->getLength());
                        fwrite(pkt->getData(), pkt->getLength(), 1, write_fd);
                        fflush(write_fd);
                        RTNodeAdapter::queueCodecBuffer(node, pkt, RT_PORT_OUTPUT);
                        pkt = NULL;
                    }
                }
                RtTime::sleepMs(50);
            } while (true);
            RT_LOGD("encoder complet..");
            break;
        }
        case RT_NODE_TYPE_DEMUXER:
            rt_plugin_pull(bus, node_type, pkt);
            break;

        case RT_NODE_TYPE_DECODER: {
            void* data = rt_hash_table_find(bus->node_all, reinterpret_cast<void *>(node_type));
            if (RT_NULL != data) {
                node = reinterpret_cast<RTNode*>(data);
            }
            do {
                RTNodeAdapter::dequeCodecBuffer(node, &pkt, RT_PORT_INPUT);
                if (RT_NULL != pkt) {
                    rt_plugin_pull(bus, node_type, pkt);

                    UINT8 *data = reinterpret_cast<UINT8 *>(pkt->getData());
                    UINT32 size = pkt->getSize();
                    RT_LOGD("data: %p size: %d", data, size);
                    if (!data) {
                        RT_LOGD("check eos ,break..");
                        break;
                    }
                    while (size > 0) {
                        UINT32 nal_length = (data[0] << 24) + (data[1] << 16) + (data[2] << 8) + data[3];
                        data[0] = 0;
                        data[1] = 0;
                        data[2] = 0;
                        data[3] = 1;
                        data += nal_length + 4;
                        size -= nal_length + 4;
                    }
                }
                rt_plugin_push(bus, node_type, pkt);

                rt_plugin_pull(bus, node_type, frame);

                if (frame) {
                    RT_LOGD("dec complete frame data: %p length: %d", frame->getData(), frame->getLength());
                    fwrite(frame->getData(), frame->getLength(), 1, write_fd);
                    fflush(write_fd);
                    RTNodeAdapter::queueCodecBuffer(node, frame, RT_PORT_OUTPUT);
                    frame = NULL;
                }
                RtTime::sleepMs(50);
            } while (true);
        }
        break;
        default:
            break;
    }

    if (nodeMeta) {
        delete nodeMeta;
        nodeMeta = NULL;
    }
    rt_plugin_run_cmd(bus, RT_NODE_CMD_STOP);
    rt_plugin_release(bus, node_type);
    rt_node_bus_destory(bus);
    return ret;
}

