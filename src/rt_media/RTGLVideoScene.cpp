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

#include "include/RTGLVideoScene.h"
#include "GLRender/RTSpriteVideo.h"
#include "GLRender/RTAction.h"

RTGLVideoScene::RTGLVideoScene() {
    init();
}

RTGLVideoScene::~RTGLVideoScene() {
    // TODO(@martin)
    rt_safe_free(mChildren);
}

void RTGLVideoScene::init() {
    mChildren = new RTSpriteVideo();
}

void RTGLVideoScene::updateFrame(UCHAR* frame, INT32 width, INT32 height) {
    RTSpriteVideo* sprite_video = reinterpret_cast<RTSpriteVideo*>(mChildren);
    sprite_video->updateFrame(frame, width, height);
}
