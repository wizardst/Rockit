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
 *   Task: stable api for media node
 */

#include "rt_header.h" // NOLINT

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "RT_Node"

#define DEBUG_NODE_CMD   1
#define DEBUG_NODE_MSG   1
#define DEBUG_NODE_EVENT 1

INT8 check_err(const RT_Node *node, INT8 err, const char* func_name) {
    if (RT_OK != err) {
        RT_LOGE("RTNode(Name=%15.15s, ctx=%p):  errno=%02d, Fail to %s",
                node->name, node->node_ctx, err, func_name);
    }
    return err;
}

#define CHECK_ERR(err) check_err(this, err, __FUNCTION__)

INT8 RT_Node::init() {
    node_ctx = RT_NULL;
    INT8 err = RT_OK;
    err = impl_init(&node_ctx);

    return CHECK_ERR(err);
}

INT8 RT_Node::release() {
    INT8 err = RT_OK;
    err = impl_release(&node_ctx);
    node_ctx = RT_NULL;

    return CHECK_ERR(err);
}

INT8 RT_Node::pull(void *data,  UINT32 *size) {
    INT8 err = RT_OK;
    err  = impl_pull(node_ctx, data, size);

    return CHECK_ERR(err);
}

INT8 RT_Node::push(void *data, UINT32 *size) {
    INT32 err = RT_OK;
    err = impl_push(node_ctx, data, size);

    return CHECK_ERR(err);
}

INT8 RT_Node::run_cmd(RT_NODE_CMD cmd, UINT32 data_int, void* data_void) {
    INT32 err = RT_OK;
    err = impl_run_cmd(node_ctx, cmd, data_int, data_void);
    #if DEBUG_NODE_CMD
    RT_LOGE("node: %-10s exec cmd:%-10s",
                  rt_node_type_name(this->type),
                  rt_node_cmd_name(cmd));
    #endif

    return CHECK_ERR(err);
}
