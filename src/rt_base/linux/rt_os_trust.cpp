/*
 * Copyright 2019 Rockchip Electronics Co. LTD
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
 * author: hery.xu@rock-chips.com
 *   date: 2019/02/27
 */

#include <dlfcn.h>
#include <fcntl.h>
#include "rt_trust.h" // NOLINT


// 9d2a86c0-cd47-4f08-8b52-c3df38929fa2
#define TA_SECURE_VIDEO_UUID { 0x9d2a86c0, 0xcd47, 0x4f08, \
                               { 0x8b, 0x52, 0xc3, 0xdf, 0x38, 0x92, 0x9f, 0xa2} }

/* The TAFs ID implemented in this TA */
typedef enum TA_SECURE_CMD {
    TA_SECURE_VIDEO_VPU_POWER_ON = 0,
    TA_SECURE_VIDEO_VPU_POWER_OFF,
    TA_SECURE_VIDEO_GET_VERSION,
    TA_SECURE_VIDEO_GET_HW_INFO,


    TA_SECURE_VIDEO_INIT_VCODEC = 100,
    TA_SECURE_VIDEO_SET_REGS,
    TA_SECURE_VIDEO_GET_REGS,

    TA_SECURE_VIDEO_PARSE_H264 = 200,  // TA_SECURE_VIDEO_PARSE_DATA,
    TA_SECURE_VIDEO_PARSER_4K_H264,
    TA_SECURE_VIDEO_PARSER_HEVC,
    TA_SECURE_VIDEO_PARSER_VP9,
    TA_SECURE_VIDEO_PARSER_VP8,
    TA_SECURE_VIDEO_PARSER_MPEG2,
    TA_SECURE_VIDEO_PARSER_MPEG4,

    TA_SECURE_VIDEO_MEMSET = 300,
    TA_SECURE_SET_VPU_SECURE,
    TA_SECURE_CANCLE_VPU_SECURE,
    TA_SECURE_VIDEO_MEMCPY,
    TA_SECURE_N_T_S,
} TA_SECURE_CMD;


#ifdef OS_LINUX

RtTrust::RtTrust() {
    init();
}

RtTrust::~RtTrust() {
    if (p->teec_sess.fd >= 0) {
        RT_LOGE("svp_ca_deinit close session");
        svp_TEEC_CloseSession(&(p->teec_sess));
        memset(&(p->teec_sess), 0, sizeof(TEEC_Session));
    }

    if (p->teec_ctx.fd >= 0) {
        RT_LOGE("svp_ca_deinit close context");
        svp_TEEC_FinalizeContext(&(p->teec_ctx));
        memset(&(p->teec_ctx), 0, sizeof(TEEC_Context));
    }
}

