/*
 * Copyright 2019 Rockchip Electronics Co. LTD
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
 * Author: shika.zhou@rock-chips.com
 *   Date: 2019/03/01
 *
 */

#ifndef SRC_RT_MEDIA_ALSALIB_ALSAVOLUMEMANAGER_H_
#define SRC_RT_MEDIA_ALSALIB_ALSAVOLUMEMANAGER_H_

#include "ALSAAapterImpl.h"

#define USER_VOL_MIN     0   // keep some voice
#define USER_VOL_MAX     100

typedef enum _audio_vol_mode {
    ALSA_AUDIO_VOLUME_SET = 0,
    ALSA_AUDIO_VOLUME_GET
} audio_vol_mode;

typedef struct _user_volume_t {
    int     volume;
    bool    is_mute;
} user_volume_t;

class ALSAVolumeManager {
 public:
    ALSAVolumeManager();
    virtual ~ALSAVolumeManager();
    bool IGetMute();
    RT_VOID IMute(bool mute);
    INT32 IGetVolume();
    RT_VOID  ISetVolume(int user_vol);
    int softVolume(int value, audio_vol_mode mode);
    RT_VOID  SetVolumeUtil(int user_vol);

 private:
};


#endif  // SRC_RT_MEDIA_ALSALIB_ALSAVOLUMEMANAGER_H_

