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
 * Module: Package ALSA-lib APi
 */

#include <string.h>          // NOLINT
#include "ALSAAapterImpl.h"  // NOLINT
#include "RTMediaMetaKeys.h" // NOLINT
#include "rt_metadata.h"     // NOLINT
#include "RTMediaBuffer.h"   // NOLINT

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "ALSAAapterImpl"
#ifdef DEBUG_FLAG
#undef DEBUG_FLAG
#endif
#define DEBUG_FLAG 0x1

RT_RET alsa_set_snd_hw_params(ALSASinkContext *ctx, int flag) {
    snd_pcm_hw_params_t *hardwareParams;
    int err;

    snd_pcm_uframes_t bufferSize = ctx->mAlsaParamsCtx->bufferSize;
    snd_pcm_uframes_t periodSize = ctx->mAlsaParamsCtx->periodSize;
    unsigned int requestedRate = ctx->mAlsaParamsCtx->sampleRate;
    unsigned int latency = ctx->mAlsaParamsCtx->latency;
    unsigned int periods = ctx->mAlsaParamsCtx->periods;
    unsigned int periodTime = 0;

    bool validFormat = (static_cast<int> (ctx->mAlsaParamsCtx->format)
                         > SND_PCM_FORMAT_UNKNOWN) && (static_cast<int> (ctx->mAlsaParamsCtx->format)
                             <= SND_PCM_FORMAT_LAST);
    const char *formatDesc = validFormat ? snd_pcm_format_description(
                                 ctx->mAlsaParamsCtx->format) : "Invalid Format";
    const char *formatName = validFormat ? snd_pcm_format_name(ctx->mAlsaParamsCtx->format)
                             : "UNKNOWN";

    if (snd_pcm_hw_params_malloc(&hardwareParams) < 0) {
        RT_LOGE("Failed to allocate ALSA hardware parameters!");
        return RT_ERR_INIT;
    }

    if (snd_pcm_hw_params_any(ctx->theInstance, hardwareParams) < 0) {
        RT_LOGE("Unable to configure hardware: %s", snd_strerror(err));
        goto done;
    }

    // Set the interleaved read and write format.
    err = snd_pcm_hw_params_set_access(ctx->theInstance, hardwareParams, SND_PCM_ACCESS_RW_INTERLEAVED);
    if (err < 0) {
        RT_LOGE("Error setting interleaved mode: %s", snd_strerror(err));
        goto done;
    }

    err = snd_pcm_hw_params_set_format(ctx->theInstance, hardwareParams, ctx->mAlsaParamsCtx->format);
    if (err < 0) {
        RT_LOGE("Unable to configure PCM format %s (%s): %s", formatName, formatDesc, snd_strerror(err));
        goto done;
    }

    RT_LOGD("Set PCM format to %s (%s)",  formatName, formatDesc);

    err = snd_pcm_hw_params_set_channels(ctx->theInstance, hardwareParams, ctx->mAlsaParamsCtx->channels);
    if (err < 0) {
        RT_LOGE("Unable to set channel count to %i: %s", ctx->mAlsaParamsCtx->channels, snd_strerror(err));
        goto done;
    }

    err = snd_pcm_hw_params_set_rate_near(ctx->theInstance, hardwareParams, &requestedRate, 0);

    if (err < 0)
        RT_LOGE("Unable to set sample rate to %u: %s", ctx->mAlsaParamsCtx->sampleRate, snd_strerror(err));
    else if (requestedRate != ctx->mAlsaParamsCtx->sampleRate)
        // Some devices have a fixed sample rate, and can not be changed.
        // This may cause resampling problems; i.e. PCM playback will be too
        // slow or fast.
        RT_LOGD("Requested rate (%u HZ) does not match actual rate (%u HZ)",
                               ctx->mAlsaParamsCtx->sampleRate, requestedRate);
    else
        RT_LOGD("Set sample rate to %u HZ", requestedRate);

    // Make sure we have at least the size we originally wanted
    err = snd_pcm_hw_params_set_buffer_size_near(ctx->theInstance, hardwareParams, &bufferSize);

    if (err < 0) {
        RT_LOGE("Unable to set buffer size to %d:  %s", (int)bufferSize, snd_strerror(err));
        goto done;
    }

    err = snd_pcm_hw_params_set_period_size_near(ctx->theInstance, hardwareParams, &periodSize, NULL);
    if (err < 0) {
        RT_LOGE("Unable to set buffer size to %d:  %s", (int)periodSize, snd_strerror(err));
        goto done;
    }

    // Setup buffers for latency
    err = snd_pcm_hw_params_set_buffer_time_near(ctx->theInstance, hardwareParams, &latency, NULL);

    if (err < 0) {
        /* That didn't work, set the period instead */
        unsigned int periodTime = latency / periods;
        err = snd_pcm_hw_params_set_period_time_near(ctx->theInstance, hardwareParams, &periodTime, NULL);
        if (err < 0) {
            RT_LOGE("Unable to set the period time for latency: %s", snd_strerror(err));
            goto done;
        }

        err = snd_pcm_hw_params_get_period_size(hardwareParams, &periodSize, NULL);
        if (err < 0) {
            RT_LOGE("Unable to get the period size for latency: %s", snd_strerror(err));
            goto done;
        }
        bufferSize = periodSize * periods;
        if (bufferSize < ctx->mAlsaParamsCtx->bufferSize) bufferSize = ctx->mAlsaParamsCtx->bufferSize;
        err = snd_pcm_hw_params_set_buffer_size_near(ctx->theInstance, hardwareParams, &bufferSize);
        if (err < 0) {
            RT_LOGE("Unable to set the buffer size for latency: %s", snd_strerror(err));
            goto done;
        }
    } else {
        // OK, we got buffer time near what we expect. See what that did for bufferSize.
        err = snd_pcm_hw_params_get_buffer_size(hardwareParams, &bufferSize);
        if (err < 0) {
            RT_LOGE("Unable to get the buffer size for latency: %s", snd_strerror(err));
            goto done;
        }
        // Does set_buffer_time_near change the passed value? It should.

        err = snd_pcm_hw_params_get_buffer_time(hardwareParams, &latency, NULL);
        if (err < 0) {
            RT_LOGE("Unable to get the buffer time for latency: %s", snd_strerror(err));
            goto done;
        }

        periodTime = latency/periods;
        err = snd_pcm_hw_params_set_period_time_near(ctx->theInstance, hardwareParams, &periodTime, NULL);
        if ( err < 0 ) {
            RT_LOGE("Unable to set the period time for latency: %s", snd_strerror(err));
            goto done;
        }
    }

    RT_LOGD("audio type flag: %d\n", flag);
    // err = snd_pcm_hw_params_set_flags(ctx->theInstance, hardwareParams, flag);
    if (err < 0)
        RT_LOGE("snd_pcm_hw_params_set_flags fail.");

    RT_LOGD("Buffer size: %d(frames)  period Size: %d(frames) ", (int)bufferSize, (int)periodSize);
    RT_LOGD("periods: %d (buffersize=periodSize*periods)", periods);
    RT_LOGD("Latency: %d   periodTime: %d", (int)latency, periodTime);

    ctx->mAlsaParamsCtx->bufferSize = bufferSize;
    ctx->mAlsaParamsCtx->latency = latency;
    ctx->mAlsaParamsCtx->periodSize = periodSize;
    // Commit the hardware parameters back to the device.
    err = snd_pcm_hw_params(ctx->theInstance, hardwareParams);
    if (err < 0) RT_LOGE("Unable to set hardware parameters: %s", snd_strerror(err));

    return RT_OK;
done:
    snd_pcm_hw_params_free(hardwareParams);
    return RT_ERR_INIT;
}

