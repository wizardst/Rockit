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

#include <string.h>
#include "rt_metadata.h" // NOLINT
#include "rt_hash_table.h" // NOLINT

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "rt_metadata"

static void MakeFourCCString(uint32_t x, char *s) {
    s[0] = x & 0xff;
    s[1] = (x >> 8) & 0xff;
    s[2] = (x >> 16) & 0xff;
    s[3] = x >> 24;
    s[4] = '\0';
}

struct RtMetaData::typed_data {
 public:
    typed_data();
    ~typed_data();

    explicit typed_data(const RtMetaData::typed_data &);
    typed_data &operator=(const RtMetaData::typed_data &);

    void clear();
    void setData(UINT32 type, const void *data, UINT32 size);
    void getData(UINT32 *type, const void **data, UINT32 *size) const;

 private:
    UINT32 mType;
    UINT32 mSize;

    union {
        void *ext_data;
        float reservoir;
    } u;

    bool usesReservoir() const {
        return mSize <= sizeof(u.reservoir);
    }

    void *allocateStorage(UINT32 size);
    void freeStorage();

    void *storage() {
        return usesReservoir() ? &u.reservoir : u.ext_data;
    }

    const void *storage() const {
        return usesReservoir() ? &u.reservoir : u.ext_data;
    }
};

RtMetaData::RtMetaData() {
    mHashTable = rt_hash_table_create(20, hash_ptr_func, hash_ptr_compare);
}

RtMetaData::~RtMetaData() {
    clear();
    rt_hash_table_destory(mHashTable);
}

void RtMetaData::clear() {
    struct rt_hash_node *list, *node;
    typed_data *item = NULL;

    for (UINT32 bucket = 0; bucket < rt_hash_table_get_num_buckets(mHashTable); bucket++) {
        list = rt_hash_table_get_bucket(mHashTable, bucket);
        for (node = list->next; node != RT_NULL; node = node->next) {
            item = reinterpret_cast<typed_data *>(node->data);
            if (NULL != item) {
                item->clear();
                rt_free(item);
            }
            node->data = RT_NULL;
        }
    }

    rt_hash_table_clear(mHashTable);
}

RT_BOOL RtMetaData::remove(UINT32 key) {
    typed_data *item = NULL;
    void *data = rt_hash_table_find(mHashTable, reinterpret_cast<void*>(key));
    if (NULL == data) {
        RT_LOGE("remove data error from key: 0x%x", key);
        return RT_FALSE;
    }

    item = reinterpret_cast<typed_data *>(data);
    item->clear();
    rt_free(item);
    return rt_hash_table_remove(mHashTable, reinterpret_cast<void*>(key), RT_TRUE);
}

RT_BOOL RtMetaData::setCString(UINT32 key, const char *value) {
    return setData(key, TYPE_C_STRING, value, strlen(value) + 1);
}

RT_BOOL RtMetaData::setInt32(UINT32 key, INT32 value) {
    return setData(key, TYPE_INT32, &value, sizeof(value));
}

RT_BOOL RtMetaData::setInt64(UINT32 key, INT64 value) {
    return setData(key, TYPE_INT64, &value, sizeof(value));
}

RT_BOOL RtMetaData::setFloat(UINT32 key, float value) {
    return setData(key, TYPE_FLOAT, &value, sizeof(value));
}

RT_BOOL RtMetaData::setPointer(UINT32 key, RT_PTR value) {
    return setData(key, TYPE_POINTER, &value, sizeof(value));
}

/**
 * Note that the returned pointer becomes invalid when additional metadata is set.
 */
RT_BOOL RtMetaData::findCString(UINT32 key, const char **value) const {
    UINT32 type;
    const void *data;
    UINT32 size;
    if (!findData(key, &type, &data, &size) || type != TYPE_C_STRING) {
        return RT_FALSE;
    }

    *value = (const char *)data;

    return RT_TRUE;
}

RT_BOOL RtMetaData::findInt32(UINT32 key, INT32 *value) const {
    UINT32 type = 0;
    const void *data;
    UINT32 size;
    void *tmp_value = NULL;
    if (!findData(key, &type, &data, &size) || type != TYPE_INT32) {
        return RT_FALSE;
    }

    CHECK_EQ(size, sizeof(*value));

    tmp_value = const_cast<void *>(data);
    *value = *reinterpret_cast<INT32 *>(tmp_value);


    return RT_TRUE;
__FAILED:
    return RT_FALSE;
}

RT_BOOL RtMetaData::findInt64(UINT32 key, INT64 *value) const {
    UINT32 type = 0;
    const void *data;
    UINT32 size;
    void *tmp_value = NULL;
    if (!findData(key, &type, &data, &size) || type != TYPE_INT64) {
        return RT_FALSE;
    }

    CHECK_EQ(size, sizeof(*value));

    tmp_value = const_cast<void *>(data);
    *value = *reinterpret_cast<INT64 *>(tmp_value);

    return RT_TRUE;
__FAILED:
    return RT_FALSE;
}

