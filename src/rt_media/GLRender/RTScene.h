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
 *   date: 20181221
 */
#ifndef SRC_RT_MEDIA_GLRENDER_RTSCENE_H_
#define SRC_RT_MEDIA_GLRENDER_RTSCENE_H_

#include "rt_type.h"   // NOLINT
#include "RTSprite.h"  // NOLINT

class RTSprite;
class RTScene : public RTSprite {
 public:
    RTScene();
    virtual ~RTScene();
    /** Render the scene.*/
    void render();
    /** override function */
    virtual void removeAllChildren();

    // override @RTSprite methods
    void draw() {}
    virtual void init() = 0;

    // override @RTObject methods
    void toString(char* buffer);
    void summary(char* buffer);
 protected:
    friend class RTSprite;
};

#endif  // SRC_RT_MEDIA_GLRENDER_RTSCENE_H_
