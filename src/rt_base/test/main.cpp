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

#include <stdio.h>

#include "rt_array_list.h"
#include "rt_dequeue.h"
#include "rt_mem.h"
#include "rt_time.h"
#include "rt_linked_list.h"
#include "rt_hash_table.h"

//!< value check
#define CHECK_EQ(ret, val, ...) \
    do{ \
        if (ret != val){ \
            printf("%s(%d): value err. val %d vs exp %d\n", __FUNCTION__, __LINE__, ret, val); \
            goto __FAILED; \
    }} while (0)

    //!< value check
#define CHECK_UNEQ(ret, val, ...) \
    do{ \
        if (ret == val){ \
            printf("%s(%d): value err.\n", __FUNCTION__, __LINE__); \
            goto __FAILED; \
    }} while (0)


#define FUNC_CHECK(ret, ...) \
    do{ \
        if (ret){ \
            printf("%s(%d) failed err: %d\n", __FUNCTION__, __LINE__, ret); \
            goto __FAILED; \
    }} while (0)

#define PRINT_TEST_BEGIN(index, total_index, content, ...) \
    do { \
        printf("[%d/%d] %s begin\n", index + 1, total_index, content); \
    } while (0)

#define PRINT_TEST_END(index, total_index, content, result, ...) \
        do { \
            printf("[%d/%d] %s [%s]\n", \
                    index + 1, total_index, content, result == RT_OK ? "succes" : "failed"); \
        } while (0)

typedef RT_RET TestFunc(int index, int total_index);

static RT_RET rt_array_list_test(int index, int total_index)
{
    PRINT_TEST_BEGIN(index, total_index, "RockitArrayListTest");
    RtArrayList *rt_list = RT_NULL;
    rt_list = array_list_create();
    CHECK_UNEQ(rt_list, RT_NULL);

    int int_array[1002];
    memset(int_array, 0, sizeof(int) * 1002);
    for (int i = 0; i < 1000; i++) {
        int_array[i] = i;
        array_list_add(rt_list, &int_array[i]);
    }
    CHECK_EQ(array_list_get_size(rt_list), 1000);

    CHECK_EQ(*((int *)array_list_get_data(rt_list, 234)), 234);

    /* now list is:
       index: 0, 1, 2 .... 233 234 235 236 237 ...
       value: 0, 1, 2 .... 233 234 235 236 237 ...
    */

    int_array[1000] = 1000;
    int_array[1001] = 1001;
    array_list_insert_at(rt_list, 234, &int_array[1000]);
    array_list_insert_at(rt_list, 234, &int_array[1001]);
    CHECK_EQ(*((int *)array_list_get_data(rt_list, 233)), 233);
    CHECK_EQ(*((int *)array_list_get_data(rt_list, 234)), 1001);
    CHECK_EQ(*((int *)array_list_get_data(rt_list, 235)), 1000);
    CHECK_EQ(*((int *)array_list_get_data(rt_list, 236)), 234);

    /* now list is:
       index: 0, 1, 2 .... 233  234  235 236 237 ...
       value: 0, 1, 2 .... 233 1001 1001 234 235 ...
    */

    array_list_set(rt_list, 234, &int_array[234]);
    array_list_set(rt_list, 235, &int_array[235]);

    /* now list is:
       index: 0, 1, 2 .... 233 234 235 236 237 ...
       value: 0, 1, 2 .... 233 234 235 234 235 ...
    */

    CHECK_EQ(*((int *)array_list_get_data(rt_list, 234)), 234);
    CHECK_EQ(*((int *)array_list_get_data(rt_list, 235)), 235);

    array_list_remove_at(rt_list, 234);
    array_list_remove_at(rt_list, 234);
    array_list_remove_at(rt_list, 234);
    array_list_remove_at(rt_list, 234);

    /* now list is:
       index: 0, 1, 2 .... 233 234 235 236 237 ...
       value: 0, 1, 2 .... 233 236 237 238 239 ...
    */

    CHECK_EQ(*((int *)array_list_get_data(rt_list, 234)), 236);
    CHECK_EQ(*((int *)array_list_get_data(rt_list, 235)), 237);

    CHECK_EQ(array_list_contains(rt_list, &int_array[234]), RT_FALSE);
    CHECK_EQ(array_list_contains(rt_list, &int_array[235]), RT_FALSE);
    CHECK_EQ(array_list_contains(rt_list, &int_array[100]), RT_TRUE);
    CHECK_EQ(array_list_contains(rt_list, &int_array[1001]), RT_FALSE);

    array_list_remove(rt_list, &int_array[236]);
    array_list_remove(rt_list, &int_array[237]);

    /* now list is:
       index: 0, 1, 2 .... 233 234 235 236 237 ...
       value: 0, 1, 2 .... 233 238 239 240 241 ...
    */
    CHECK_EQ(*((int *)array_list_get_data(rt_list, 234)), 238);
    CHECK_EQ(*((int *)array_list_get_data(rt_list, 235)), 239);

    CHECK_EQ(array_list_get_size(rt_list), 996);

    FUNC_CHECK(array_list_remove_at(rt_list, 0));
    /* now list is:
       index: 0, 1, 2 .... 233 234 235 236 237 ... 993 994
       value: 1, 2, 3 .... 238 239 240 241 242 ... 998 999
    */

    FUNC_CHECK(array_list_remove_at(rt_list, 994));
    /* now list is:
       index: 0, 1, 2 .... 233 234 235 236 237 ... 993
       value: 1, 2, 3 .... 238 239 240 241 242 ... 998
    */

    CHECK_EQ(*((int *)array_list_get_data(rt_list, 0)), 1);
    CHECK_EQ(*((int *)array_list_get_data(rt_list, 993)), 998);

    array_list_remove_all(rt_list);
    CHECK_EQ(array_list_get_size(rt_list), 0);
    CHECK_EQ(array_list_get_data(rt_list, 0), RT_NULL);

    array_list_destroy(&rt_list);

    CHECK_EQ(rt_list, RT_NULL);

__RET:
    PRINT_TEST_END(index, total_index, "RockitArrayListTest", RT_OK);
    return RT_OK;
__FAILED:
    PRINT_TEST_END(index, total_index, "RockitArrayListTest", RT_ERR_UNKNOWN);
    return RT_ERR_UNKNOWN;
}

