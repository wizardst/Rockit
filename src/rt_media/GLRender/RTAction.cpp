/*
 * Copyright 2018 Rockchip Electronics Co-> LTD
 *
 * Licensed under the Apache License, Version 2->0 (the "License");
 * you may not use this file except in compliance with the License->
 * You may obtain a copy of the License at
 *
 *      http://www->apache->org/licenses/LICENSE-2->0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied->
 * See the License for the specific language governing permissions and
 * limitations under the License->
 *
 * author: martin->cheng@rock-chips->com
 *   date: 20181219
 * module: render engine for OpenGL-ES2.0
 */

#include "RTAction.h"        // NOLINT
#include "RTSprite.h"        // NOLINT
#include "rt_string_utils.h" // NOLINT

#define DURATION_MIN 0.001
#define UNSET_VALUE -1000.0f

void RTAction::startWithTarget(RTSprite *target) {
    this->mTarget = target;
}

void RTAction::stop() {
    this->mTarget = RT_NULL;
}

void RTAction::step(float dt) {
    RT_LOGE("[Action STEP]. override me");
}

void RTAction::update(float time) {
    RT_LOGE("[Action update]. override me");
}

/*
 *  ActionTime Implementation
 *  2018-12-21
 */
void RTActionTime::setDuration(RT_FLOAT duration) {
    mDuration = duration;
    if (mDuration <= DURATION_MIN) {
        mDuration = DURATION_MIN;
    }
    mElapsed = 0.0f;
}

void RTActionTime::step(RT_FLOAT dt) {
    mElapsed += dt;
    this->update(mElapsed/mDuration);
}

void RTActionTime::startWithTarget(RTSprite *target) {
    RTAction::startWithTarget(target);
    mElapsed = 0.0f;
}

/*
 *  RTActionSequence Implementation
 *  2018-12-21
 */
RTActionSequence::RTActionSequence() {
    mSequence = array_list_create();
}

RTActionSequence::~RTActionSequence() {
    array_list_destroy(mSequence);
    mSequence = RT_NULL;
}

void RTActionSequence::addAction(RTActionTime* action) {
     array_list_add(mSequence, reinterpret_cast<void*>(action));
}

void RTActionSequence::startWithTarget(RTSprite *target) {
    // @TODO(martin)
}

void RTActionSequence::stop(void) {
    // @TODO(martin)
}

void RTActionSequence::update(RT_FLOAT dt) {
    // @TODO(martin)
}

/*
 *  RTRepeatForever Implementation
 *  2018-12-21
 */
RTRepeatForever::RTRepeatForever(RTActionTime* action, RT_BOOL _reverse) {
    mInnerAction = action;
    mReverse     = _reverse;
}

RTRepeatForever::~RTRepeatForever() {
    mInnerAction = RT_NULL;
    // rt_safe_delete(mInnerAction);
}

void RTRepeatForever::startWithTarget(RTSprite *target) {
    RTActionTime::startWithTarget(target);
    mInnerAction->startWithTarget(target);
}

RT_BOOL RTRepeatForever::isDone(void) {
    return RT_FALSE;
}

void RTRepeatForever::step(RT_FLOAT dt) {
    mInnerAction->step(dt);
    if (mInnerAction->isDone()) {
        if (RT_TRUE == mReverse) {
            mInnerAction->autoReverse();
        }
        mInnerAction->step(0.0f);
    }
}

/*
 *  RTCompound Implementation
 *  2018-12-21
 */
RTCompound::RTCompound() {
    RTObject::trace(getName(), this, sizeof(RTCompound));

    mSrcOpacity = mDstOpacity = 255;
    mSrcScaleX  = mSrcScaleY  = mSrcScaleZ  = UNSET_VALUE;
    mDstScaleX  = mDstScaleY  = mDstScaleZ  = UNSET_VALUE;
    mDstRotateX = mDstRotateY = mDstRotateZ = UNSET_VALUE;
    mDstRotateX = mDstRotateY = mDstRotateZ = UNSET_VALUE;
    mSrcPositionX = mSrcPositionY = mSrcPositionZ = UNSET_VALUE;
    mDstPositionX = mDstPositionY = mDstPositionZ = UNSET_VALUE;
}

RTCompound::~RTCompound() {
    RTObject::untrace(getName(), this);
    // TODO(@martin)
}

void RTCompound::scaleTo(RT_FLOAT sx, RT_FLOAT sy, RT_FLOAT sz) {
    mDstScaleX = sx; mDstScaleY = sy; mDstScaleZ = sz;
}

void RTCompound::translatTo(RT_FLOAT tx, RT_FLOAT ty, RT_FLOAT tz) {
    mDstPositionX = tx; mDstPositionY = ty; mDstPositionZ = tz;
}

void RTCompound::rotateTo(RT_FLOAT rx, RT_FLOAT ry, RT_FLOAT rz) {
    mDstRotateX  = rx; mDstRotateY = ry; mDstRotateZ = rz;
}

void RTCompound::fadeTo(UINT8 to) {
    mDstOpacity = to;
}

void RTCompound::startWithTarget(RTSprite *target) {
    RTActionTime::startWithTarget(target);
    if (RT_NULL != this->mTarget) {
        mSrcOpacity = mTarget->getOpacity();
        mSrcScaleX  = mTarget->getScaleX();
        mSrcScaleY  = mTarget->getScaleY();
        mSrcScaleZ  = mTarget->getScaleZ();

        mTarget->getPosition(&mSrcPositionX, &mSrcPositionY);
        mTarget->getRotation(&mDstRotateX, &mDstRotateY);
    }
}

void RTCompound::update(float time) {
    RT_FLOAT pos_x = mSrcPositionX + (mDstPositionX - mSrcPositionX) * time;
    RT_FLOAT pos_y = mSrcPositionY + (mDstPositionY - mSrcPositionY) * time;
    if (RT_NULL != this->mTarget) {
        mTarget->setOpacity((UINT8)(mSrcOpacity + (mDstOpacity - mSrcOpacity) * time));
        mTarget->setScaleX(mSrcScaleX + (mDstScaleX - mSrcScaleX) * time);
        mTarget->setScaleY(mSrcScaleY + (mDstScaleX - mSrcScaleX) * time);
        mTarget->setScaleZ(mSrcScaleZ + (mDstScaleZ - mSrcScaleZ) * time);

        mTarget->setPosition(pos_x, pos_y);
    }
}
