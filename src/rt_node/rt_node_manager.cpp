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
 *   date: 2018/09/03
 */

#include "rt_node_manager.h"
#include "rt_node_parser.h"
#include "rt_node_codec.h"

INT32 rt_node_register_all() {
    rt_node_register(RT_NULL);
    rt_node_register(RT_NULL);
    return RT_OK;
}

INT32 rt_node_register(const RT_Node *node) {
    return RT_OK;
}

const RT_Node* rt_node_root(){
   return RT_NULL;
}

const RT_Node* rt_node_find(UINT8 cat_type, UINT8 sub_type) {
   return RT_NULL;
}

void rt_node_close(RT_Node *node) {
    return ;
}

const char *rt_node_license(RT_Node *node) {
    return node->name;
}

const char *rt_node_name(RT_Node *node){
    return node->name;
}
