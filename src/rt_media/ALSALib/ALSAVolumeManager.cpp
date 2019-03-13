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

#include "ALSAVolumeManager.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "ALSAVolumeManager"

static user_volume_t  user_volume = {0, false};

ALSAVolumeManager::ALSAVolumeManager() {
}

ALSAVolumeManager::~ALSAVolumeManager() {
}

RT_VOID  ALSAVolumeManager::setVolumeUtil(int user_vol) {
    double k, audio_vol;

    user_vol = check_range(user_vol, USER_VOL_MIN, USER_VOL_MAX);
    /* set volume will unmute */
    if (user_volume.is_mute) {
        user_vol = user_volume.volume;
        user_volume.is_mute = false;
    } else {
        user_volume.volume = user_vol;
    }

    softVolume(user_vol, ALSA_AUDIO_VOLUME_SET);
}

int ALSAVolumeManager::softVolume(int value, audio_vol_mode mode) {
    int ret = 0;
    char vol[128] = {0};

    int size1 = snprintf(vol, sizeof(vol), "%d", value);
    switch (mode) {
        case ALSA_AUDIO_VOLUME_SET:
            alsa_amixer_cset_vol_impl(vol, true);
            break;
        case ALSA_AUDIO_VOLUME_GET:
            ret = alsa_amixer_cset_vol_impl(NULL, false);
            break;
        default:
            break;
    }

    return ret;
}

RT_VOID  ALSAVolumeManager::setVolume(int user_vol) {
    if (user_volume.is_mute) {
       RT_LOGD("Cannot setVolume, Now volume is muted = %d", user_volume.is_mute);
    } else {
       setVolumeUtil(user_vol);
    }
}

INT32 ALSAVolumeManager::getVolume() {
    double k, offset, audio_vol;
    int user_vol = 0;

    user_vol = softVolume(0, ALSA_AUDIO_VOLUME_GET);

    return user_vol;
}

RT_VOID ALSAVolumeManager::setMute(bool mute) {
    if (mute && !user_volume.is_mute) {
        user_volume.is_mute = true;

        softVolume(0, ALSA_AUDIO_VOLUME_SET);

    } else if (!mute && user_volume.is_mute) {
        /* set volume will unmute */
        setVolumeUtil(0);
    }
}

bool ALSAVolumeManager::getMute() {
    return user_volume.is_mute;
}

