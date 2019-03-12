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

#ifndef SRC_RT_MEDIA_ALSALIB_ALSAAAPTERUTILS_H_
#define SRC_RT_MEDIA_ALSALIB_ALSAAAPTERUTILS_H_

#include <math.h>
#include <alsa/asoundlib.h>
#include <alsa/pcm.h>
#include "rt_header.h" // NOLINT

#define SOFTVOL_ELEM "name=Master Playback Volume"
// #define WRITE_DEVICE_NAME "softvol"
// #define WRITE_DEVICE_NAME "default"   // linux-3308
#define WRITE_DEVICE_NAME "hw:1,0"       // android

#undef isdigit
#define isdigit(c) ((unsigned int) ((c) - '0') < 10)

#define convert_prange1(val, min, max) \
        ceil((val) * ((max) - (min)) * 0.01 + (min))

#define check_range(val, min, max) \
        ((val < min) ? (min) : (val > max) ? (max) : (val))

int parse_control_id(const char *str, snd_ctl_elem_id_t *id);
void show_control_id(snd_ctl_elem_id_t *id);
INT32 get_integer(char **ptr, INT32 min, INT32 max);
const char *control_iface(snd_ctl_elem_id_t *id);

#endif  // SRC_RT_MEDIA_ALSALIB_ALSAAAPTERUTILS_H_

