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
 *   date: 20180802
 */

#ifndef SRC_RT_NODE_INCLUDE_RT_NODE_DEFINE_H_
#define SRC_RT_NODE_INCLUDE_RT_NODE_DEFINE_H_

/**
 * \file
 * This file implements node (module) macros used to define a rt_node.
 */

typedef enum _RT_NODE_TYPE {
    // ! basic nodes
    RT_NODE_TYPE_BASE   = 1000,
    RT_NODE_TYPE_EXTRACTOR,
    RT_NODE_TYPE_DEMUXER,
    RT_NODE_TYPE_MUXER,
    RT_NODE_TYPE_DECODER,
    RT_NODE_TYPE_ENCODER,
    RT_NODE_TYPE_FILTER,
    RT_NODE_TYPE_SINK,
    RT_NODE_TYPE_DEVICE,

    // !
    RT_NODE_TYPE_MAX,
} RT_NODE_TYPE;

typedef enum _RT_NODE_CMD {
    RT_NODE_CMD_BASE  = 2000,
    RT_NODE_CMD_INIT,
    RT_NODE_CMD_PREPARE,
    RT_NODE_CMD_START,
    RT_NODE_CMD_STOP,
    RT_NODE_CMD_CAPS_CHANGE,
    RT_NODE_CMD_SEEK,
    RT_NODE_CMD_RESET,
    RT_NODE_CMD_REINIT,
    RT_NODE_CMD_FLUSH,
    RT_NODE_CMD_PAUSE,
    RT_NODE_CMD_NAVIGATION,
    RT_NODE_CMD_DRAIN,

    // QOS and debug cmd
    RT_NODE_CMD_LATENCY,
    RT_NODE_CMD_STAT,
    RT_NODE_CMD_QOS,
    RT_NODE_CMD_DUMP,
} RT_NODE_CMD;

typedef enum _RT_NODE_MSG {
    RT_NODE_MSG_BASE = 3000,
    RT_NODE_MSG_CAPS,
    RT_NODE_MSG_SEGMENT,
    RT_NODE_MSG_BUFFERSIZE,
    RT_NODE_MSG_EOS,
    RT_NODE_MSG_SINK,
    RT_NODE_MSG_MAX,
} RT_NODE_MSG;

typedef enum _RT_NODE_EVENT {
    RT_NODE_EVENT_BASE = 4000,
    RT_NODE_EVENT_QOS,
    RT_NODE_EVENT_ERROR,
    RT_NODE_EVENT_HTTP_2XX,
    RT_NODE_EVENT_HTTP_3XX,
    RT_NODE_EVENT_HTTP_4XX,
    RT_NODE_EVENT_MAX,
} RT_NODE_EVENT;

const char* rt_node_type_name(RT_NODE_TYPE id_type);
const char* rt_node_cmd_name(RT_NODE_CMD id_cmd);
const char* rt_node_msg_name(RT_NODE_MSG id_msg);
const char* rt_node_event_name(RT_NODE_EVENT id_event);

/**
 * Current plugin ABI version
 */
#define MODULE_SYMBOL 1.0.0

#endif  // SRC_RT_NODE_INCLUDE_RT_NODE_DEFINE_H_
