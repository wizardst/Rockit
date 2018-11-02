/*
 * Copyright 2018 The Rockit Open Source Project
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

#include "rt_base/rt_base_tests.h"
#include "rt_node/rt_node_tests.h"
#include "rt_task/rt_task_tests.h"
#include "rt_check.h"

RT_RET unit_test_best_practice() {
    rt_mem_record_reset();
    unit_test_hash_table(0,0);
    rt_mem_record_dump();
}

int main(int argc, char **argv)
{
    return unit_test_best_practice();
}
