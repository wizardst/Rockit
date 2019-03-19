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
 *   Date: 2019/01/30
 */

#ifndef SRC_RT_MEDIA_ALSALIB_ALSAAAPTERIMPL_H_
#define SRC_RT_MEDIA_ALSALIB_ALSAAAPTERIMPL_H_

#include <alsa/asoundlib.h>
#include <alsa/pcm.h>
#include "rt_mem.h"          // NOLINT
#include "rt_log.h"          // NOLINT
#include "rt_common.h"       // NOLINT
#include "ALSAAapterUtils.h" // NOLINT

#define DEFAULT_OUT_LATENCY     139319
#define DEFAULT_OUT_BUFFERSIZE  4096
#define DEFAULT_OUT_PERIODSIZE  1024
#define DEFAULT_OUT_PERIODS     4
#define DEFAULT_SAMPLE_RATE     44100

typedef struct AlsaParamsContext_ {
    snd_pcm_format_t    format;
    UINT32              channels;
    UINT32              sampleRate;
    unsigned int        latency;
    snd_pcm_uframes_t   periodSize;
    snd_pcm_uframes_t   bufferSize;
    int                 periods;
} AlsaParamsContext;

typedef struct ALSASinkContext_ {
    AlsaParamsContext   *mAlsaParamsCtx;
    snd_pcm_t           *theInstance;
} ALSASinkContext;

RT_RET alsa_set_snd_hw_params(ALSASinkContext *ctx, int flag);

RT_RET alsa_set_snd_sw_params(ALSASinkContext *ctx);

int alsa_snd_write_data(ALSASinkContext *ctx, void *data, int bytes);

ALSASinkContext* alsa_snd_create(const char *name, RtMetaData *metadata);

RT_VOID alsa_snd_destroy(ALSASinkContext *ctx);

int alsa_amixer_cset_vol_impl(char *audio_vol, bool roflag);

#endif  // SRC_RT_MEDIA_ALSALIB_ALSAAAPTERIMPL_H_

