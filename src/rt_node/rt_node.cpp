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
 * author: martin.cheng
 *   date: 20180704
 */

#include "rt_header.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "RT_Node"

RT_NODE_CONTEXT RT_Node::init() const {
    RT_NODE_CONTEXT ctx;
    INT32 err = impl_init(&ctx);
    err       = impl_subinit(ctx);

    RT_LOGT("node_context=%p", ctx);

    return ctx;
}

INT8 RT_Node::release(RT_NODE_CONTEXT ctx) const{
    INT32 err = impl_release(&ctx);

    RT_LOGT("node_context=%p", ctx);

    return log_error(err, "release");
}

INT8 RT_Node::read(RT_NODE_CONTEXT ctx, void *data,  UINT32 *size) const{
    INT32 err = RT_ERR_BAD;
    err = impl_read(ctx, data, size);

    return log_error(err, "read");
}

INT8 RT_Node::write(RT_NODE_CONTEXT ctx, void *data, UINT32 *size) const{
    INT32 err = RT_ERR_BAD;
    err = impl_write(ctx, data, size);

    return log_error(err, "write");
}

INT8 RT_Node::dump(RT_NODE_CONTEXT ctx) const{
    INT32 err = RT_ERR_BAD;
    err = impl_dump(ctx);

    return log_error(err, "dump");
}

INT8 RT_Node::log_error(INT8 err, const char* func_name) const{
    if(RT_ERR_UNKNOWN == err) {
        RT_LOGE("%15.15s:  errno=%02d, Fail to %s", name, err, func_name);
    }
    return err;
}
