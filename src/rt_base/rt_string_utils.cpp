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
 *   date: 20181126
 */

#include "rt_string_utils.h" // NOLINT
#include <string>            // NOLINT

#define MAX_NAME_LEN 12

INT32 rt_str_vsnprintf(char* buffer, size_t max_len,
                             const char* format, va_list args) {
    return vsnprintf(buffer, max_len, format, args);
}

INT32 rt_str_snprintf(char* buffer, size_t max_len,
                            const char* format, ...) {
    va_list args;
    va_start(args, format);
    INT32 err = vsnprintf(buffer, max_len, format, args);
    va_end(args);
    return err;
}

std::string rt_str_vsprintf(const char* format, va_list args) {
    va_list tmp_args;  // va_list can't change, so copy it
    va_copy(tmp_args, args);
    const int required_len = vsnprintf(NULL, 0, format, tmp_args) + 1;
    va_end(tmp_args);

    std::string buf(required_len, '\0');
    if (vsnprintf(&buf[0], buf.size(), format, args) < 0) {
        // throw runtime_error{"rt_str_vsprintf encoding error"};
    }
    return buf;
}

std::string rt_str_sprintf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    std::string str(rt_str_vsprintf(format, args));
    va_end(args);
    return str;
}

const char* rt_str_to_char(const std::string buf) {
    return buf.c_str();
}
