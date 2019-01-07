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

#ifndef SRC_TESTS_RT_NODE_RT_NODE_TESTS_H_
#define SRC_TESTS_RT_NODE_RT_NODE_TESTS_H_

#include "rt_header.h" // NOLINT
#include "rt_test_header.h" // NOLINT

RT_RET unit_test_node_bus(INT32 index, INT32 total);
RT_RET unit_test_node_data_flow(INT32 index, INT32 total);
RT_RET unit_test_ff_node_decoder(INT32 index, INT32 total);
RT_RET unit_test_ff_node_encoder(INT32 index, INT32 total);
RT_RET unit_test_ff_node_demuxer(INT32 index, INT32 total);


#endif  // SRC_TESTS_RT_NODE_RT_NODE_TESTS_H_
