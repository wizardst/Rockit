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

#ifndef SRC_RT_MEDIA_MPI_MPIADAPTERUTILS_H_
#define SRC_RT_MEDIA_MPI_MPIADAPTERUTILS_H_

#include "rt_header.h"        // NOLINT
#include "RTVideoUtils.h"     // NOLINT
#include "rk_mpi.h"           // NOLINT
#include "rt_common.h"        // NOLINT

RtCodingType fa_utils_find_codectype_from_mpp(MppCodingType mpp_coding_type);
MppCodingType fa_utils_find_mpp_codectype_from_rt(RtCodingType rt_coding_type);

#endif  // SRC_RT_MEDIA_MPI_MPIADAPTERUTILS_H_

