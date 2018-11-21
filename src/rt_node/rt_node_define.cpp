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
 *   date: 2018/11/20
 */

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "NodeDefine"

#include "rt_node_define.h" // NOLINT
#include "rt_header.h" // NOLINT

typedef struct rt_enum_name_t {
    UINT32      type;
    const char *name;
} rt_enum_name;

static const rt_enum_name g_node_type_names[] = {
    { RT_NODE_TYPE_BASE,      "Base" },
    { RT_NODE_TYPE_EXTRACTOR, "Extractor" },
    { RT_NODE_TYPE_DEMUXER,   "Demuxer" },
    { RT_NODE_TYPE_MUXER,     "Muxer" },
    { RT_NODE_TYPE_DECODER,   "Decoder" },
    { RT_NODE_TYPE_ENCODER,   "Encoder" },
    { RT_NODE_TYPE_FILTER,    "Filter" },
    { RT_NODE_TYPE_RENDER,    "Render" },
    { RT_NODE_TYPE_DEVICE,    "Device" },
};

static const rt_enum_name g_node_cmd_names[] = {
    { RT_NODE_CMD_BASE,        "BASE" },
    { RT_NODE_CMD_INIT,        "INIT" },
    { RT_NODE_CMD_PREPARE,     "PREPARE" },
    { RT_NODE_CMD_START,       "START" },
    { RT_NODE_CMD_STOP,        "STOP" },
    { RT_NODE_CMD_CAPS_CHANGE, "CAPS_CHANGE" },
    { RT_NODE_CMD_SEEK,        "SEEK" },
    { RT_NODE_CMD_RESET,       "RESET" },
    { RT_NODE_CMD_REINIT,      "REINIT" },
    { RT_NODE_CMD_NAVIGATION,  "NAVIGATION" },
    { RT_NODE_CMD_DRAIN,       "DRAIN" },

    // QOS and debug cmd
    { RT_NODE_CMD_LATENCY,    "LATENCY" },
    { RT_NODE_CMD_STAT,       "STAT" },
    { RT_NODE_CMD_QOS,        "QOS" },
    { RT_NODE_CMD_DUMP,       "DUMP" }
};

static const rt_enum_name g_node_msg_names[] = {
    { RT_NODE_MSG_BASE,       "BASE" },
    { RT_NODE_MSG_CAPS,       "CPAS" },
    { RT_NODE_MSG_SEGMENT,    "CPAS" },
    { RT_NODE_MSG_BUFFERSIZE, "BUFFERSIZE" },
    { RT_NODE_MSG_EOS,        "EOS" },
    { RT_NODE_MSG_SINK,       "SINK" },
    { RT_NODE_MSG_MAX,        "MAX" },
};

static const rt_enum_name g_node_event_names[] = {
    { RT_NODE_EVENT_BASE,     "BASE" },
    { RT_NODE_EVENT_QOS,      "QOS" },
    { RT_NODE_EVENT_ERROR,    "ERROR" },
    { RT_NODE_EVENT_HTTP_2XX, "HTTP_2XX" },
    { RT_NODE_EVENT_HTTP_3XX, "HTTP_3XX" },
    { RT_NODE_EVENT_HTTP_4XX, "HTTP_4XX" },
    { RT_NODE_EVENT_MAX,      "MAX" },
};

const char* rt_node_type_name(RT_NODE_TYPE id_type) {
    return g_node_type_names[(id_type - RT_NODE_TYPE_BASE)].name;
}

const char* rt_node_cmd_name(RT_NODE_CMD id_cmd) {
    return g_node_cmd_names[(id_cmd - RT_NODE_CMD_BASE)].name;
}

const char* rt_node_msg_name(RT_NODE_MSG id_msg) {
    return g_node_msg_names[(id_msg-RT_NODE_MSG_BASE)].name;
}

const char* rt_node_event_name(RT_NODE_EVENT id_event) {
    return g_node_event_names[(id_event - RT_NODE_EVENT_BASE)].name;
}