static RT_RET rt_linked_list_test(int index, int total_index)
{
    PRINT_TEST_BEGIN(index, total_index, "RockitLinkedListTest");
    RtLinkedList *rt_list = RT_NULL;
    rt_list = linked_list_create();
    CHECK_UNEQ(rt_list, RT_NULL);

    int int_array[1002];
    memset(int_array, 0, sizeof(int) * 1002);
    for (int i = 0; i < 1000; i++) {
        int_array[i] = i;
        linked_list_add(rt_list, &int_array[i]);
    }
    CHECK_EQ(linked_list_get_size(rt_list), 1000);

    CHECK_EQ(*((int *)linked_list_get_data(rt_list, 234)), 234);

    /* now list is:
       index: 0, 1, 2 .... 233 234 235 236 237 ...
       value: 0, 1, 2 .... 233 234 235 236 237 ...
    */

    int_array[1000] = 1000;
    int_array[1001] = 1001;
    linked_list_insert_at(rt_list, 234, &int_array[1000]);
    linked_list_insert_at(rt_list, 234, &int_array[1001]);
    CHECK_EQ(*((int *)linked_list_get_data(rt_list, 233)), 233);
    CHECK_EQ(*((int *)linked_list_get_data(rt_list, 234)), 1001);
    CHECK_EQ(*((int *)linked_list_get_data(rt_list, 235)), 1000);
    CHECK_EQ(*((int *)linked_list_get_data(rt_list, 236)), 234);

    /* now list is:
       index: 0, 1, 2 .... 233  234  235 236 237 ...
       value: 0, 1, 2 .... 233 1001 1001 234 235 ...
    */

    linked_list_set(rt_list, 234, &int_array[234]);
    linked_list_set(rt_list, 235, &int_array[235]);

    /* now list is:
       index: 0, 1, 2 .... 233 234 235 236 237 ...
       value: 0, 1, 2 .... 233 234 235 234 235 ...
    */

    CHECK_EQ(*((int *)linked_list_get_data(rt_list, 234)), 234);
    CHECK_EQ(*((int *)linked_list_get_data(rt_list, 235)), 235);

    FUNC_CHECK(linked_list_remove_at(rt_list, 234));
    FUNC_CHECK(linked_list_remove_at(rt_list, 234));
    FUNC_CHECK(linked_list_remove_at(rt_list, 234));
    FUNC_CHECK(linked_list_remove_at(rt_list, 234));

    /* now list is:
       index: 0, 1, 2 .... 233 234 235 236 237 ...
       value: 0, 1, 2 .... 233 236 237 238 239 ...
    */

    CHECK_EQ(*((int *)linked_list_get_data(rt_list, 234)), 236);
    CHECK_EQ(*((int *)linked_list_get_data(rt_list, 235)), 237);

    CHECK_EQ(linked_list_contains(rt_list, &int_array[234]), RT_FALSE);
    CHECK_EQ(linked_list_contains(rt_list, &int_array[235]), RT_FALSE);
    CHECK_EQ(linked_list_contains(rt_list, &int_array[100]), RT_TRUE);
    CHECK_EQ(linked_list_contains(rt_list, &int_array[1001]), RT_FALSE);

    FUNC_CHECK(linked_list_remove(rt_list, &int_array[236]));
    FUNC_CHECK(linked_list_remove(rt_list, &int_array[237]));

    /* now list is:
       index: 0, 1, 2 .... 233 234 235 236 237 ...
       value: 0, 1, 2 .... 233 238 239 240 241 ...
    */
    CHECK_EQ(*((int *)linked_list_get_data(rt_list, 234)), 238);
    CHECK_EQ(*((int *)linked_list_get_data(rt_list, 235)), 239);

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

    CHECK_EQ(*((int *)linked_list_get_data(rt_list, 0)), 1);
    CHECK_EQ(*((int *)linked_list_get_data(rt_list, 993)), 998);

    linked_list_remove_all(rt_list);
    CHECK_EQ(linked_list_get_size(rt_list), 0);
    CHECK_EQ(linked_list_get_data(rt_list, 0), RT_NULL);

    linked_list_destroy(&rt_list);

    CHECK_EQ(rt_list, RT_NULL);

__RET:
    PRINT_TEST_END(index, total_index, "RockitLinkedListTest", RT_OK);
    return RT_OK;
__FAILED:
    PRINT_TEST_END(index, total_index, "RockitLinkedListTest", RT_ERR_UNKNOWN);
    return RT_ERR_UNKNOWN;
}

