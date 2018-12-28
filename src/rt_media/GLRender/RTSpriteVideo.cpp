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

    GLfloat angle;
    GLfloat vpMatrix[4][4];
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

GLubyte* genCheckImage(int width, int height, int checkSize) {
    int x, y;
    GLubyte *pixels = reinterpret_cast<GLubyte *>(rt_malloc_size(GLubyte, width*height*checkSize));

    if (pixels == NULL)
        return NULL;

    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            GLubyte rColor = 0;
            GLubyte bColor = 0;

            if ((x / checkSize) % 2 == 0) {
                rColor = 255 * ((y / checkSize) % 2);
                bColor = 255 * (1 - ((y / checkSize) % 2));
            } else {
                bColor = 255 * ((y / checkSize) % 2);
                rColor = 255 * (1 - ((y / checkSize) % 2));
            }

            pixels[(y * height + x) * 3] = rColor;
            pixels[(y * height + x) * 3 + 1] = 255;
            pixels[(y * height + x) * 3 + 2] = bColor;
        }
    }

    return pixels;
}

void RTSpriteVideo::draw() {
    update();
    UserData *userData = reinterpret_cast<UserData*>(mUserData);
    GLfloat vVertices[] = { -0.5f,  0.5f, 0.0f,  // Position 0
                            0.0f,  0.0f,        // TexCoord 0
                           -0.5f, -0.5f, 0.0f,  // Position 1
                            0.0f,  1.0f,        // TexCoord 1
                            0.5f, -0.5f, 0.0f,  // Position 2
                            1.0f,  1.0f,        // TexCoord 2
                            0.5f,  0.5f, 0.0f,  // Position 3
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
    userData->angle += 0.1f;
    if (userData->angle > 360.0f) {
        userData->angle = 0.0f;
    }
    if (RT_NULL != mCamera) {
        mCamera->setPerspective(60.0f, 800/600, -1.0f, 20.0f);
        mCamera->setModelRotate(0.0f, 0.0f, 1.0f, userData->angle);
        mCamera->getShaderMatric(userData->vpMatrix);
    }
}

GLuint CreateSimpleTexture2D() {
    // Texture object handle
    GLuint textureId;

    // 2x2 Image, 3 bytes per pixel (R, G, B)
    GLubyte *pixels = genCheckImage(128, 128, 64);

    // Use tightly packed data
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Generate a texture object
    glGenTextures(1, &textureId);

    // Bind the texture object
    glBindTexture(GL_TEXTURE_2D, textureId);

    // Load the texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);

    // Set the filtering mode
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    RT_LOGD("glTexImage2D(id=%d)", textureId);

    return textureId;
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
    userData->textureId = CreateSimpleTexture2D();
    userData->angle     = 0.0f;

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}
