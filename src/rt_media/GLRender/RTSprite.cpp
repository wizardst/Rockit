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

#include "RTSprite.h"        // NOLINT
#include "RTActionManager.h" // NOLINT
#include <string.h>          // NOLINT

RTSprite::RTSprite() {
    mRotationX = mRotationY = 0.0f;
    mPositonX  = mPositonY  = 0.0f;
    mScaleX    = mScaleY    = mScaleZ = mScale = 1.0f;
    mZOrder    = 0;
    mVisible   = RT_TRUE;
    mRunning   = RT_TRUE;
    mOpacity   = 255;
    mTag       = 0;

    mChildren  = RT_NULL;
    mParent    = RT_NULL;
    mUserData  = RT_NULL;
    mActionManager = RT_NULL;
    mCamera    = RT_NULL;
}

void RTSprite::addChild(RTSprite* child) {
    mChildren = child;
}

void RTSprite::addChild(RTSprite* child, INT32 zOrder) {
    this->addChild(child);
    this->setZOrder(zOrder);
}

void RTSprite::addChild(RTSprite* child, INT32 zOrder, INT32 tag) {
    this->addChild(child, zOrder);
    this->setTag(tag);
}

RTSprite* RTSprite::getChildByTag(INT32 tag) const {
    return RT_NULL;
}

INT32 RTSprite::getChildrenCount() const {
    return 0;
}

void RTSprite::setParent(RTSprite* parent) {
    mParent = parent;
}

void RTSprite::removeChild(RTSprite* child, RT_BOOL cleanup /*= true*/) {
    // @TODO(martin)
}

void RTSprite::removeAllChildren() {
    // @TODO(martin)
}

void RTSprite::sortAllChildren() {
    // @TODO(martin)
}

RTAction* RTSprite::runAction(RTAction* action) {
    CHECK_IS_NULL(action);

    if (RT_NULL != this->mActionManager) {
        this->mActionManager->addAction(action, this, !mRunning);
    }

__FAILED:
    return action;
}

void RTSprite::stopAllActions() {
    if (RT_NULL != this->mActionManager) {
        this->mActionManager->removeAllActionsFromTarget(this);
    }
}

void RTSprite::stopAction(RTAction* action) {
    CHECK_IS_NULL(action);

    if (RT_NULL != this->mActionManager) {
        this->mActionManager->removeAction(action);
    }

__FAILED:
    return;
}

void RTSprite::resume() {
    this->mActionManager->resumeTarget(this);
}

void RTSprite::pause() {
    this->mActionManager->pauseTarget(this);
}

void RTSprite::setTag(INT32 tag) {
    this->mTag = tag;
}

void RTSprite::setActionManager(RTActionManager* actionManager) {
    this->mActionManager = actionManager;
}

void RTSprite::setUserData(void *userData) {
    this->mUserData = userData;
}

void RTSprite::setOpacity(UINT8 opacity) {
    this->mOpacity = opacity;
}

void RTSprite::setZOrder(INT32 zOrder) {
    this->mZOrder = zOrder;
}

void RTSprite::setScaleX(RT_FLOAT scaleX) {
    this->mScaleX = scaleX;
}

void RTSprite::setScaleY(RT_FLOAT scaleY) {
    this->mScaleY = scaleY;
}

void RTSprite::setScaleZ(RT_FLOAT scaleZ) {
    this->mScaleZ = scaleZ;
}

void RTSprite::setScale(RT_FLOAT scale) {
    this->mScale  = scale;
    this->mScaleX = this->mScaleY = this->mScaleZ = scale;
}

void RTSprite::setScale(RT_FLOAT scaleX, RT_FLOAT scaleY, RT_FLOAT scaleZ) {
    this->mScaleX = scaleX;
    this->mScaleY = scaleY;
    this->mScaleZ = scaleZ;
}

void RTSprite::setPosition(RT_FLOAT positonX, RT_FLOAT positonY) {
    this->mPositonX = positonX;
    this->mPositonY = positonY;
}

void RTSprite::getPosition(RT_FLOAT* positonX, RT_FLOAT* positonY) const {
    *positonX = this->mPositonX;
    *positonY = this->mPositonY;
}

void RTSprite::setVisible(RT_BOOL visible) {
    this->mVisible = visible;
}

void RTSprite::setRotation(RT_FLOAT rotationX, RT_FLOAT rotationY) {
    this->mRotationX = rotationX;
    this->mRotationY = rotationY;
}

void RTSprite::getRotation(RT_FLOAT* rotationX, RT_FLOAT* rotationY) const {
    *rotationX = this->mRotationX;
    *rotationY = this->mRotationY;
}
