/*

Copyright (C) 2021  Jan BOON (Kaetemi) <jan.boon@kaetemi.be>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

/*

This is it.

*/

#include "platform.h"
#include "win32_exception.h"

#include <shellapi.h>
#include <boxer/boxer.h>

namespace game {

HINSTANCE g_ModuleHandle;
HWND g_MainWindow;
HGLRC g_GLContext;

namespace /* anonymous */ {

std::exception_ptr s_WindowProcException = null;

void update()
{
	
}

void render()
{
	
}

LRESULT CALLBACK windowProc(_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	try
	{
		// GAME_DEBUG_ASSERT(g_MainWindow == hwnd); // Only one window, for now
		switch(uMsg)
		{
		case WM_CREATE:
		{
			PIXELFORMATDESCRIPTOR pfd = {
				sizeof(PIXELFORMATDESCRIPTOR),
				1,
				PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
				PFD_TYPE_RGBA, 32,
				0, 0, 0, 0, 0, 0,
				0,
				0,
				0,
				0, 0, 0, 0,
				24, 8, 0,
				PFD_MAIN_PLANE,
				0,
				0, 0, 0
			};

			HDC hdc = GetDC(hwnd);
			GAME_FINALLY([&]() -> void { ReleaseDC(hwnd, hdc); });

			int format;
			format = ChoosePixelFormat(hdc, &pfd); 
			SetPixelFormat(hdc,format, &pfd);

			HGLRC hglrc = wglCreateContext(hdc);
			GAME_IF_THROW_LAST_ERROR(!hglrc);
			wglMakeCurrent(hdc, hglrc);
			g_GLContext = hglrc;

			// Initialize GL
			if (gl3wInit())
				throw Exception("OpenGL failed to initialize."sv, 1);
			if (!gl3wIsSupported(4, 6))
				throw Exception("OpenGL 4.6 is not supported."sv, 1);
			printf("OpenGL %s, GLSL %s\n", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));

			break;
		}
		case WM_CLOSE:
		{
			return 0;
		}
		case WM_DESTROY:
		{
			if (g_GLContext)
				wglDeleteContext(g_GLContext);
			PostQuitMessage(0);
			break;
		}
		}
	}
	catch (...)
	{
		s_WindowProcException = std::current_exception();
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int main()
{
	try
	{
		// Get module handle
		g_ModuleHandle = (HINSTANCE)GetModuleHandle(NULL);
		GAME_FINALLY([&]() -> void { g_ModuleHandle = NULL; });
		
		// Use icon from the executable, if any
		WCHAR szExePath[MAX_PATH];
		GetModuleFileNameW(NULL, szExePath, MAX_PATH);
		HICON hIcon = ExtractIconW(g_ModuleHandle, szExePath, 0);
		
		// Register the window class
		WNDCLASSW wndClass;
		wndClass.style = CS_DBLCLKS | CS_OWNDC;
		wndClass.lpfnWndProc = windowProc;
		wndClass.cbClsExtra = 0;
		wndClass.cbWndExtra = 0;
		wndClass.hInstance = g_ModuleHandle;
		wndClass.hIcon = hIcon;
		wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wndClass.lpszMenuName = NULL;
		wndClass.lpszClassName = L"PolyverseGame";
		GAME_IF_THROW_LAST_ERROR(!RegisterClassW(&wndClass));
		GAME_FINALLY([&]() -> void { UnregisterClassW(L"PolyverseGame", g_ModuleHandle); });
		
		// Create a window
		RECT r;
		SetRect(&r, 0, 0, 1280, 720);
		AdjustWindowRect(&r, WS_OVERLAPPEDWINDOW, FALSE);
		g_MainWindow = CreateWindowW(
			L"PolyverseGame",
			L"Game",
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT,
			(r.right - r.left), (r.bottom - r.top),
			0, NULL, g_ModuleHandle, 0);
		GAME_IF_THROW_LAST_ERROR(!g_MainWindow);
		GAME_FINALLY([&]() -> void { DestroyWindow(g_MainWindow); });
		
		// Message loop
		MSG msg = { 0 };
		do
		{
			try
			{
				if (PeekMessageW(&msg, NULL, 0U, 0U, PM_REMOVE))
				{
					// Translate and dispatch the message
					TranslateMessage(&msg);
					DispatchMessage(&msg);
					if (s_WindowProcException)
					{
						std::exception_ptr ex = s_WindowProcException;
						s_WindowProcException = null;
						std::rethrow_exception(ex);
					}
				}
				else
				{
					update();
					render();
				}
			}
			catch (Exception &ex)
			{
				boxer::show(ex.what(), "Game Exception");
			}
			catch (...)
			{
				boxer::show("A system exception occured.", "Game Exception");
			}
		} while (msg.message != WM_QUIT);
		
		// Done
		return EXIT_SUCCESS;
	}
	catch (Exception &ex)
	{
		boxer::show(ex.what(), "Game Exception");
	}
	catch (...)
	{
		boxer::show("A system exception occured.", "Game Exception");
	}
	return EXIT_FAILURE;
}

} /* anonymous namespace */

}

int main()
// int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	return game::main();
}

/* end of file */