RT_RET RtTrust::init() {
    RT_RET ret        = RT_OK;
    uint32_t    err_origin = 0;
    TEEC_UUID   uuid       = TA_SECURE_VIDEO_UUID;

    void * mRkSecureApiHdl = dlopen("/vendor/lib/libteec.so", RTLD_LAZY);
    if (mRkSecureApiHdl == NULL) {
        mRkSecureApiHdl = dlopen("/system/lib/libteec.so", RTLD_LAZY);
        if (mRkSecureApiHdl == NULL) {
            RT_LOGE("dlopen failed, error: %s", dlerror());
            return RT_ERR_BAD;
        }
    }

    svp_TEEC_InitializeContext = (func_TEEC_InitializeContext)dlsym(mRkSecureApiHdl, "TEEC_InitializeContext");
    svp_TEEC_FinalizeContext = (func_TEEC_FinalizeContext)dlsym(mRkSecureApiHdl, "TEEC_FinalizeContext");
    svp_TEEC_OpenSession = (func_TEEC_OpenSession)dlsym(mRkSecureApiHdl, "TEEC_OpenSession");
    svp_TEEC_CloseSession = (func_TEEC_CloseSession)dlsym(mRkSecureApiHdl, "TEEC_CloseSession");
    svp_TEEC_InvokeCommand = (func_TEEC_InvokeCommand)dlsym(mRkSecureApiHdl, "TEEC_InvokeCommand");

    p = reinterpret_cast<SVPCAImpl*>(rt_mem_calloc("secure_ca", sizeof(SVPCAImpl)));
    if (NULL == p) {
        RT_LOGE("malloc failed\n");
        ret = RT_ERR_MALLOC;
        goto __RETURN;
    }

    /* Initialize a context connecting us to the TEE */
    ret = svp_TEEC_InitializeContext(NULL, &(p->teec_ctx));
    if (ret != TEEC_SUCCESS) {
        RT_LOGE("TEEC_InitializeContext failed with code 0x%x\n", ret);
        ret = RT_ERR_BAD;
        goto __RETURN;
    }

    ret = svp_TEEC_OpenSession(&(p->teec_ctx),
                               &(p->teec_sess),
                               &uuid,
                               TEEC_LOGIN_PUBLIC,
                               NULL,
                               NULL,
                               &err_origin);
    if (ret != TEEC_SUCCESS) {
        RT_LOGE("TEEC_Opensession failed with code 0x%x origin 0x%x\n",
                  ret, err_origin);
        svp_TEEC_FinalizeContext(&(p->teec_ctx));
        ret = RT_ERR_BAD;
        goto __RETURN;
    }

__RETURN:
    if (ret != RT_OK && p) {
        rt_mem_safe_free("secure_ca", reinterpret_cast<void **>(&p));
        p = NULL;
    }
    return ret;
}


RT_RET RtTrust::secure_memcpy_normal(void *dst_normal, uint32_t src_secure, uint32_t size) {
    TEEC_Operation op;
    uint32_t err_origin = 0;
    RT_RET ret = RT_OK;


    memset(&op, 0, sizeof(TEEC_Operation));
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INOUT,
                                     TEEC_VALUE_INOUT,
                                     TEEC_NONE,
                                     TEEC_NONE);
    op.params[0].tmpref.buffer = dst_normal;
    op.params[0].tmpref.size = size;
    op.params[1].value.a = src_secure;
    op.params[1].value.b = size;

    ret = svp_TEEC_InvokeCommand(&(p->teec_sess), TA_SECURE_VIDEO_MEMCPY, &op, &err_origin);
    if (ret != RT_OK) {
        RT_LOGE("Invoke TA_SECURE_VPU_SECURE failed");
        svp_TEEC_FinalizeContext(&(p->teec_ctx));
        svp_TEEC_CloseSession(&(p->teec_sess));
        ret = RT_ERR_BAD;
        goto __RETURN;
    }

__RETURN:
    return ret;
}



RT_RET RtTrust::normal_memcpy_secure(void *src_normal, uint32_t dst_secure, uint32_t size) {
    TEEC_Operation op;
    uint32_t err_origin = 0;
    RT_RET ret = RT_OK;


    memset(&op, 0, sizeof(TEEC_Operation));
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INOUT,
                                     TEEC_VALUE_INOUT,
                                     TEEC_NONE,
                                     TEEC_NONE);
    op.params[0].tmpref.buffer = src_normal;
    op.params[0].tmpref.size = size;
    op.params[1].value.a = dst_secure;
    op.params[1].value.b = size;


    ret = svp_TEEC_InvokeCommand(&(p->teec_sess), TA_SECURE_N_T_S, &op, &err_origin);
    if (ret != TEEC_SUCCESS) {
        RT_LOGE("Invoke TA_SECURE_VPU_SECURE failed");
        svp_TEEC_FinalizeContext(&(p->teec_ctx));
        svp_TEEC_CloseSession(&(p->teec_sess));
        ret = RT_ERR_BAD;
        goto __RETURN;
    }

__RETURN:
    return ret;
}

#endif
