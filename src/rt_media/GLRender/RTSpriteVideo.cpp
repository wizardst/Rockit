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
 * module: render engine for OpenGL-ES2.0
 *    REF: http://www.cnblogs.com/sanjin/p/3580331.html
 *    REF: https://www.jianshu.com/p/e4643b141644
 *    REF: http://blog.db-in.com/cameras-on-opengl-es-2-x/
 *    REF: https://github.com/doggycoder/AndroidOpenGLDemo
 */

#include "rt_header.h"       // NOLINT
#include "rt_string_utils.h" // NOLINT

#include "RTSpriteVideo.h"   // NOLINT
#include "RTShaderUtil.h"    // NOLINT
#include "RTCamera.h"        // NOLINT

#include <GLES2/gl2.h>       // NOLINT

typedef struct _UserData {
    // Handle to a program object
    GLuint programObject;

    // Attribute locations
    GLint  positionLoc;
    GLint  mvpLoc;
    GLint  texCoordLoc;

    // Sampler location
    GLint samplerLoc;

    // Texture handle
    GLuint textureId;

    GLint  width;
    GLint  height;

    GLfloat   angle;
    GLint     stepStrategy;
    GLfloat   vpMatrix[4][4];
    GLubyte*  pixels;
} UserData;

RTSpriteVideo::RTSpriteVideo() {
    RTObject::trace(getName(), this, sizeof(RTSpriteVideo));
    this->mUserData = rt_malloc(UserData);
    this->initUserData();
    this->mCamera = new RTCamera();
}

RTSpriteVideo::~RTSpriteVideo() {
    RTObject::untrace(getName(), this);

    UserData *userData = reinterpret_cast<UserData*>(mUserData);

    // Delete texture object
    glDeleteTextures(1, &userData->textureId);

    // Delete program object
    glDeleteProgram(userData->programObject);

    rt_safe_free(mUserData);
}

void rebindImage2D(UserData* userData) {
    GLubyte* pixels = userData->pixels;

    // Bind the texture object
    if ((RT_NULL != pixels) && (userData->textureId > 0)) {
        RT_LOGE("Image(%p) %dx%d", pixels, userData->width, userData->height);

        glBindTexture(GL_TEXTURE_2D, userData->textureId);

        // Load the texture
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, userData->width, userData->height, 0, \
                                       GL_RGB, GL_UNSIGNED_BYTE, pixels);
        // Set the filtering mode
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
}

GLuint createTexture2D(UserData* userData) {
    // Texture object handle
    GLuint textureId;

    // Use tightly packed data
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Generate a texture object
    glGenTextures(1, &textureId);
    userData->textureId = textureId;

    return textureId;
}

void RTSpriteVideo::draw() {
    update();
    glEnable(GL_DEPTH_TEST);
    UserData *userData = reinterpret_cast<UserData*>(mUserData);
    float pos_w = 0.5f;
    if (userData->height > 0) {
        pos_w = (userData->width / userData->height)/2.0f;
    }
    GLfloat vVertices[] = { -pos_w, 0.5f, 0.0f,  // Position 0
                            0.0f,  0.0f,        // TexCoord 0
                           -pos_w, -0.5f, 0.0f,  // Position 1
                            0.0f,  1.0f,        // TexCoord 1
                            pos_w, -0.5f, 0.0f,  // Position 2
                            1.0f,  1.0f,        // TexCoord 2
                            pos_w,  0.5f, 0.0f,  // Position 3
                            1.0f,  0.0f         // TexCoord 3
                         };
    GLushort indices[] = { 0, 1, 2, 0, 2, 3 };

    // Set the viewport
    glViewport(0, 0, 800, 600);

    // Clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT);

    // Use the program object
    glUseProgram(userData->programObject);

    // Load the vertex position
    glVertexAttribPointer(userData->positionLoc, 3, GL_FLOAT,
                          GL_FALSE, 5 * sizeof(GLfloat), vVertices);
    // Load the texture coordinate
    glVertexAttribPointer(userData->texCoordLoc, 2, GL_FLOAT,
                          GL_FALSE, 5 * sizeof(GLfloat), &vVertices[3]);

    glEnableVertexAttribArray(userData->positionLoc);
    glEnableVertexAttribArray(userData->texCoordLoc);

    // Bind the texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, userData->textureId);

    // Set the sampler texture unit to 0
    glUniform1i(userData->samplerLoc, 0);
    // Load the MVP matrix
    GLfloat* vpMatrix = reinterpret_cast<GLfloat*>(&(userData->vpMatrix[0][0]));
    glUniformMatrix4fv(userData->mvpLoc, 1, GL_FALSE, vpMatrix);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);

    // RT_LOGE("glDrawElements");
}

void RTSpriteVideo::update() {
    UserData *userData = reinterpret_cast<UserData*>(mUserData);
    // userData->angle += 0.1f;
    if (userData->stepStrategy) {
        userData->angle += 0.8f;
        if (userData->angle > 30.0f) {
            userData->stepStrategy = 0;
        }
    } else {
        userData->angle -= 0.8f;
        if (userData->angle < -30.0f) {
            userData->stepStrategy = 1;
        }
    }

    rebindImage2D(userData);
    if (RT_NULL != mCamera) {
        // mCamera->setPerspective(90.0f, 800/600, 0.5f, 55.0f);
        // mCamera->setModelScale(0.01f, 0.01f, 1.0f);
        mCamera->setModelRotate(0.0f, 0.0f, 1.0f, userData->angle);
        mCamera->getShaderMatric(userData->vpMatrix);
    }
}

void RTSpriteVideo::updateFrame(UCHAR* frame, INT32 width, INT32 height) {
    UserData *userData = reinterpret_cast<UserData*>(mUserData);
    userData->pixels   = frame;
    userData->width    = width;
    userData->height   = height;
}

void RTSpriteVideo::initUserData() {
    UserData *userData = reinterpret_cast<UserData*>(mUserData);
    const GLchar vShaderStr[] =
        "uniform mat4 u_mvpMatrix;    \n"
        "attribute vec4 a_position;   \n"
        "attribute vec2 a_texCoord;   \n"
        "varying vec2 v_texCoord;     \n"
        "void main()                  \n"
        "{                            \n"
        "   gl_Position = a_position*u_mvpMatrix; \n"
        "   v_texCoord = a_texCoord;  \n"
        "}                            \n";

    const GLchar fShaderStr[] =
        "precision mediump float;                            \n"
        "varying vec2 v_texCoord;                            \n"
        "uniform sampler2D s_texture;                        \n"
        "void main()                                         \n"
        "{                                                   \n"
        "  gl_FragColor = texture2D( s_texture, v_texCoord );\n"
        "}                                                   \n";

    // Load the shaders and get a linked program object
    userData->programObject = utils_load_program(vShaderStr, fShaderStr);

    // Get the attribute locations
    userData->positionLoc = glGetAttribLocation(userData->programObject, "a_position");
    userData->texCoordLoc = glGetAttribLocation(userData->programObject, "a_texCoord");

    // Get the uniform locations
    userData->samplerLoc = glGetUniformLocation(userData->programObject, "s_texture");
    userData->mvpLoc     = glGetUniformLocation(userData->programObject, "u_mvpMatrix");

    // Load the texture
    createTexture2D(userData);
    userData->angle        = 0.0f;
    userData->stepStrategy = 1;

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}
