#include <stdio.h>
#include "rt_array_list.h"
#include "rt_mem.h"

//!< vaule check
#define CHECK_EQ(ret, val, ...) \
do{ \
    if (ret != val){ \
        printf("%s(%d): value err. val %d vs %d\n", __FUNCTION__, __LINE__, ret, val); \
        return -1; \
}} while (0)


int main(int argc, char **argv)
{
    printf("begin rockit base test..\n");
    printf("begin rockit array list test..\n");
    do {
        RtArrayList *rt_list = NULL;
        rt_list = array_list_create();
        if (!rt_list) {
            printf("array list create failed!\n");
            return -1;
        }

        printf("input data(count = 1000).\n");
        int int_array[1002];
        memset(int_array, 0, sizeof(int) * 1002);
        for (int i = 0; i < 1000; i++) {
            int_array[i] = i;
            array_list_add(rt_list, &int_array[i]);
        }

        CHECK_EQ(*((int *)array_list_get(rt_list, 234)), 234);

        int_array[1000] = 1000;
        int_array[1001] = 1001;
        array_list_insert_at(rt_list, 234, &int_array[1000]);
        array_list_insert_at(rt_list, 234, &int_array[1001]);
        CHECK_EQ(*((int *)array_list_get(rt_list, 233)), 233);
        CHECK_EQ(*((int *)array_list_get(rt_list, 234)), 1001);
        CHECK_EQ(*((int *)array_list_get(rt_list, 235)), 1000);
        CHECK_EQ(*((int *)array_list_get(rt_list, 236)), 234);

        /* now list is:
           index: 0, 1, 2 .... 233  234  235 236 237
           value: 0, 1, 2 .... 233 1001 1001 234 235
        */

        array_list_set(rt_list, 234, &int_array[234]);
        array_list_set(rt_list, 235, &int_array[235]);

        /* now list is:
           index: 0, 1, 2 .... 233 234 235 236 237
           value: 0, 1, 2 .... 233 234 235 234 235
        */

        CHECK_EQ(*((int *)array_list_get(rt_list, 234)), 234);
        CHECK_EQ(*((int *)array_list_get(rt_list, 235)), 235);

        array_list_remove_at(rt_list, 234);
        array_list_remove_at(rt_list, 234);
        array_list_remove_at(rt_list, 234);
        array_list_remove_at(rt_list, 234);

        /* now list is:
           index: 0, 1, 2 .... 233 234 235 236 237
           value: 0, 1, 2 .... 233 236 237 238 239
        */

        CHECK_EQ(*((int *)array_list_get(rt_list, 234)), 236);
        CHECK_EQ(*((int *)array_list_get(rt_list, 235)), 237);

        CHECK_EQ(array_list_contains(rt_list, &int_array[234]), RT_FALSE);
        CHECK_EQ(array_list_contains(rt_list, &int_array[235]), RT_FALSE);
        CHECK_EQ(array_list_contains(rt_list, &int_array[100]), RT_TRUE);
        CHECK_EQ(array_list_contains(rt_list, &int_array[1001]), RT_FALSE);

        array_list_remove(rt_list, &int_array[236]);
        array_list_remove(rt_list, &int_array[237]);

        /* now list is:
           index: 0, 1, 2 .... 233 234 235 236 237
           value: 0, 1, 2 .... 233 238 239 240 241
        */
        CHECK_EQ(*((int *)array_list_get(rt_list, 234)), 238);
        CHECK_EQ(*((int *)array_list_get(rt_list, 235)), 239);

        CHECK_EQ(array_list_get_size(rt_list), 996);

        array_list_remove_all(rt_list);
        CHECK_EQ(array_list_get_size(rt_list), 0);
        CHECK_EQ(array_list_get(rt_list, 0), RT_NULL);

        array_list_destroy(&rt_list);

        CHECK_EQ(rt_list, RT_NULL);
    } while (0);
    printf("rockit array list test success..\n");
    return 0;
}
