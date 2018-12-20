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

#include "rt_base/rt_base_tests.h" // NOLINT
#include "rt_node/rt_node_tests.h" // NOLINT
#include "rt_node/rt_ffmpeg_tests.h" // NOLINT
#include "rt_task/rt_task_tests.h" // NOLINT
#include "rt_check.h" // NOLINT

RT_RET unit_test_best_practice() {
    rt_mem_record_reset();

    /* your unit test */
    unit_test_node_bus(0, 0);

    rt_mem_record_dump();

    unit_test_node_basic(0, 0);
    return RT_OK;
}

int main(int argc, char **argv) {
    return unit_test_best_practice();
}
