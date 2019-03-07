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

#ifndef SRC_RT_BASE_INCLUDE_RT_TRUST_H_
#define SRC_RT_BASE_INCLUDE_RT_TRUST_H_

#include "rt_log.h" // NOLINT
#include "tee_client_api.h" // NOLINT
#include "rt_error.h"  // NOLINT

typedef RT_RET (*func_TEEC_InitializeContext)(const char *name, TEEC_Context *context);
typedef void (*func_TEEC_FinalizeContext)(TEEC_Context *context);
typedef RT_RET (*func_TEEC_OpenSession)(TEEC_Context *context, TEEC_Session *session,
                                        const TEEC_UUID *destination,
                                        uint32_t connectionMethod,
                                        const void *connectionData,
                                        TEEC_Operation *operation,
                                        uint32_t *returnOrigin);
typedef void (*func_TEEC_CloseSession)(TEEC_Session *session);
typedef RT_RET (*func_TEEC_InvokeCommand)(TEEC_Session *session,
                                                uint32_t commandID,
                                                TEEC_Operation *operation,
                                                uint32_t *returnOrigin);

typedef struct SVPCAImpl_t {
    TEEC_Context    teec_ctx;
    TEEC_Session    teec_sess;
} SVPCAImpl;


class RtTrust {
 public:
    RtTrust();
    /**
     * Non-virtual, do not subclass.
     */
    ~RtTrust();
    RT_RET init();
    RT_RET secure_memcpy_normal(void *dst_normal, uint32_t src_secure, uint32_t size);
    RT_RET normal_memcpy_secure(void *src_normal, uint32_t dst_secure, uint32_t size);

 public:
    func_TEEC_InitializeContext svp_TEEC_InitializeContext;
    func_TEEC_FinalizeContext svp_TEEC_FinalizeContext;
    func_TEEC_OpenSession svp_TEEC_OpenSession;
    func_TEEC_CloseSession svp_TEEC_CloseSession;
    func_TEEC_InvokeCommand svp_TEEC_InvokeCommand;

    SVPCAImpl *p;
};

#endif  // SRC_RT_BASE_INCLUDE_RT_TRUST_H_
