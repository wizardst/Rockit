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
 * author: rimon.xu@rock-chips.com
 *   date: 20181102
 */

#ifndef SRC_TESTS_RT_BASE_RT_BASE_TESTS_H_
#define SRC_TESTS_RT_BASE_RT_BASE_TESTS_H_

#include "rt_header.h" // NOLINT
#include "rt_test_header.h" // NOLINT

RT_RET unit_test_hash_table(INT32 index, INT32 total_index);
RT_RET unit_test_array_list(INT32 index, INT32 total_index);
RT_RET unit_test_linked_list(INT32 index, INT32 total_index);
RT_RET unit_test_deque_limit(INT32 index, INT32 total_index);
RT_RET unit_test_deque_normal(INT32 index, INT32 total_index);

RT_RET unit_test_memory(INT32 index, INT32 total_index);
RT_RET unit_test_mem_service(INT32 index, INT32 total_index);
RT_RET unit_test_mutex(INT32 index, INT32 total_index);
RT_RET unit_test_thread(INT32 index, INT32 total_index);
RT_RET unit_test_lock_unlock(INT32 index, INT32 total_index);
RT_RET unit_test_cond_lock(INT32 index, INT32 total_index);

#endif  // SRC_TESTS_RT_BASE_RT_BASE_TESTS_H_