RT_RET alsa_set_snd_sw_params(ALSASinkContext *ctx) {
    snd_pcm_sw_params_t * softwareParams;
    int err;

    snd_pcm_uframes_t bufferSize = 0;
    snd_pcm_uframes_t periodSize = 0;
    snd_pcm_uframes_t startThreshold, stopThreshold;
    snd_pcm_tstamp_t tstamp_mode = SND_PCM_TSTAMP_ENABLE;

    if (snd_pcm_sw_params_malloc(&softwareParams) < 0) {
        RT_LOGE("Failed to allocate ALSA software parameters!");
        return RT_ERR_INIT;
    }

    // Get the current software parameters
    err = snd_pcm_sw_params_current(ctx->theInstance, softwareParams);
    if (err < 0) {
        RT_LOGE("Unable to get software parameters: %s", snd_strerror(err));
        goto done;
    }

    // Configure ALSA to start the transfer when the buffer is almost full.
    snd_pcm_get_params(ctx->theInstance, &bufferSize, &periodSize);

    startThreshold = bufferSize - 1;
    stopThreshold = bufferSize;

    err = snd_pcm_sw_params_set_start_threshold(ctx->theInstance, softwareParams, startThreshold);
    if (err < 0) {
        RT_LOGE("Unable to set start threshold to %lu frames: %s", startThreshold, snd_strerror(err));
        goto done;
    }

    err = snd_pcm_sw_params_set_stop_threshold(ctx->theInstance, softwareParams, stopThreshold);
    if (err < 0) {
        RT_LOGE("Unable to set stop threshold to %lu frames: %s", stopThreshold, snd_strerror(err));
        goto done;
    }

    // Allow the transfer to start when at least periodSize samples can be
    // processed.
    err = snd_pcm_sw_params_set_avail_min(ctx->theInstance, softwareParams, periodSize);
    if (err < 0) {
        RT_LOGE("Unable to configure available minimum to %lu: %s", periodSize, snd_strerror(err));
        goto done;
    }

    err = snd_pcm_sw_params_set_tstamp_mode(ctx->theInstance, softwareParams, tstamp_mode);
    if (err < 0) {
        RT_LOGE("Unable to set tstamp mode to %s: %s", tstamp_mode, snd_strerror(err));
        goto done;
    }

    // Commit the software parameters back to the device.
    err = snd_pcm_sw_params(ctx->theInstance, softwareParams);
    if (err < 0) RT_LOGE("Unable to configure software parameters: %s", snd_strerror(err));

    return RT_OK;
done:
    snd_pcm_sw_params_free(softwareParams);
    return RT_ERR_INIT;
}