//±àÐ´hashº¯Êý
unsigned int hash_func(UINT32 bucktes, void *key)
{
    return *((UINT32 *)(key)) % bucktes;
}

typedef struct test_struct_t {
    UINT32 key;
    UINT32 value;
} testStruct;

static RT_RET rt_hash_table_test(int index, int total_index)
{
    PRINT_TEST_BEGIN(index, total_index, "RockitHashTableTest");

    /*
     * init array
     */
    testStruct test_arry[1000];
    memset(test_arry, 0, sizeof(testStruct) * 1000);
    for (int i = 0; i < 1000; i++) {
        test_arry[i].key = i;
        test_arry[i].value = 1000 - i;
    }
    
    RtHash *hash = rt_hash_alloc(100, hash_func);
    int size = sizeof(test_arry) / sizeof(test_arry[0]);
    for (int i = 0; i < size; i++)
    {
        rt_hash_add(hash, &test_arry[i].key, sizeof(test_arry[i].key),
            &(test_arry[i]), sizeof((test_arry[i])));
    }

    testStruct *st = NULL;
    for (int i = 0; i < size; i++)
    {
        st = (testStruct *)rt_hash_lookup(hash, &test_arry[999 - i].key, sizeof(test_arry[999 - i].key));
        CHECK_UNEQ(st, RT_NULL);
        CHECK_EQ(st->value, i + 1);
    }
    for (int i = 0; i < size; i++)
    {
        rt_hash_free(hash, &test_arry[i].key, sizeof(test_arry[i].key));
    }

    for (int i = 0; i < size; i++)
    {
        st = (testStruct *)rt_hash_lookup(hash, &test_arry[i].key, sizeof(test_arry[i].key));
        CHECK_EQ(st, RT_NULL);
    }

__RET:
    PRINT_TEST_END(index, total_index, "RockitHashTableTest", RT_OK);
    return RT_OK;
__FAILED:
    PRINT_TEST_END(index, total_index, "RockitHashTableTest", RT_ERR_UNKNOWN);
    return RT_ERR_UNKNOWN;
}

