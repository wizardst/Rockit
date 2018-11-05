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
 *   date: 20181031
 */

#include "rt_base_tests.h"
#include "rt_linked_list.h"

RT_RET unit_test_linked_list(INT32 index, INT32 total_index)
{
    RtLinkedList *rt_list = RT_NULL;
    rt_list = linked_list_create();
    CHECK_UE(rt_list, RT_NULL);

    INT32 INT32_array[1002];
    rt_memset(INT32_array, 0, sizeof(INT32) * 1002);
    for (INT32 i = 0; i < 1000; i++) {
        INT32_array[i] = i;
        linked_list_add(rt_list, &INT32_array[i]);
    }
    CHECK_EQ(linked_list_get_size(rt_list), 1000);

    CHECK_EQ(*((INT32 *)linked_list_get_data(rt_list, 234)), 234);

    /* now list is:
       index: 0, 1, 2 .... 233 234 235 236 237 ...
       value: 0, 1, 2 .... 233 234 235 236 237 ...
    */

    INT32_array[1000] = 1000;
    INT32_array[1001] = 1001;
    linked_list_insert_at(rt_list, 234, &INT32_array[1000]);
    linked_list_insert_at(rt_list, 234, &INT32_array[1001]);
    CHECK_EQ(*((INT32 *)linked_list_get_data(rt_list, 233)), 233);
    CHECK_EQ(*((INT32 *)linked_list_get_data(rt_list, 234)), 1001);
    CHECK_EQ(*((INT32 *)linked_list_get_data(rt_list, 235)), 1000);
    CHECK_EQ(*((INT32 *)linked_list_get_data(rt_list, 236)), 234);

    /* now list is:
       index: 0, 1, 2 .... 233  234  235 236 237 ...
       value: 0, 1, 2 .... 233 1001 1001 234 235 ...
    */

    linked_list_set(rt_list, 234, &INT32_array[234]);
    linked_list_set(rt_list, 235, &INT32_array[235]);

    /* now list is:
       index: 0, 1, 2 .... 233 234 235 236 237 ...
       value: 0, 1, 2 .... 233 234 235 234 235 ...
    */

    CHECK_EQ(*((INT32 *)linked_list_get_data(rt_list, 234)), 234);
    CHECK_EQ(*((INT32 *)linked_list_get_data(rt_list, 235)), 235);

    FUNC_CHECK(linked_list_remove_at(rt_list, 234));
    FUNC_CHECK(linked_list_remove_at(rt_list, 234));
    FUNC_CHECK(linked_list_remove_at(rt_list, 234));
    FUNC_CHECK(linked_list_remove_at(rt_list, 234));

    /* now list is:
       index: 0, 1, 2 .... 233 234 235 236 237 ...
       value: 0, 1, 2 .... 233 236 237 238 239 ...
    */

    CHECK_EQ(*((INT32 *)linked_list_get_data(rt_list, 234)), 236);
    CHECK_EQ(*((INT32 *)linked_list_get_data(rt_list, 235)), 237);

    CHECK_EQ(linked_list_contains(rt_list, &INT32_array[234]), RT_FALSE);
    CHECK_EQ(linked_list_contains(rt_list, &INT32_array[235]), RT_FALSE);
    CHECK_EQ(linked_list_contains(rt_list, &INT32_array[100]), RT_TRUE);
    CHECK_EQ(linked_list_contains(rt_list, &INT32_array[1001]), RT_FALSE);

    FUNC_CHECK(linked_list_remove(rt_list, &INT32_array[236]));
    FUNC_CHECK(linked_list_remove(rt_list, &INT32_array[237]));

    /* now list is:
       index: 0, 1, 2 .... 233 234 235 236 237 ...
       value: 0, 1, 2 .... 233 238 239 240 241 ...
    */
    CHECK_EQ(*((INT32 *)linked_list_get_data(rt_list, 234)), 238);
    CHECK_EQ(*((INT32 *)linked_list_get_data(rt_list, 235)), 239);

    CHECK_EQ(linked_list_get_size(rt_list), 996);

    FUNC_CHECK(linked_list_remove_at(rt_list, 0));
    /* now list is:
       index: 0, 1, 2 .... 233 234 235 236 237 ... 993 994
       value: 1, 2, 3 .... 238 239 240 241 242 ... 998 999
    */

    FUNC_CHECK(linked_list_remove_at(rt_list, 994));
    /* now list is:
       index: 0, 1, 2 .... 233 234 235 236 237 ... 993
       value: 1, 2, 3 .... 238 239 240 241 242 ... 998
    */

    CHECK_EQ(*((INT32 *)linked_list_get_data(rt_list, 0)), 1);
    CHECK_EQ(*((INT32 *)linked_list_get_data(rt_list, 993)), 998);

    linked_list_remove_all(rt_list);
    CHECK_EQ(linked_list_get_size(rt_list), 0);
    CHECK_EQ(linked_list_get_data(rt_list, 0), RT_NULL);

    linked_list_destroy(&rt_list);

    CHECK_EQ(rt_list, RT_NULL);

__RET:
    return RT_OK;
__FAILED:
    return RT_ERR_UNKNOWN;
}

