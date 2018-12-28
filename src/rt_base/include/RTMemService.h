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
 *   date: 2018/07/05
 */

#ifndef SRC_RT_BASE_INCLUDE_RTMEMSERVICE_H_
#define SRC_RT_BASE_INCLUDE_RTMEMSERVICE_H_

#include "rt_header.h" // NOLINT

typedef struct _mem_node {
    INT32       size;
    void        *ptr;
    const char  *caller;
} MemNode;

class RTMemService {
 public:
    RTMemService();
    ~RTMemService();
    void addNode(const char *caller, void* ptr, UINT32 size);
    void removeNode(void* ptr, UINT32 *size);
    void reset();
    void dump();
    INT32 findNode(const char *caller, void* ptr, UINT32 *size);

 public:
    MemNode *mem_nodes;
    UINT32  nodes_max;
    UINT32  nodes_cnt;
    UINT32  total_size;
};

#endif  // SRC_RT_BASE_INCLUDE_RTMEMSERVICE_H_

