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


#include "RTObject.h"      // NOLINT

#include "rt_type.h"       // NOLINT
#include "rt_array_list.h" // NOLINT

#ifndef SRC_RT_MEDIA_GLRENDER_RTACTION_H_
#define SRC_RT_MEDIA_GLRENDER_RTACTION_H_

 /** @class RTAction
 * @brief Base class for Action objects.
 * Possible actions:
 * - An action with a duration of 00.1 seconds.
 * - An action with a duration of 20.0 seconds.
 * - An action with a speed 1.5f or 0.5f
 */
class RTSprite;
class RTAction : public RTObject {
 public:
    virtual void startWithTarget(RTSprite *target);
    virtual void stop();
    virtual void step(RT_FLOAT dt);
    virtual void update(RT_FLOAT time);

    void setTag(INT32 tag) {mTag = tag;}
    INT32 getTag() const { return mTag; }

    void setTarget(RTSprite *target) { mTarget = target;}
    RTSprite* getTarget() const { return mTarget; }

    RT_FLOAT getSpeed(void) const { return mSpeed; }
    void setSpeed(RT_FLOAT speed) { mSpeed = speed; }

 protected:
    RTSprite *mTarget;
    INT32     mTag;
    //! Speed in seconds
    RT_FLOAT  mSpeed;
};

class RTActionTime : public RTAction {
 public:
    virtual RT_BOOL isDone(void) { return mElapsed >= mDuration;}

    /** Get duration in seconds of the action.
     *
     * @return The duration in seconds.
     */
    RT_FLOAT getDuration() const { return mDuration; }
    /** Set duration in seconds of the action.
     *
     * @param The duration in seconds.
     */
    void setDuration(RT_FLOAT duration);
    /** Get time in seconds elapsed since the actions started
     *
     * @return The seconds had elapsed.
     */
    RT_FLOAT getElapsed(void) { return mElapsed; }

    virtual void startWithTarget(RTSprite *target);
    virtual void autoReverse() {}
    virtual void step(RT_FLOAT dt);

 protected:
    //! Elapsed tine in seconds
    RT_FLOAT  mElapsed;
    //! Duration in seconds.
    RT_FLOAT  mDuration;
};

/** @class RTActionSequence
 * @brief Runs actions sequentially, one after another.
 */
class RTActionSequence : public RTActionTime {
 public:
    RTActionSequence();
    ~RTActionSequence();

 public:
    virtual void addAction(RTActionTime* action);
    virtual void startWithTarget(RTSprite *target);
    virtual void stop(void);
    virtual void update(RT_FLOAT dt);

 private:
    RtArrayList*  mSequence;
};

class RTRepeatForever : public RTActionTime {
 public:
    RTRepeatForever(RTActionTime* action, RT_BOOL _reverse);
    ~RTRepeatForever();

 public:
    virtual void startWithTarget(RTSprite *target);

    // @override parent methods
    virtual RT_BOOL isDone(void);
    virtual void step(RT_FLOAT dt);
    // virtual void update(RT_FLOAT dt);

 private:
    RTActionTime* mInnerAction;
    RT_BOOL       mReverse;
};

class RTCompound  : public RTActionTime {
 public:
    RTCompound();
    ~RTCompound();

 public:
    virtual void scaleTo(RT_FLOAT sx, RT_FLOAT sy, RT_FLOAT sz);
    virtual void translatTo(RT_FLOAT tx, RT_FLOAT ty, RT_FLOAT tz);
    virtual void rotateTo(RT_FLOAT rx, RT_FLOAT ry, RT_FLOAT rz);
    virtual void fadeTo(UINT8 to);

    // @override parent methods
    virtual void startWithTarget(RTSprite *target);
    // virtual RT_BOOL isDone(void);
    // virtual void step(RT_FLOAT dt);
    virtual void update(RT_FLOAT dt);

    // override @RTObject methods
    void toString(char* buffer);
    void summary(char* buffer);

 private:
    RTActionTime* mInnerAction;

    UINT8    mSrcOpacity,   mDstOpacity;
    RT_FLOAT mSrcScaleX,    mSrcScaleY,    mSrcScaleZ;
    RT_FLOAT mDstScaleX,    mDstScaleY,    mDstScaleZ;
    RT_FLOAT mSrcPositionX, mSrcPositionY, mSrcPositionZ;
    RT_FLOAT mDstPositionX, mDstPositionY, mDstPositionZ;
    RT_FLOAT mSrcRotateX,   mSrcRotateY,   mSrcRotateZ;
    RT_FLOAT mDstRotateX,   mDstRotateY,   mDstRotateZ;
};

#endif  // SRC_RT_MEDIA_GLRENDER_RTACTION_H_