static RT_RET rt_deque_limit_test(int index, int total_index)
{
    PRINT_TEST_BEGIN(index, total_index, "RockitDequeTest#Limit(100)");

    RT_Deque *rt_deque = RT_NULL;
    int int_array[1002];
    memset(int_array, 0, sizeof(int) * 1002);
    for(int i = 0; i < 1000; i++) {
        int_array[i] = i;
    }

    /* 
     * WARN: limit mode don't need external free
     */
    rt_deque = deque_create(100);
    CHECK_UNEQ(rt_deque, RT_NULL);

    for(int i = 0; i < 1000; i++) {
        if (i < 100) {
            FUNC_CHECK(deque_push_tail(rt_deque, &int_array[i]));
        } else {
            CHECK_UNEQ(deque_push_tail(rt_deque, &int_array[i]), RT_OK);
        }
    }
    CHECK_EQ(deque_size(rt_deque), 100);

    /* now deque is:
       index: 0 1 2 ... 100 101 102 ... 998 999
       value: 0 1 2 ... 100 101 102 ... 998 999
    */

    for(int i = 0; i < 1000; i++) {
        if (i < 100) {
            CHECK_EQ(*((int *)deque_get(rt_deque, i)), i);
        } else {
            CHECK_EQ(deque_get(rt_deque, i), RT_NULL);
        }
    }
    CHECK_EQ(deque_size(rt_deque), 100);

    for(int i = 0; i < 1000; i++) {
        RT_DequeEntry entry;
        if (i < 100) {
            entry = deque_pop(rt_deque);
            CHECK_UNEQ(entry.data, RT_NULL);
            CHECK_EQ(*((int *)(entry.data)), i);
        } else {
            CHECK_EQ(deque_pop(rt_deque).data, RT_NULL);
        }
    }
    CHECK_EQ(deque_size(rt_deque), 0);

    for(int i = 0; i < 1000; i++) {
        if (i < 100) {
            FUNC_CHECK(deque_push_head(rt_deque, &int_array[i]));
        } else {
            CHECK_UNEQ(deque_push_head(rt_deque, &int_array[i]), RT_OK);
        }

    }
    CHECK_EQ(deque_size(rt_deque), 100);

    for(int i = 0; i < 1000; i++) {
        RT_DequeEntry entry;
        if (i < 100) {
            entry = deque_pop(rt_deque);
            CHECK_UNEQ(entry.data, RT_NULL);
            CHECK_EQ(*((int *)(entry.data)), 99 - i);
        } else {
            CHECK_EQ(deque_pop(rt_deque).data, RT_NULL);
        }

    }
    CHECK_EQ(deque_size(rt_deque), 0);

    deque_destory(&rt_deque);
    CHECK_EQ(rt_deque, RT_NULL);

__RET:
    PRINT_TEST_END(index, total_index, "RockitDequeTest#Limit(100)", RT_OK);
    return RT_OK;
__FAILED:
    PRINT_TEST_END(index, total_index, "RockitDequeTest#Limit(100)", RT_ERR_UNKNOWN);
    return RT_ERR_UNKNOWN;
}

