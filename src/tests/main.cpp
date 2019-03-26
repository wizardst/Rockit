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

#include "rt_base/rt_base_tests.h"   // NOLINT
#include "rt_media/rt_media_tests.h" // NOLINT
#include "rt_node/rt_node_tests.h"   // NOLINT
#include "rt_node/rt_ffmpeg_tests.h" // NOLINT
#include "rt_task/rt_task_tests.h"   // NOLINT
#include "rt_check.h"                // NOLINT
#include "RTObject.h"                // NOLINT

#define TEST_URI "E:\\CloudSync\\low-used\\videos\\start.mp3"

RT_RET unit_test_best_practice(const char* uri) {
    rt_mem_record_reset();
    RTObject::resetTraces();

    rt_mem_record_dump();
    RTObject::dumpTraces();

    return RT_OK;
}

int main(int argc, char **argv) {
    const char* media_name = (argc > 1)? argv[1]:TEST_URI;
    return unit_test_best_practice(media_name);
}
