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
 * author: Rimon.Xu@rock-chips.com
 *   date: 20181205
 */

#ifndef SRC_RT_BASE_INCLUDE_RT_METADATA_H_
#define SRC_RT_BASE_INCLUDE_RT_METADATA_H_

#include <stdint.h>
#include "rt_header.h" // NOLINT

struct RtHashTable;

class RtMetaData {
 public:
    RtMetaData();
    RtMetaData(const RtMetaData &from);
    RtMetaData& operator = (const RtMetaData &);

    virtual ~RtMetaData();

    enum Type {
        TYPE_NONE     = MKTAG('n', 'o', 'n', 'e'),
        TYPE_C_STRING = MKTAG('c', 's', 't', 'r'),
        TYPE_INT32    = MKTAG('i', 'n', '3', '2'),
        TYPE_INT64    = MKTAG('i', 'n', '6', '4'),
        TYPE_FLOAT    = MKTAG('f', 'l', 'o', 'a'),
        TYPE_POINTER  = MKTAG('p', 'n', 't', 'r'),
    };

    void clear();
    RT_BOOL remove(UINT32 key);

    RT_BOOL setCString(UINT32 key, const char *value);
    RT_BOOL setInt32(UINT32 key, INT32 value);
    RT_BOOL setInt64(UINT32 key, INT64 value);
    RT_BOOL setFloat(UINT32 key, float value);
    RT_BOOL setPointer(UINT32 key, RT_PTR value);

    RT_BOOL findCString(UINT32 key, const char **value) const;
    RT_BOOL findInt32(UINT32 key, INT32 *value) const;
    RT_BOOL findInt64(UINT32 key, INT64 *value) const;
    RT_BOOL findFloat(UINT32 key, float *value) const;
    RT_BOOL findPointer(UINT32 key, RT_PTR *value) const;

    RT_BOOL setData(UINT32 key, UINT32 type, const void *data, UINT32 size);

    RT_BOOL findData(UINT32 key, UINT32 *type,
                  const void **data, UINT32 *size) const;

    RT_BOOL hasData(UINT32 key) const;

    void dumpToLog() const;

 private:
    struct typed_data;
    RtHashTable *mHashTable;
};

#endif  // SRC_RT_BASE_INCLUDE_RT_METADATA_H_