RT_BOOL RtMetaData::findFloat(UINT32 key, float *value) const {
    UINT32 type = 0;
    const void *data;
    UINT32 size;
    void *tmp_value = NULL;
    if (!findData(key, &type, &data, &size) || type != TYPE_FLOAT) {
        return RT_FALSE;
    }

    CHECK_EQ(size, sizeof(*value));

    tmp_value = const_cast<void *>(data);
    *value = *reinterpret_cast<float *>(tmp_value);

    return RT_TRUE;
__FAILED:
    return RT_FALSE;
}

RT_BOOL RtMetaData::findPointer(UINT32 key, void **value) const {
    UINT32 type = 0;
    const void *data;
    UINT32 size;
    void *tmp_value = NULL;
    if (!findData(key, &type, &data, &size) || type != TYPE_POINTER) {
        return RT_FALSE;
    }

    CHECK_EQ(size, sizeof(*value));

    tmp_value = const_cast<void *>(data);
    *value = *reinterpret_cast<void **>(tmp_value);

    return RT_TRUE;
__FAILED:
    return RT_FALSE;
}

RT_BOOL RtMetaData::setData(
        UINT32 key, UINT32 type, const void *data, UINT32 size) {
    bool overwrote_existing = true;
    typed_data *item = NULL;
    item = reinterpret_cast<typed_data *>(
               rt_hash_table_find(mHashTable, reinterpret_cast<void*>(key)));
    if (NULL == item) {
        overwrote_existing = false;
        item = rt_malloc(typed_data);
        if (NULL == item) {
            RT_LOGE("malloc type data failed!");
            return RT_FALSE;
        }
        memset(item, 0, sizeof(typed_data));
        rt_hash_table_insert(mHashTable,
                             reinterpret_cast<void*>(key),
                             reinterpret_cast<void*>(item));
    }

    item->setData(type, data, size);

    return overwrote_existing;
}

RT_BOOL RtMetaData::findData(
        UINT32 key,
        UINT32 *type,
        const void **data,
        UINT32 *size) const {
    typed_data *item = NULL;

    item = reinterpret_cast<typed_data *>(
               rt_hash_table_find(mHashTable, reinterpret_cast<void*>(key)));
    if (NULL == item) {
        char string_key[8];
        memset(string_key, 0, sizeof(string_key));
        MakeFourCCString(key, string_key);
        // RT_LOGD("find data failed! key: %s", string_key);
        return RT_FALSE;
    }

    item->getData(type, data, size);

    return RT_TRUE;
}

RT_BOOL RtMetaData::hasData(UINT32 key) const {
    typed_data *item = NULL;

    item = reinterpret_cast<typed_data *>(
               rt_hash_table_find(mHashTable, reinterpret_cast<void*>(key)));
    if (NULL != item) {
        return RT_TRUE;
    } else {
        return RT_FALSE;
    }
}

RtMetaData::typed_data::typed_data()
    : mType(0),
      mSize(0) {
}

RtMetaData::typed_data::~typed_data() {
}

RtMetaData::typed_data::typed_data(const typed_data &from)
    : mType(from.mType),
      mSize(0) {

    void *dst = allocateStorage(from.mSize);
    if (dst) {
        rt_memcpy(dst, const_cast<void *>(from.storage()), mSize);
    }
}

RtMetaData::typed_data &RtMetaData::typed_data::operator=(
        const RtMetaData::typed_data &from) {
    if (this != &from) {
        clear();
        mType = from.mType;
        void *dst = allocateStorage(from.mSize);
        if (dst) {
            rt_memcpy(dst, const_cast<void *>(from.storage()), mSize);
        }
    }

    return *this;
}

void RtMetaData::typed_data::clear() {
    freeStorage();

    mType = 0;
}

void RtMetaData::typed_data::setData(
        UINT32 type, const void *data, UINT32 size) {
    clear();

    mType = type;

    void *dst = allocateStorage(size);
    if (dst) {
        rt_memcpy(dst, const_cast<void *>(data), size);
    }
}

void RtMetaData::typed_data::getData(
        UINT32 *type, const void **data, UINT32 *size) const {
    *type = mType;
    *size = mSize;
    *data = storage();
}

void *RtMetaData::typed_data::allocateStorage(UINT32 size) {
    mSize = size;

    if (usesReservoir()) {
        return &u.reservoir;
    }

    u.ext_data = rt_malloc_size(INT8, mSize);
    if (u.ext_data == NULL) {
        RT_LOGE("Couldn't allocate %zu bytes for item", size);
        mSize = 0;
    }
    memset(u.ext_data, 0, mSize);
    return u.ext_data;
}

void RtMetaData::typed_data::freeStorage() {
    if (!usesReservoir()) {
        if (u.ext_data) {
            rt_free(u.ext_data);
            u.ext_data = NULL;
        }
    }

    mSize = 0;
}

void RtMetaData::dumpToLog() const {
    //# TODO(debug): dump metadata
    return;
}

