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

#ifndef SRC_RT_MEDIA_INCLUDE_RTOBJECT_H_
#define SRC_RT_MEDIA_INCLUDE_RTOBJECT_H_

#include <typeinfo>
#include "rt_type.h"  // NOLINT

class RTMemService;
class RTObject {
 public:
    virtual void summary(INT32 fd) = 0;
    virtual const char* getName() = 0;
    static void dumpTraces();                // Called when runtime env inits
    static void resetTraces();               // Called when runtime env exits
 protected:
    void trace(const char* name, void* ptr, UINT32 size);  // Called in Constructor
    void untrace(const char* name, void* ptr);  // Called in Destructor
 private:
    static RTMemService* mObjTraces;
};

#endif   // SRC_RT_MEDIA_INCLUDE_RTOBJECT_H_
