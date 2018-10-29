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
 *   date: 20180714
 */

#ifndef __RT_NODE_MANAGER_H__
#define __RT_NODE_MANAGER_H__

#include "rt_header.h"
#include "rt_node.h"
 /**
 *   step 1: register media nodes(parser/codec/filter/device) in dequeue
 *   step 2: find media node you wanted(or auto), then use and close node
 *   step 3: release all media nodes in dequeue
 **/

INT32          rt_node_register_all();
INT32          rt_node_register(const RT_Node *node);
const RT_Node* rt_node_root();
const RT_Node* rt_node_find(UINT8 cat_type, UINT8 sub_type);
void           rt_node_close(RT_Node *node);
const char*    rt_node_license(RT_Node *node);
const char*    rt_node_name(RT_Node *node);

#endif // __RT_NODE_MANAGER_H__
