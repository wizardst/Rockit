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
 *   date: 20181218
 */

#include "rt_type.h"         // NOLINT
#include "rt_string_utils.h" // NOLINT

#ifndef SRC_RT_BASE_INCLUDE_RTOBJECT_H_
#define SRC_RT_BASE_INCLUDE_RTOBJECT_H_

#define MAX_NAME_LEN 32

class RTObject {
 public:
    virtual void toString(char* buffer) = 0;
    virtual void summary(char* buffer)  = 0;
    virtual void setName(const char* name);
    virtual const char* getName( ) { return mName; }

 protected:
    RTObject();
    ~RTObject();

 private:
    char mName[MAX_NAME_LEN];
};

#endif  // SRC_RT_BASE_INCLUDE_RTOBJECT_H_
