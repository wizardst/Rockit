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
 * module: render engine for OpenGL-ES2.0
 */

#include "RTActionManager.h"  // NOLINT
#include "RTAction.h"         // NOLINT

#include "rt_hash_table.h"    // NOLINT
#include "rt_string_utils.h"  // NOLINT

typedef struct _hashElement {
    RTSprite   *target;
    RTAction   *action;
    RT_BOOL     paused;
} tHashElement;

RTActionManager::RTActionManager() {
    RTObject::trace(getName(), this, sizeof(RTActionManager));

    mActionTable = rt_hash_table_create(12, hash_ptr_func, hash_ptr_compare);
}

RTActionManager::~RTActionManager() {
    RTObject::untrace(getName(), this);
    rt_hash_table_clear(this->mActionTable);
    rt_hash_table_destory(this->mActionTable);
}

void RTActionManager::addAction(RTAction *action, RTSprite *target, RT_BOOL paused) {
    tHashElement *element = rt_malloc(tHashElement);
    CHECK_IS_NULL(element);

    element->target = target;
    element->action = action;
    element->paused = paused;

    rt_hash_table_insert(this->mActionTable, target, element);

    action->startWithTarget(target);

__FAILED:
    return;
}

void RTActionManager::removeAllActions() {
    UINT32               idx = 0;
    tHashElement*        item;
    struct rt_hash_node *list, *node;

    for (idx = 0; idx < rt_hash_table_get_num_buckets(this->mActionTable); idx++) {
        list = rt_hash_table_get_bucket(this->mActionTable, idx);
        for (node = list->next; node != RT_NULL; node = node->next) {
            item  = reinterpret_cast<tHashElement*>(node->data);
            rt_safe_free(item);
        }
    }
    rt_hash_table_clear(this->mActionTable);
}

void RTActionManager::removeAllActionsFromTarget(RTSprite *target) {
    return;
}

void RTActionManager::removeAction(RTAction *action) {
    return;
}

void RTActionManager::removeActionByTag(int tag, RTSprite *target) {
    return;
}

void RTActionManager::pauseTarget(RTSprite *target) {
    return;
}

void RTActionManager::resumeTarget(RTSprite *target) {
    return;
}

void RTActionManager::update(float dt) {
    return;
}
