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
 * Author: rimon.xu@rock-chips.com
 *   Date: 2019/01/03
 */

#ifndef SRC_RT_MEDIA_MPI_INCLUDE_MPIADAPTERUTILS_H_
#define SRC_RT_MEDIA_MPI_INCLUDE_MPIADAPTERUTILS_H_

#include "rt_header.h"        // NOLINT
#include "rk_mpi.h"           // NOLINT
#include "rt_common.h"        // NOLINT
#include "RTMediaDef.h"       // NOLINT

UINT32 ma_mpp_to_rt_codec_id(const UINT32 type);
UINT32 ma_rt_to_mpp_codec_id(const UINT32 type);

UINT32 ma_mpp_to_rt_color_format(const UINT32 type);
UINT32 ma_rt_to_mpp_color_format(const UINT32 type);

UINT32 ma_mpp_to_rt_rc_mode(const UINT32 type);
UINT32 ma_rt_to_mpp_rc_mode(const UINT32 type);

#endif  // SRC_RT_MEDIA_MPI_INCLUDE_MPIADAPTERUTILS_H_