static RT_RET rt_deque_normal_test(int index, int total_index)
{
    PRINT_TEST_BEGIN(index, total_index, "RockitDequeTest#NormalTest");

    RT_Deque *rt_deque = RT_NULL;
    int int_array[1002];
    memset(int_array, 0, sizeof(int) * 1002);
    for(int i = 0; i < 1000; i++) {
        int_array[i] = i;
    }

    rt_deque = deque_create();
    CHECK_UNEQ(rt_deque, RT_NULL);

    for(int i = 0; i < 1000; i++) {
        FUNC_CHECK(deque_push_tail(rt_deque, &int_array[i]));
    }
    CHECK_EQ(deque_size(rt_deque), 1000);

    /* now deque is:
       index: 0 1 2 ... 100 101 102 ... 998 999
       value: 0 1 2 ... 100 101 102 ... 998 999
    */

    for(int i = 0; i < 1000; i++) {
        CHECK_EQ(*((int *)deque_get(rt_deque, i)), i);
    }
    CHECK_EQ(deque_size(rt_deque), 1000);

    for(int i = 0; i < 1000; i++) {
        RT_DequeEntry entry;
        entry = deque_pop(rt_deque);
        CHECK_UNEQ(entry.data, RT_NULL);
        CHECK_EQ(*((int *)(entry.data)), i);
    }
    CHECK_EQ(deque_size(rt_deque), 0);

    for(int i = 0; i < 1000; i++) {
        FUNC_CHECK(deque_push_head(rt_deque, &int_array[i]));
    }
    CHECK_EQ(deque_size(rt_deque), 1000);
    /* now deque is:
       index: 0   1   2   ... 998 999
       value: 999 998 997 ... 1   0
    */

    for(int i = 0; i < 1000; i++) {
        CHECK_EQ(*((int *)deque_get(rt_deque, 999 - i)), i);
    }
    CHECK_EQ(deque_size(rt_deque), 1000);

    for(int i = 0; i < 1000; i++) {
        RT_DequeEntry entry;
        entry = deque_pop(rt_deque);
        CHECK_UNEQ(entry.data, RT_NULL);
        CHECK_EQ(*((int *)(entry.data)), 999 - i);
    }
    CHECK_EQ(deque_size(rt_deque), 0);

    deque_destory(&rt_deque);
    CHECK_EQ(rt_deque, RT_NULL);

__RET:
    PRINT_TEST_END(index, total_index, "RockitDequeTest#NormalTest", RT_OK);
    return RT_OK;
__FAILED:
    PRINT_TEST_END(index, total_index, "RockitDequeTest#NormalTest", RT_ERR_UNKNOWN);
    return RT_ERR_UNKNOWN;
}

int run_test(RtArrayList *list)
{
    int total_count = array_list_get_size(list);
    int cur_count = array_list_get_size(list);

    TestFunc *func = NULL;

    for (int i = 0; i < total_count; i++) {
        func = (TestFunc *)array_list_get_data(list, 0);
        UINT64 start = RtTime::getNowTimeUs();
        func(i, total_count);
        UINT64 end = RtTime::getNowTimeUs();
        UINT64 space_time = end - start;
        printf("test space time: %lld.%lld ms\n\n", space_time / 1000, space_time % 1000);
        CHECK_EQ(array_list_remove_at(list, 0),  RT_OK);
    }

__RET:
    return RT_OK;
__FAILED:
    return RT_ERR_UNKNOWN;
}

int main(int argc, char **argv)
{
    printf("rockit base test begin..\n");

    TestFunc *func = NULL;
    RtArrayList *rt_list = RT_NULL;
    rt_list = array_list_create();
    CHECK_UNEQ(rt_list, RT_NULL);

    array_list_add(rt_list, (void *)rt_array_list_test);
    array_list_add(rt_list, (void *)rt_deque_normal_test);
    array_list_add(rt_list, (void *)rt_deque_limit_test);
    array_list_add(rt_list, (void *)rt_linked_list_test);
    array_list_add(rt_list, (void *)rt_hash_table_test);

    run_test(rt_list);
    
__RET:
    printf("rockit base test end..\n");
    return RT_OK;
__FAILED:
    printf("rockit base test end..\n");
    return RT_ERR_UNKNOWN;
}
