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
 *   date: 2018/09/19
 */

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "base_thread"

/*rand and usleep*/
#include <unistd.h>

#include "rt_mutex.h" // NOLINT
#include "rt_thread.h" // NOLINT
#include "rt_base_tests.h" // NOLINT

typedef struct _fake_lock_context {
    RtMutex     *lock0;
    RtMutex     *lock1;
    RtCondition *rt_cond;
    RT_BOOL      wait;
    UINT32       wait_cnt;
} FakeLockContext;

void utils_lock_ctx_init(FakeLockContext *ctx) {
    ctx->lock0    = new RtMutex();
    ctx->lock1    = new RtMutex();
    ctx->rt_cond  = new RtCondition();
    ctx->wait     = RT_TRUE;
    ctx->wait_cnt = 0;
}

void utils_lock_ctx_deinit(FakeLockContext *ctx) {
    #if 1
    rt_safe_delete(ctx->lock0);
    RT_ASSERT(RT_NULL == ctx->lock0);

    rt_safe_delete(ctx->lock1);
    RT_ASSERT(RT_NULL == ctx->lock1);

    rt_safe_delete(ctx->rt_cond);
    RT_ASSERT(RT_NULL == ctx->rt_cond);
    #else
    delete ctx->lock0;
    ctx->lock0 = RT_NULL;

    delete ctx->lock1;
    ctx->lock1 = RT_NULL;

    delete ctx->rt_cond;
    ctx->rt_cond = RT_NULL;
    #endif
}

RT_RET unit_test_mutex(INT32 index, INT32 total_index) {
    RtMutex *lock = new RtMutex();
    lock->lock();
    lock->unlock();
    delete lock;
    return RT_OK;
}

void* unit_test_thread_loop(void*) {
    int rand_sleep = (rand() % 1000) * 10000; // NOLINT
    UINT32 count = 0;
    while (count++ < 5) {
        RT_LOGE("start, count is %d", count);
        usleep(rand_sleep);
        RT_LOGE("done, count is %d", count);
    }
    return NULL;
}

RT_RET unit_test_thread(INT32 index, INT32 total_index) {
    RtThread* td = new RtThread(unit_test_thread_loop, NULL);
    td->start();
    td->join();
    RT_LOGE("done");
    return RT_OK;
}

void* callback_lock_unlock(void* fake_ctx) {
    FakeLockContext *ctx = reinterpret_cast<FakeLockContext *>(fake_ctx);
    UINT32 idx = 0, cnt_lock = 0, cnt_auto = 0, cnt_test = 1024;
    for (idx = 0; idx < cnt_test; idx++) {
        if ((idx % 2) == 0) {
            RtAutoMutex autolock(ctx->lock0);
            cnt_lock++;
        } else {
            ctx->lock1->lock();
            cnt_auto++;
            ctx->lock1->unlock();
        }
        if (idx % (cnt_test / 10) == 0) {
            RT_LOGE("stats: [pid=%d; cnt_lock=%03d; cnt_auto=%03d; pass=%03d/%03d]",
                           RtThread::getThreadID(), cnt_lock, cnt_auto, idx+1, cnt_test);
        }
        RtTime::sleepMs(1);
    }
    RT_LOGE("stats: [pid=%d; cnt_lock=%03d; cnt_auto=%03d; pass=%03d/%03d] DONE",
                           RtThread::getThreadID(), cnt_lock, cnt_auto, idx, cnt_test);
    return RT_NULL;
}

void* callback_cond_lock(void* fake_ctx) {
    FakeLockContext *ctx = reinterpret_cast<FakeLockContext *>(fake_ctx);
    UINT32 idx = 0, cnt_sig = 0, cnt_test = 1024;
    ctx->lock0->lock();
    RT_BOOL wait = ctx->wait;
    ctx->wait    = RT_FALSE;
    ctx->lock0->unlock();
    for (idx = 0; (idx < cnt_test) || (ctx->wait_cnt < cnt_test); idx++) {
        if (RT_TRUE == wait) {
            if (0 == ctx->lock0->trylock()) {
                // RT_LOGE("stats: [pid=%d; idx=%3d]", RtThread::getThreadID(), idx);
                ctx->rt_cond->wait(ctx->lock1);
                ctx->wait_cnt++;
                ctx->lock0->unlock();
            }
        } else {
            if (0 != ctx->lock0->trylock()) {
                // RT_LOGE("stats: [pid=%d; idx=%3d]", RtThread::getThreadID(), idx);
                ctx->rt_cond->signal();
                cnt_sig++;
            } else {
                ctx->lock0->unlock();
            }
        }
        if (ctx->wait_cnt % (cnt_test / 10) == 0) {
            RT_LOGE("stats: [pid=%d; cnt_wait=%03d; cnt_sig=%03d; pass=%03d/%03d]",
                           RtThread::getThreadID(), ctx->wait_cnt, cnt_sig, idx + 1, cnt_test);
        }
        RtTime::sleepMs(1);
    }
    RT_LOGE("stats: [pid=%d; cnt_wait=%03d; cnt_sig=%03d; pass=%03d/%03d] DONE",
                           RtThread::getThreadID(), ctx->wait_cnt, cnt_sig, idx, cnt_test);
    return RT_NULL;
}

RT_RET unit_test_lock_unlock(INT32 index, INT32 total_index) {
    FakeLockContext ctx;
    utils_lock_ctx_init(&ctx);
    RtThread* th0 = new RtThread(callback_lock_unlock, &ctx);
    RtThread* th1 = new RtThread(callback_lock_unlock, &ctx);
    th0->start();
    th1->start();
    th0->join();
    th1->join();
    utils_lock_ctx_deinit(&ctx);
    rt_safe_delete(th0);
    rt_safe_delete(th1);
    RT_LOGE("success! \n");
    return RT_OK;
}

RT_RET unit_test_cond_lock(INT32 index, INT32 total_index) {
    FakeLockContext ctx;
    utils_lock_ctx_init(&ctx);
    RtThread* th0 = new RtThread(callback_cond_lock, &ctx);
    RtThread* th1 = new RtThread(callback_cond_lock, &ctx);
    th0->start();
    th1->start();
    th0->join();
    th1->join();
    utils_lock_ctx_deinit(&ctx);
    rt_safe_delete(th0);
    rt_safe_delete(th1);
    RT_LOGE("success! \n");
    return RT_OK;
}

