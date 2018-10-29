#include <iostream>

#include <unistd.h>

#include "../src/tests/test_node_basic.h"
#include "../src/tests/test_base_rebust.h"
#include "../src/tests/test_base_thread.h"
#include "../src/tests/test_base_container.h"
#include "../src/tests/test_task_taskpool.h"
#include "rt_time.h"
#include "rt_log.h"

using namespace std;

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "main"

#define HAVE_TEST_BASE_CONTAINER 1
#define HAVE_TEST_BASE_REBUST 0
#define HAVE_TEST_BASE_THREAD 0
#define HAVE_TEST_NODE_BASIC 0
#define HAVE_TEST_NODE_TASK_TASKPOOL 0

void unit_test_time() {
    UINT64 now_ms = RtTime::getNowTimeMs();
    usleep(10*1000);
    RT_LOGE("unit_test_time now_us=%lldms", (RtTime::getNowTimeMs()-now_ms));
}

int main()
{
    cout << "Hello world!" << endl;

    #if HAVE_TEST_BASE_REBUST
    unit_test_memery();
    #endif

    #if HAVE_TEST_NODE_BASIC
    unit_test_demuxer();
    unit_test_muxer();
    unit_test_demuxer();
    unit_test_muxer();
    #endif // HAVE_TEST_NODE_BASIC

    #if HAVE_TEST_BASE_THREAD
    unit_test_time();
    unit_test_mutex();
    unit_test_thread();
    #endif // HAVE_TEST_BASE_THREAD

    #if HAVE_TEST_NODE_TASK_TASKPOOL
    unit_test_task();
    unit_test_taskpool(1, 3, 3);
    unit_test_taskpool(1, 3, 5);

    unit_test_taskpool(0, 3, 3);
    unit_test_taskpool(8, 40, 400);
    #endif

    #if HAVE_TEST_BASE_CONTAINER
    unit_test_link_list();
    unit_test_kernel_link_list();
    unit_test_kernel_hash_list();
    #endif

    rt_mem_record_dump();

    return 0;
}
