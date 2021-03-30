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

#include <boxer/boxer.h>

HINSTANCE g_ModuleHandle;
HWND g_MainWindow;

void update()
{
	
}

void render()
{
	
}

LRESULT CALLBACK windowProc(_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_CLOSE:
			return 0;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
	}
	
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int main()
{
	try
	{
		// Get module handle
		g_ModuleHandle = (HINSTANCE)GetModuleHandle(NULL);
		GAME_FINALLY([&]() -> void { g_ModuleHandle = NULL; });
		
		// Use icon from the executable, if any
		HICON hIcon = NULL;
		WCHAR szExePath[MAX_PATH];
		GetModuleFileNameW(NULL, szExePath, MAX_PATH);
		HICON hIcon = ExtractIconW(g_ModuleHandle, szExePath, 0);
		
		// Register the window class
		WNDCLASS wndClass;
		wndClass.style = CS_DBLCLKS;
		wndClass.lpfnWndProc = windowProc;
		wndClass.cbClsExtra = 0;
		wndClass.cbWndExtra = 0;
		wndClass.hInstance = g_ModuleHandle;
		wndClass.hIcon = hIcon;
		wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wndClass.lpszMenuName = NULL;
		wndClass.lpszClassName = "PolyverseGame";
		GAME_IF_THROW_LAST_ERROR(!RegisterClass(&wndClass));
		GAME_FINALLY([&]() -> void { UnregisterClass("PolyverseGame", g_ModuleHandle); });
		
		// Create a window
		RECT r;
		SetRect(&r, 0, 0, 1280, 720);
		AdjustWindowRect(&r, WS_OVERLAPPEDWINDOW, FALSE);
		g_MainWindow = CreateWindow(
			"PolyverseGame",
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
			if (PeekMessageW(&msg, NULL, 0U, 0U, PM_REMOVE))
			{
				// Translate and dispatch the message
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				update();
				render();
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

/* end of file */
