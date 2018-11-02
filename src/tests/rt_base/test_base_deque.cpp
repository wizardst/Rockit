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
 * author: rimon.xu
 *   date: 20181031
 * e-mail: rimon.xu@rock-chips.com
 */

#include "rt_base_tests.h"
#include "rt_dequeue.h"

RT_RET unit_test_deque_limit(int index, int total_index)
{
    RT_Deque *rt_deque = RT_NULL;
    int int_array[1002];
    rt_memset(int_array, 0, sizeof(int) * 1002);
    for (int i = 0; i < 1000; i++) {
        int_array[i] = i;
    }

    /*
     * WARN: limit mode don't need external free
     */
    rt_deque = deque_create(100);
    CHECK_UE(rt_deque, RT_NULL);

    for (int i = 0; i < 1000; i++) {
        if (i < 100) {
            FUNC_CHECK(deque_push_tail(rt_deque, &int_array[i]));
        } else {
            CHECK_UE(deque_push_tail(rt_deque, &int_array[i]), RT_OK);
        }
    }
    CHECK_EQ(deque_size(rt_deque), 100);

    /* now deque is:
       index: 0 1 2 ... 100 101 102 ... 998 999
       value: 0 1 2 ... 100 101 102 ... 998 999
    */

    for (int i = 0; i < 1000; i++) {
        if (i < 100) {
            CHECK_EQ(*((int *)deque_get(rt_deque, i)), i);
        } else {
            CHECK_EQ(deque_get(rt_deque, i), RT_NULL);
        }
    }
    CHECK_EQ(deque_size(rt_deque), 100);

    for (int i = 0; i < 1000; i++) {
        RT_DequeEntry entry;
        if (i < 100) {
            entry = deque_pop(rt_deque);
            CHECK_UE(entry.data, RT_NULL);
            CHECK_EQ(*((int *)(entry.data)), i);
        } else {
            CHECK_EQ(deque_pop(rt_deque).data, RT_NULL);
        }
    }
    CHECK_EQ(deque_size(rt_deque), 0);

    for (int i = 0; i < 1000; i++) {
        if (i < 100) {
            FUNC_CHECK(deque_push_head(rt_deque, &int_array[i]));
        } else {
            CHECK_UE(deque_push_head(rt_deque, &int_array[i]), RT_OK);
        }

    }
    CHECK_EQ(deque_size(rt_deque), 100);

    for (int i = 0; i < 1000; i++) {
        RT_DequeEntry entry;
        if (i < 100) {
            entry = deque_pop(rt_deque);
            CHECK_UE(entry.data, RT_NULL);
            CHECK_EQ(*((int *)(entry.data)), 99 - i);
        } else {
            CHECK_EQ(deque_pop(rt_deque).data, RT_NULL);
        }

    }
    CHECK_EQ(deque_size(rt_deque), 0);

    deque_destory(&rt_deque);
    CHECK_EQ(rt_deque, RT_NULL);

__RET:
    return RT_OK;
__FAILED:
    return RT_ERR_UNKNOWN;
}

RT_RET unit_test_deque_normal(int index, int total_index)
{
    RT_Deque *rt_deque = RT_NULL;
    int int_array[1002];
    rt_memset(int_array, 0, sizeof(int) * 1002);
    for (int i = 0; i < 1000; i++) {
        int_array[i] = i;
    }

    rt_deque = deque_create();
    CHECK_UE(rt_deque, RT_NULL);

    for (int i = 0; i < 1000; i++) {
        FUNC_CHECK(deque_push_tail(rt_deque, &int_array[i]));
    }
    CHECK_EQ(deque_size(rt_deque), 1000);

    /* now deque is:
       index: 0 1 2 ... 100 101 102 ... 998 999
       value: 0 1 2 ... 100 101 102 ... 998 999
    */

    for (int i = 0; i < 1000; i++) {
        CHECK_EQ(*((int *)deque_get(rt_deque, i)), i);
    }
    CHECK_EQ(deque_size(rt_deque), 1000);

    for (int i = 0; i < 1000; i++) {
        RT_DequeEntry entry;
        entry = deque_pop(rt_deque);
        CHECK_UE(entry.data, RT_NULL);
        CHECK_EQ(*((int *)(entry.data)), i);
    }
    CHECK_EQ(deque_size(rt_deque), 0);

    for (int i = 0; i < 1000; i++) {
        FUNC_CHECK(deque_push_head(rt_deque, &int_array[i]));
    }
    CHECK_EQ(deque_size(rt_deque), 1000);
    /* now deque is:
       index: 0   1   2   ... 998 999
       value: 999 998 997 ... 1   0
    */

    for (int i = 0; i < 1000; i++) {
        CHECK_EQ(*((int *)deque_get(rt_deque, 999 - i)), i);
    }
    CHECK_EQ(deque_size(rt_deque), 1000);

    for (int i = 0; i < 1000; i++) {
        RT_DequeEntry entry;
        entry = deque_pop(rt_deque);
        CHECK_UE(entry.data, RT_NULL);
        CHECK_EQ(*((int *)(entry.data)), 999 - i);
    }
    CHECK_EQ(deque_size(rt_deque), 0);

    deque_destory(&rt_deque);
    CHECK_EQ(rt_deque, RT_NULL);

__RET:
    return RT_OK;
__FAILED:
    return RT_ERR_UNKNOWN;
}

