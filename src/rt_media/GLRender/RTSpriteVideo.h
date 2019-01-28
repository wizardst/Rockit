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

#include "RTSprite.h"  // NOLINT

#ifndef SRC_RT_MEDIA_GLRENDER_RTSPRITEVIDEO_H_
#define SRC_RT_MEDIA_GLRENDER_RTSPRITEVIDEO_H_

class RTSpriteVideo : public RTSprite {
 public:
    RTSpriteVideo();

    ~RTSpriteVideo();

    // override pure virtual methods of RTObject class
    virtual void summary(INT32 fd) {}
    virtual const char* getName() { return typeid(this).name(); }

    virtual void draw();
    virtual void update();

    void updateFrame(UCHAR* frame, INT32 width, INT32 height);

 private:
    virtual void initUserData();
};

#endif  // SRC_RT_MEDIA_GLRENDER_RTSPRITEVIDEO_H_
