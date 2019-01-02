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
 * Author: martin.cheng@rock-chips.com
 *   Date: 2018/12/26
 */

#include <string.h>
#include "include/RTObject.h" // NOLINT
#include "RTMemService.h" // NOLINT
#include "rt_log.h" // NOLINT

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "RTObject"
#ifdef DEBUG_FLAG
#undef DEBUG_FLAG
#endif
#define DEBUG_FLAG 0x0

RTMemService* RTObject::mObjTraces = RT_NULL;

RT_BOOL RTObject::equals(RTObject* ptr) {
    UINT32 hash = RTObject::jvmHashCode(typeid(ptr).name());
    return this->hashCode() == hash;
}

UINT32 RTObject::hashCode() {
    return RTObject::jvmHashCode(typeid(this).name());
}

void RTObject::dumpTraces() {
    if (RT_NULL != mObjTraces) {
        mObjTraces->dump();
    }
}

void RTObject::resetTraces() {
    if (RT_NULL != mObjTraces) {
        mObjTraces->reset();
    }
}

// By default, the hash function of the java virtual machine is used.
UINT32 RTObject::jvmHashCode(const char* name) {
    const char *str = name;
    UINT32 hash = 0;

    while (*str != '\0') {
        hash = (hash * 31) + *str;
        // hash = (hash << 5) - hash + *str;
        str++;
    }

    // FORCE hash code is positive
    hash = hash & 0x7FFFFFFF;

    return hash;
}

void RTObject::trace(const char* name, void* ptr, UINT32 size) {
    if ((RT_NULL == mObjTraces)&&(!DEBUG_FLAG)) {
        mObjTraces = new RTMemService();
        mObjTraces->reset();
    }
    // TODO(@martin) : debug object
    if (RT_NULL != mObjTraces) {
        mObjTraces->addNode(name, ptr, size);
    } else {
        RT_LOGD_IF(DEBUG_FLAG, "TRACE %s(ptr=%p,size=%03d)", name, ptr, size);
    }
}

void RTObject::untrace(const char* name, void* ptr) {
    // TODO(@martin) : debug object
    UINT32 size;
    if (RT_NULL != mObjTraces) {
        mObjTraces->removeNode(ptr, &size);
    } else {
        RT_LOGD_IF(DEBUG_FLAG, "CLEAR %s(ptr=%p)", name);
    }
}
