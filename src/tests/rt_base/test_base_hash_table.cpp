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
#include "rt_hash_table.h"

UINT32 hash_func(UINT32 bucktes, void *key)
{
    return *((UINT32 *)(key)) % bucktes;
}

typedef struct test_struct_t {
    UINT32 key;
    UINT32 value;
} testStruct;

RT_RET unit_test_hash_table(INT32 index, INT32 total_index)
{
    /*
     * init array
     */
    testStruct test_arry[1000];
    rt_memset(test_arry, 0, sizeof(testStruct) * 1000);
    for (INT32 i = 0; i < 1000; i++) {
        test_arry[i].key = i;
        test_arry[i].value = 1000 - i;
    }

    RtHash *hash = rt_hash_alloc(100, hash_func);
    INT32 size = sizeof(test_arry) / sizeof(test_arry[0]);
    for (INT32 i = 0; i < size; i++) {
        rt_hash_add(hash, &test_arry[i].key, sizeof(test_arry[i].key),
                    &(test_arry[i]), sizeof((test_arry[i])));
    }

    testStruct *st = NULL;
    for (INT32 i = 0; i < size; i++) {
        st = (testStruct *)rt_hash_lookup(hash, &test_arry[999 - i].key, sizeof(test_arry[999 - i].key));
        CHECK_UE(st, RT_NULL);
        CHECK_EQ(st->value, i + 1);
    }
    for (INT32 i = 0; i < size; i++) {
        rt_hash_free(hash, &test_arry[i].key, sizeof(test_arry[i].key));
    }

    for (INT32 i = 0; i < size; i++) {
        st = (testStruct *)rt_hash_lookup(hash, &test_arry[i].key, sizeof(test_arry[i].key));
        CHECK_EQ(st, RT_NULL);
    }

__RET:
    return RT_OK;
__FAILED:
    return RT_ERR_UNKNOWN;
}

