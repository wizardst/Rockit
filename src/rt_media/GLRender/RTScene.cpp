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

#include "RTScene.h"         // NOLINT
#include "rt_string_utils.h" // NOLINT

/*
 * RTScene Implementation
 */
RTScene::RTScene() {
    mChildren = RT_NULL;
}

RTScene::~RTScene() {
    // @TODO(martin)
}

void RTScene::render() {
    if (RT_NULL != mChildren) {
        mChildren->draw();
    }
}

void RTScene::removeAllChildren() {
    // @TODO(martin)
}

void RTScene::toString(char* buffer) {
    rt_str_snprintf(buffer, MAX_NAME_LEN, "%s", "RTSprite/RTScene"); // NOLINT
}

void RTScene::summary(char* buffer) {
    this->toString(buffer);
}
