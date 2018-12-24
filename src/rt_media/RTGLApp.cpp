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
 */

#include "include/RTGLApp.h"         // NOLINT
#include "include/RTGLView.h"        // NOLINT
#include "include/RTGLVideoScene.h"  // NOLINT
#include "GLRender/RTDirector.h" // NOLINT
#include "rt_type.h"                 // NOLINT
#include "rt_string_utils.h"         // NOLINT
#include <stdio.h>                   // NOLINT

#if HAVE_WINDOW_WIN32
#include <windows.h>
#include <GLES2/gl2.h>

HWND native_window_create(const char* title, INT32 width, INT32 height);

RTGLApp::RTGLApp() {
    mGLView = new RTGLView();
    mWndPtr = 0;
}

RTGLApp::~RTGLApp() {
    mGLView->destroy();
    rt_safe_delete(mGLView);
}

void RTGLApp::createWindow(const char* title, INT32 width, INT32 height) {
    HWND wnd = native_window_create(title, width, height);
    mWndPtr  = wnd;
    mGLView->initialize(wnd, width, height);
    RTDirector::getInstance()->setAnimInterval(1.0/60.0f);
    RTGLVideoScene* scene = new RTGLVideoScene();
    RTDirector::getInstance()->startScene(scene);
}

void RTGLApp::runtime() {
    RTDirector::getInstance()->mainLoop();
    mGLView->swapBuffer();
}


void RTGLApp::eventLoop() {
    MSG msg = { 0 };
    int done = 0;
    while (!done) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != 0) {
            switch (msg.message) {
                case WM_QUIT:
                    done = 1;
                    break;
                default:
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                    break;
            }
        } else {
            // SendMessage( (HWND)mWndPtr, WM_PAINT, 0, 0 );
            runtime();
        }
    }
}

void RTGLApp::toString(char* buffer) {
    rt_str_snprintf(buffer, MAX_NAME_LEN, "%s", "RTObject/RTGLApp");
}

void RTGLApp::summary(char* buffer) {
    this->toString(buffer);
}

LRESULT WINAPI EventProc(HWND hWnd, UINT uMsg,
                              WPARAM wParam, LPARAM lParam) {
    LRESULT err = -1;
    POINT   point;
    RECT    rect;
    EGLWindowCtx *egl_ctx = RT_NULL;
    switch (uMsg) {
        case WM_CREATE:
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case WM_CHAR:
            egl_ctx = reinterpret_cast<EGLWindowCtx*>((LONG_PTR)GetWindowLongPtr(hWnd, GWL_USERDATA));
            if (RT_NULL != egl_ctx) {
                GetCursorPos(&point);
                // Message Handler
            }
            break;
        default:
            err = DefWindowProc(hWnd, uMsg, wParam, lParam);
            break;
    }
    return err;
}

HWND native_window_create(const char* title, INT32 width, INT32 height) {
    WNDCLASS  wndclass  = {0};
    DWORD     wStyle    =  0;
    RECT      winRect   = {0, 0, 0, 0};
    HINSTANCE hInstance =  GetModuleHandle(NULL);

    wndclass.style         = CS_OWNDC;
    wndclass.lpfnWndProc   = (WNDPROC)EventProc;
    wndclass.hInstance     = hInstance;
    wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wndclass.lpszClassName = "Rockit";

    if (!RegisterClass(&wndclass)) {
        RT_LOGE("Fail to RegisterClass(%p)", &wndclass);
        // return FALSE;
    }

    wStyle = WS_VISIBLE | WS_POPUP | WS_BORDER | WS_SYSMENU | WS_CAPTION;

    winRect.left   = 0;
    winRect.top    = 0;
    winRect.right  = width;
    winRect.bottom = height;

    #if TO_DO_FLAG
    egl_ctx->mWnd = CreateWindow(
                         "Rockit",
                         title,
                         wStyle,
                         0,
                         0,
                         winRect.right  - winRect.left,
                         winRect.bottom - winRect.top,
                         NULL,
                         NULL,
                         hInstance,
                         NULL);
    #else
    // HINSTANCE hInstance = GetModuleHandle(NULL);

    HWND preWnd = GetShellWindow();
    HWND hwd    = CreateWindowEx(0,       // window extensive style
                            WC_DIALOG,    // class name
                            "Rockit",     // caption name
                            WS_DLGFRAME | WS_SYSMENU,      // window caption
                            CW_USEDEFAULT, CW_USEDEFAULT,  // or WS_OVERLAPPEDWINDOW
                            800, 600,    // window width and height
                            preWnd,      // parent window handle
                            NULL,        // window menu handle
                            hInstance,   // program instance handle
                            NULL);       // creation parameters
    if (0 != hwd) {
        SetWindowLongPtr(hwd, GWLP_WNDPROC, (LONG_PTR) EventProc);
        ShowWindow(hwd, SW_SHOW);
        RT_LOGE("ShowWindow(%p)", hwd);
    }
    #endif

    // SetWindowLongPtr (hwd, GWL_USERDATA, (LONG) (LONG_PTR) egl_ctx );

    return hwd;
}

// unit-test
void unit_test_win32_gles_app() {
    RTGLApp* app = new RTGLApp();
    app->createWindow("Rockit", 800, 600);
    app->eventLoop();
    rt_safe_delete(app);
}

#else
// unit-test
void unit_test_win32_gles_app() {
    return;
}
#endif

