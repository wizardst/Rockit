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

#ifndef SRC_RT_MEDIA_GLRENDER_RTSHADERUTIL_H_
#define SRC_RT_MEDIA_GLRENDER_RTSHADERUTIL_H_

#include <GLES2/gl2.h>   // NOLINT

GLuint utils_load_shader(GLenum type, const GLchar *shaderSrc);
GLuint utils_load_program(const GLchar *vertShaderSrc, const GLchar *fragShaderSrc);

#endif  // SRC_RT_MEDIA_GLRENDER_RTSHADERUTIL_H_