int alsa_snd_write_data(ALSASinkContext *ctx, void *data, int bytes) {
    snd_pcm_sframes_t n;
    int sent = 0;

    if (ctx->theInstance == NULL) {
        RT_LOGE("writeData: theInstance is NULL!");
        return RT_ERR_INIT;
    }

    do {
        n = snd_pcm_writei(ctx->theInstance,
                              reinterpret_cast<char *>(data) + sent,
                              snd_pcm_bytes_to_frames(ctx->theInstance, bytes - sent));

        if (n == -EBADFD) {
            // Somehow the stream is in a bad state. The driver probably
            // has a bug and snd_pcm_recover() doesn't seem to handle this.
            RT_LOGE("writeData snd_pcm_writei return -EBADFD");
            return n;
        } else if (n < 0) {
            RT_LOGE("writeData snd_pcm_writei return = %ld\n", n);
            if (ctx->theInstance) {
                // snd_pcm_recover() will return 0 if successful in recovering from
                // an error, or -errno if the error was unrecoverable.
                if (-EIO == n/* || -EPIPE==n*/) {
                    return n;
                }

                n = snd_pcm_recover(ctx->theInstance, n, 0);

                if (n) {
                    return static_cast<ssize_t>(n);
                }
            }
        } else {
            //   mFrameCount += n;
            sent += static_cast<ssize_t>(snd_pcm_frames_to_bytes(ctx->theInstance, n));
        }
    } while (ctx->theInstance && sent < bytes);
    return sent;
}

ALSASinkContext* alsa_snd_create(const char *name, RtMetaData *metadata) {
    AlsaParamsContext *params_ctx = RT_NULL;
    ALSASinkContext *ctx = rt_malloc(ALSASinkContext);
    ctx->theInstance = RT_NULL;

    ctx->mAlsaParamsCtx = rt_malloc(AlsaParamsContext);
    params_ctx = ctx->mAlsaParamsCtx;

    params_ctx->format = SND_PCM_FORMAT_S16_LE;
    params_ctx->channels = 2;
    params_ctx->sampleRate = DEFAULT_SAMPLE_RATE;
    params_ctx->latency = DEFAULT_OUT_LATENCY;
    params_ctx->periodSize = DEFAULT_OUT_PERIODSIZE;
    params_ctx->bufferSize = DEFAULT_OUT_BUFFERSIZE;
    params_ctx->periods = DEFAULT_OUT_PERIODS;

    snd_pcm_t *pHandle;
    int rc = snd_pcm_open(&pHandle, name, SND_PCM_STREAM_PLAYBACK, 0);
    if (rc < 0) {
        RT_LOGE("Failed to snd_pcm_open device: %s", strerror(rc));
        goto __FAILED;
    }
    ctx->theInstance = pHandle;

    return ctx;

__FAILED:
    if (ctx->mAlsaParamsCtx) {
        rt_free(ctx->mAlsaParamsCtx);
        ctx->mAlsaParamsCtx = RT_NULL;
    }

    if (ctx) {
        rt_free(ctx);
        ctx = RT_NULL;
    }

    return RT_NULL;
}

RT_VOID alsa_snd_destroy(ALSASinkContext *ctx) {
    snd_pcm_t *pHandle = ctx->theInstance;
    int err;
    ctx->theInstance = RT_NULL;
    if (pHandle) {
        err = snd_pcm_close(pHandle);

        if (err < 0) {
            RT_LOGE("Failed snd_pcm_close! err = %s", strerror(err));
        }
    }

    if (ctx->mAlsaParamsCtx) {
        rt_free(ctx->mAlsaParamsCtx);
        ctx->mAlsaParamsCtx = RT_NULL;
    }

    if (ctx) {
        rt_free(ctx);
        ctx = RT_NULL;
    }
}
