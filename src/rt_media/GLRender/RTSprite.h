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

#include "rt_type.h"   // NOLINT
#include "RTObject.h"  // NOLINT

#ifndef SRC_RT_MEDIA_GLRENDER_RTSPRITE_H_
#define SRC_RT_MEDIA_GLRENDER_RTSPRITE_H_

class RTAction;
class RTActionManager;
class RTCamera;

class RTSprite : public RTObject {
 protected:
    RTSprite();
    virtual ~RTSprite();

 public:
    // Sprite Life Cycle Operations
    virtual void addChild(RTSprite* child);
    virtual void addChild(RTSprite* child, INT32 zOrder);
    virtual void addChild(RTSprite* child, INT32 zOrder, INT32 tag);
    virtual RTSprite* getChildByTag(INT32 tag) const;
    virtual INT32 getChildrenCount() const;
    virtual void setParent(RTSprite* parent);
    virtual RTSprite* getParent() { return mParent; }
    virtual void removeChild(RTSprite* child, RT_BOOL cleanup = true);
    virtual void removeAllChildren();
    virtual void sortAllChildren();

    virtual RTAction* runAction(RTAction* action);
    virtual void stopAllActions();
    virtual void stopAction(RTAction* action);
    virtual RTCamera* getCamera() { return mCamera;}

    virtual void resume();
    virtual void pause();

    /**
     * Override this method to draw your own node.
     * The following GL states will be enabled by default:
     * - `glEnableClientState(GL_VERTEX_ARRAY);`
     * - `glEnableClientState(GL_COLOR_ARRAY);`
     * - `glEnableClientState(GL_TEXTURE_COORD_ARRAY);`
     * - `glEnable(GL_TEXTURE_2D);`
     * AND YOU SHOULD NOT DISABLE THEM AFTER DRAWING YOUR NODE
     * But if you enable any other GL state, you should disable it after drawing your node.
     *
     */
    virtual void draw() = 0;

    // Sprite Property Operations
    virtual void setTag(INT32 tag);
    virtual INT32 getTag() const { return mTag; }

    virtual void setActionManager(RTActionManager* actionManager);
    virtual RTActionManager* getActionManager() { return mActionManager; }

    virtual void setUserData(void *userData);
    virtual const void* getUserData() const { return mUserData; }

    virtual void setOpacity(UINT8 opacity);
    virtual UINT8 getOpacity() const { return mOpacity; }

    virtual void setZOrder(INT32 localZOrder);
    virtual INT32 getZOrder() const { return mZOrder; }

    virtual void setScaleX(RT_FLOAT scaleX);
    virtual RT_FLOAT getScaleX() const {return mScaleX; }

    virtual void setScaleY(RT_FLOAT scaleY);
    virtual RT_FLOAT getScaleY() const {return mScaleX; }

    virtual void setScaleZ(RT_FLOAT scaleZ);
    virtual RT_FLOAT getScaleZ() const {return mScaleX; }

    virtual void setScale(RT_FLOAT scale);
    virtual RT_FLOAT getScale() const {return mScale; }

    virtual void setScale(RT_FLOAT scaleX, RT_FLOAT scaleY, RT_FLOAT scaleZ);

    virtual void setPosition(RT_FLOAT x, RT_FLOAT y);
    virtual void getPosition(RT_FLOAT* x, RT_FLOAT* y) const;

    virtual void setVisible(RT_BOOL visible);
    virtual RT_BOOL isVisible() const { return mVisible; }

    virtual void setRotation(RT_FLOAT rotationX, RT_FLOAT rotationY);
    virtual void getRotation(RT_FLOAT* rotationX, RT_FLOAT* rotationY) const;

 private:
    RT_FLOAT mRotationX;            ///< rotation on the X-axis
    RT_FLOAT mRotationY;            ///< rotation on the Y-axis

    RT_FLOAT mPositonX;             ///< position on the X-axis
    RT_FLOAT mPositonY;             ///< position on the Y-axis

    RT_FLOAT mScaleX;               ///< scaling factor on x-axis
    RT_FLOAT mScaleY;               ///< scaling factor on y-axis
    RT_FLOAT mScaleZ;               ///< scaling factor on z-axis
    RT_FLOAT mScale;               ///< scaling factor

    INT32    mZOrder;               ///< Local order used to sort the sprite

    RT_BOOL  mVisible;              ///< sprite visible
    RT_BOOL  mRunning;              ///< sprite running
    UINT8    mOpacity;              ///< sprite opacity
    INT32    mTag;                  ///< just to identify this sprite

 protected:
    RTSprite        *mChildren;       ///< array of children nodes
    RTSprite        *mParent;         ///< parent node
    void            *mUserData;       ///< use data for shader
    RTCamera        *mCamera;
    RTActionManager *mActionManager;  ///< a pointer to ActionManager singleton, which is used to handle all the actions
};

#endif  // SRC_RT_MEDIA_GLRENDER_RTSPRITE_H_
