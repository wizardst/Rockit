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
 * author: martin.cheng@rock-chips.com
 *   date: 20190115
 *    ref: ffmpeg/libavcodec/avcodec.h
 */

#ifndef SRC_RT_MEDIA_INCLUDE_RTMEDIAERROR_H_
#define SRC_RT_MEDIA_INCLUDE_RTMEDIAERROR_H_

#include <sys/types.h>
#include <errno.h>

typedef enum {
    // See system/core/include/utils/Errors.h
    // System standard errors from -1 through (possibly) -133
    //
    // Errors with special meanings and side effects.
    // INVALID_OPERATION:  Operation attempted in an illegal state (will try to signal to app).
    // DEAD_OBJECT:        Signal from CodecBase to MediaCodec that MediaServer has died.
    // NAME_NOT_FOUND:     Signal from CodecBase to MediaCodec that the component was not found.
    RTE_OK             = 0,    // Everything's swell.
    RTE_NO_ERROR       = 0,    // No errors.

    RTE_UNKNOWN      = (-2147483647-1),  // INT32_MIN value

    RTE_NO_MEMORY    = -ENOMEM,
    RTE_INVALID_OPT  = -ENOSYS,
    RTE_BAD_VALUE    = -EINVAL,
    RTE_BAD_TYPE     = (RTE_UNKNOWN + 1),
    RTE_NAME_NOT_FOUND      = -ENOENT,
    RTE_PERMISSION_DENIED   = -EPERM,
    RTE_NO_INIT             = -ENODEV,
    RTE_ALREADY_EXISTS      = -EEXIST,
    RTE_DEAD_OBJECT         = -EPIPE,
    RTE_FAILED_TRANSACTION  = (RTE_UNKNOWN + 2),
    RTE_JPARKS_BROKE_IT     = -EPIPE,
#if 0   // !defined(HAVE_MS_C_RUNTIME)
    RTE_BAD_INDEX           = -EOVERFLOW,
    RTE_NOT_ENOUGH_DATA     = -ENODATA,
    RTE_WOULD_BLOCK         = -EWOULDBLOCK,
    RTE_TIMED_OUT           = -ETIMEDOUT,
    RTE_UNKNOWN_TRANSACTION = -EBADMSG,
#else
    RTE_BAD_INDEX           = -E2BIG,
    RTE_NOT_ENOUGH_DATA     = (RTE_UNKNOWN + 3),
    RTE_WOULD_BLOCK         = (RTE_UNKNOWN + 4),
    RTE_TIMED_OUT           = (RTE_UNKNOWN + 5),
    RTE_UNKNOWN_TRANSACTION = (RTE_UNKNOWN + 6),
#endif

    // Media Errors for Rockit(RT)
    RTE_MEDIA_BASE        = -1000,

    RTE_ALREADY_CONNECTED = RTE_MEDIA_BASE,
    RTE_NOT_CONNECTED     = RTE_MEDIA_BASE - 1,
    RTE_UNKNOWN_HOST      = RTE_MEDIA_BASE - 2,
    RTE_CANNOT_CONNECT    = RTE_MEDIA_BASE - 3,
    RTE_IO                = RTE_MEDIA_BASE - 4,
    RTE_CONNECTION_LOST   = RTE_MEDIA_BASE - 5,
    RTE_MALFORMED         = RTE_MEDIA_BASE - 7,
    RTE_OUT_OF_RANGE      = RTE_MEDIA_BASE - 8,
    RTE_BUFFER_TOO_SMALL  = RTE_MEDIA_BASE - 9,
    RTE_UNSUPPORTED       = RTE_MEDIA_BASE - 10,
    RTE_END_OF_STREAM     = RTE_MEDIA_BASE - 11,

    // Media Info
    RTI_FORMAT_CHANGED    = RTE_MEDIA_BASE - 12,
    RTI_DISCONTINUITY     = RTE_MEDIA_BASE - 13,
    RTI_BUFFERS_CHANGED   = RTE_MEDIA_BASE - 14,
    RTI_TIME_OUT          = RTE_MEDIA_BASE - 15,

    // DRM Errors for Rockit(RT). The following constant values should be in sync with
    // drm/drm_framework_common.h
    RTE_DRM_BASE = -2000,

    RTE_DRM_UNKNOWN                        = RTE_DRM_BASE,
    RTE_DRM_NO_LICENSE                     = RTE_DRM_BASE - 1,
    RTE_DRM_LICENSE_EXPIRED                = RTE_DRM_BASE - 2,
    RTE_DRM_SESSION_NOT_OPENED             = RTE_DRM_BASE - 3,
    RTE_DRM_DECRYPT_UNIT_NOT_INITIALIZED   = RTE_DRM_BASE - 4,
    RTE_DRM_DECRYPT                        = RTE_DRM_BASE - 5,
    RTE_DRM_CANNOT_HANDLE                  = RTE_DRM_BASE - 6,
    RTE_DRM_TAMPER_DETECTED                = RTE_DRM_BASE - 7,
    RTE_DRM_NOT_PROVISIONED                = RTE_DRM_BASE - 8,
    RTE_DRM_DEVICE_REVOKED                 = RTE_DRM_BASE - 9,
    RTE_DRM_RESOURCE_BUSY                  = RTE_DRM_BASE - 10,
    RTE_DRM_INSUFFICIENT_OUTPUT_PROTECTION = RTE_DRM_BASE - 11,
    RTE_DRM_LAST_USED_ERRORCODE            = RTE_DRM_BASE - 11,
    RTE_DRM_VENDOR_MAX                     = RTE_DRM_BASE - 500,
    RTE_DRM_VENDOR_MIN                     = RTE_DRM_BASE - 999,
} RTMediaError;

#endif  // SRC_RT_MEDIA_INCLUDE_RTMEDIAERROR_H_
