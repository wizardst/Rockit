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
 * author: Rimon.Xu@rock-chips.com
 *   date: 20181213
 */

#ifndef SRC_RT_BASE_INCLUDE_RT_METADATA_KEY_H_
#define SRC_RT_BASE_INCLUDE_RT_METADATA_KEY_H_

enum {
    /* common param */
    kKeyWidth       = MKTAG('w', 'i', 't', 'h'),
    kKeyHeight      = MKTAG('h', 'i', 'g', 'h'),

    /* decode and encode */
    kKeyCodingType  = MKTAG('c', 'd', 't', 'p'),

    /* media common */
    kKeyTimeStamps  = MKTAG('t', 's', 't', 'p'),
    kKeyFrameWidth  = MKTAG('f', 'w', 'i', 't'),
    kKeyFrameHeight = MKTAG('f', 'h', 'i', 't'),
    kKeyEOS         = MKTAG('k', 'e', 'o', 's'),
};


#endif  // SRC_RT_BASE_INCLUDE_RT_METADATA_KEY_H_

