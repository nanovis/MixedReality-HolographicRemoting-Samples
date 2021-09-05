//*********************************************************
//    Copyright (c) Microsoft. All rights reserved.
//
//    Apache 2.0 License
//
//    You may obtain a copy of the License at
//    http://www.apache.org/licenses/LICENSE-2.0
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
//    implied. See the License for the specific language governing
//    permissions and limitations under the License.
//
//*********************************************************
#define GLEW_STATIC

#include "pch.h"
#include "OpenXrProgram.h"
#include <iostream>
#include "glew.h"
#include "wglew.h"
#include <SampleShared/CommandLineUtility.h>



#define SCREEN_WIDTH 787
#define SCREEN_HEIGHT 550


#define KEYDOWN(vk) (GetAsyncKeyState(vk) & 0x8000)

#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
constexpr const char* ProgramName = "DirectX";
#else
constexpr const char* ProgramName = "SampleRemoteOpenXr_uwp";
#endif
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    //initate Opengl window
        HWND hWndGL;
        WNDCLASSEX wc;
        ZeroMemory(&wc, sizeof(WNDCLASSEX));
        wc.cbSize = sizeof(WNDCLASSEX);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = hInstance;
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.lpszClassName = L"WindowClass";

            RegisterClassEx(&wc);

           hWndGL = CreateWindowEx(NULL,
                                    L"WindowClass",
                                    L"OpenGL  ",
                                    WS_OVERLAPPEDWINDOW,
                                    SCREEN_WIDTH + 20,
                                    0,
                                    SCREEN_WIDTH,
                                    SCREEN_HEIGHT,
                                    NULL,
                                    NULL,
                                    hInstance,
                                    NULL);

            ShowWindow(hWndGL, nCmdShow);
            

                MSG msg;


    try {
        sample::AppOptions options;
        sample::ParseCommandLine(options);



        auto graphics = sample::CreateCubeGraphics();

        //1- open Opengl Window
        graphics->getOpenGLProgramPtr()->initWindow(hWndGL);
        auto program = sample::CreateOpenXrProgram(ProgramName, std::move(graphics), options);




      
        program->Run();

    } catch (const std::exception& ex) {
        DEBUG_PRINT("Unhandled Exception: %s\n", ex.what());
        return 1;
    } catch (...) {
        DEBUG_PRINT("Unhandled Exception\n");
        return 1;
    }
    return 0;
}



