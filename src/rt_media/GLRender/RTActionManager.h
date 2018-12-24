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

#ifndef SRC_RT_MEDIA_GLRENDER_RTACTIONMANAGER_H_
#define SRC_RT_MEDIA_GLRENDER_RTACTIONMANAGER_H_

#include "rt_header.h"  // NOLINT
#include "RTObject.h"   // NOLINT

class RTAction;
class RTSprite;
class RTActionManager : public RTObject {
 public:
    RTActionManager();
    ~RTActionManager();

    /** Returns a string representation of the object.
      *
      * @param buffer    buffer to save representation.
      */
    virtual void toString(char* buffer);

    /** Returns a string summary of the object.
      *
      * @param buffer    buffer to save summary.
      */
    virtual void summary(char* buffer);

    /** Adds an action with a target.
     *
     * @param action    A certain action.
     * @param target    The target which need to be added an action.
     * @param paused    Is the target paused or not.
     */
    void addAction(RTAction *action, RTSprite *target, RT_BOOL paused);

    /** Removes all actions from all the targets.
     */
    void removeAllActions();

    /** Removes all actions from a certain target.
     All the actions that belongs to the target will be removed.
     *
     * @param target    A certain target.
     */
    void removeAllActionsFromTarget(RTSprite *target);

    /** Removes an action given an action reference.
     *
     * @param action    A certain target.
     */
    void removeAction(RTAction *action);

    /** Removes an action given its tag and the target.
     *
     * @param tag       The action's tag.
     * @param target    A certain target.
     */
    void removeActionByTag(int tag, RTSprite *target);


    /** Pauses the target: all running actions and newly added actions will be paused.
     *
     * @param target    A certain target.
     */
    void pauseTarget(RTSprite *target);

    /** Resumes the target. All queued actions will be resumed.
     *
     * @param target    A certain target.
     */
    void resumeTarget(RTSprite *target);

    /** Main loop of ActionManager.
     * @param dt    In seconds.
     */
    void update(float dt);

 public:
    struct RtHashTable  *mActionTable;
};

#endif  // SRC_RT_MEDIA_GLRENDER_RTACTIONMANAGER_H_

