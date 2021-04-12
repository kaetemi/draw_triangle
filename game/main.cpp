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
#include "gl_exception.h"
#include "message_box.h"

#include <shellapi.h>
#include <GL/wglext.h>

#define GAME_GL_MAJOR 4
#define GAME_GL_MINOR 4

namespace game {

HINSTANCE ModuleHandle;
HICON MainIcon;
HWND MainWindow;
HDC MainDeviceContext;
HGLRC MainGlContext;

int ArgC;
char **ArgV;

bool ArbSpirV;
bool ArbSpirVExt;

namespace /* anonymous */ {

std::exception_ptr s_WindowProcException = null;
HGLRC s_DummyGlContext;

const char *s_PosVertGlsl[] = { {
#include "pos.vert.inl"
} };

const uint8_t s_PosVertSpv[] = {
#include "pos.vert.spv.inl"
};

GLuint s_PosVertShader;

const char *s_PosColVertGlsl[] = { {
#include "pos_col.vert.inl"
	} };

const uint8_t s_PosColVertSpv[] = {
#include "pos_col.vert.spv.inl"
};

GLuint s_PosColVertShader;

void loadShader(GLuint shader, const uint8_t *spv, size_t spvLen, const char *const *glsl, size_t glslLen)
{
	if (ArbSpirV && spv)
	{
		glShaderBinary(1, &shader, GL_SHADER_BINARY_FORMAT_SPIR_V, spv, (GLsizei)spvLen);
		glSpecializeShader(shader, "main", 0, null, null);
	}
	else if (glsl)
	{
		GLint len = (GLint)glslLen;
		glShaderSource(shader, 1, glsl, &len);
		glCompileShader(shader);
	}
	else
	{
		throw Exception("No shader loaded");
	}
	GAME_THROW_IF_GL_ERROR();
}

void init()
{
	// Create vertex shader
	s_PosVertShader = glCreateShader(GL_VERTEX_SHADER);
	loadShader(s_PosVertShader, s_PosVertSpv, sizeof(s_PosVertSpv), s_PosVertGlsl, sizeof(s_PosVertGlsl[0]));

	s_PosColVertShader = glCreateShader(GL_VERTEX_SHADER);
	loadShader(s_PosColVertShader, s_PosColVertSpv, sizeof(s_PosColVertSpv), s_PosColVertGlsl, sizeof(s_PosColVertGlsl[0]));
}

void update()
{
	
}

void render()
{
	// Set current context
	GAME_THROW_LAST_ERROR_IF(!wglMakeCurrent(MainDeviceContext, MainGlContext));

	// Clear background
	static const GLfloat bg[4] = { 0.0f, 0.125f, 0.25f, 1.0f };
	glClearBufferfv(GL_COLOR, 0, bg);
	GAME_THROW_IF_GL_ERROR();

	// ...

	// Swap
	GAME_THROW_LAST_ERROR_IF(!SwapBuffers(MainDeviceContext));
}

void release()
{
	GAME_SAFE_C_DELETE(glDeleteShader, s_PosColVertShader);
	GAME_SAFE_C_DELETE(glDeleteShader, s_PosVertShader);
}

void wmCreate(HWND hwnd);
void wmDestroy();

#define RETHROW_WND_PROC_EXCEPTION() if (s_WindowProcException) \
	{ \
		std::exception_ptr ex = s_WindowProcException; \
		s_WindowProcException = null; \
		std::rethrow_exception(ex); \
	}

LRESULT CALLBACK windowProc(_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	try
	{
		// GAME_DEBUG_ASSERT(MainWindow == hwnd); // Only one window, for now
		switch(uMsg)
		{
		case WM_CREATE:
		{
			wmCreate(hwnd);
			break;
		}
		case WM_CLOSE:
		{
			// by default calls DestroyWindow
			// alternatively, implement some warning before closing
			// return 0;
			break;
		}
		case WM_DESTROY:
		{
			wmDestroy();
			break;
		}
		}
	}
	catch (...)
	{
		s_WindowProcException = std::current_exception();
	}
	return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK dummyWindowProc(_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	try
	{
		switch(uMsg)
		{
		case WM_CREATE:
		{
			// Create a dummy context
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
			GAME_THROW_LAST_ERROR_IF(!hglrc);
			wglMakeCurrent(hdc, hglrc);
			s_DummyGlContext = hglrc;
			
			// Initialize GL
			if (gl3wInit())
				throw Exception("OpenGL failed to initialize."sv, 1);
			if (!gl3wIsSupported(GAME_GL_MAJOR, GAME_GL_MINOR))
				throw Exception("OpenGL " GAME_STR(GAME_GL_MAJOR) "." GAME_STR(GAME_GL_MINOR)
					" is not supported by the installed graphics drivers."sv, 1);

			break;
		}
		case WM_CLOSE:
		{
			return 0;
		}
		case WM_DESTROY:
		{
			if (s_DummyGlContext)
			{
				wglDeleteContext(s_DummyGlContext);
				s_DummyGlContext = NULL;
			}
			break;
		}
		}
	}
	catch (...)
	{
		s_WindowProcException = std::current_exception();
	}
	return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

void registerClass(wchar_t *className, WNDPROC wndProc)
{
	WNDCLASSW wndClass;
	wndClass.style = CS_DBLCLKS | CS_OWNDC;
	wndClass.lpfnWndProc = wndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = ModuleHandle;
	wndClass.hIcon = MainIcon;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = className;
	GAME_THROW_LAST_ERROR_IF(!RegisterClassW(&wndClass));
}

void wmCreate(HWND hwnd)
{
	// Create dummy window for context
	registerClass(L"PolyverseGameDummy", dummyWindowProc);
	GAME_FINALLY([&]() -> void { UnregisterClassW(L"PolyverseGameDummy", ModuleHandle); });

	RECT r;
	SetRect(&r, 0, 0, 640, 480);
	AdjustWindowRect(&r, WS_OVERLAPPEDWINDOW, FALSE);
	HWND dummyHwnd = CreateWindowW(
		L"PolyverseGameDummy",
		L"OpenGL Context",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		(r.right - r.left), (r.bottom - r.top),
		0, NULL, ModuleHandle, 0);
	RETHROW_WND_PROC_EXCEPTION();
	GAME_THROW_LAST_ERROR_IF(!dummyHwnd);
	GAME_FINALLY([&]() -> void { DestroyWindow(dummyHwnd); });
	GAME_DEBUG_ASSERT(s_DummyGlContext);

	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR)
	};

	HDC hdc = GetDC(hwnd);
	MainDeviceContext = hdc;

	PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
	if (!wglChoosePixelFormatARB)
		throw Exception("Missing function `wglChoosePixelFormatARB`.");

	const int attribs[] = {
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
		WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
		WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
		WGL_FRAMEBUFFER_SRGB_CAPABLE_EXT, GL_TRUE,
		WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
		WGL_RED_BITS_ARB, 8,
		WGL_GREEN_BITS_ARB, 8,
		WGL_BLUE_BITS_ARB, 8,
		WGL_ALPHA_BITS_ARB, 8,
		WGL_DEPTH_BITS_ARB, 24,
		WGL_STENCIL_BITS_ARB, 8,
		0, 0
	};

	int format = 0;
	UINT numFormats = 0;
	if (!wglChoosePixelFormatARB(hdc, attribs, NULL, 1, &format, &numFormats) || !numFormats)
		throw Exception("Failed to choose pixel format.");
	
	GAME_THROW_LAST_ERROR_IF(!SetPixelFormat(hdc, format, &pfd));

	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
	if (!wglCreateContextAttribsARB)
		throw Exception("Missing function `wglCreateContextAttribsARB`.");

	const int contextAttribs[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, GAME_GL_MAJOR,
		WGL_CONTEXT_MINOR_VERSION_ARB, GAME_GL_MINOR,
		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0, 0
	};

	HGLRC hglrc = wglCreateContextAttribsARB(hdc, NULL, contextAttribs);
	GAME_THROW_LAST_ERROR_IF(!hglrc);
	GAME_THROW_LAST_ERROR_IF(!wglMakeCurrent(hdc, hglrc));
	MainGlContext = hglrc;
	GAME_THROW_IF_GL_ERROR(); // Should not happen, but flush it anyway!

	GAME_DEBUG_OUTPUT(fmt::format(
		"OpenGL {}, GLSL {}\nVendor: {}, Renderer: {}\n",
		glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION),
		glGetString(GL_VENDOR), glGetString(GL_RENDERER)));

	PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC)wglGetProcAddress("wglGetExtensionsStringARB");
	if (!wglGetExtensionsStringARB)
		throw Exception("Missing function `wglGetExtensionsStringARB`.");

	const char *wglExtensions = wglGetExtensionsStringARB(hdc);
	GAME_THROW_IF_GL_ERROR();
	GAME_DEBUG_OUTPUT(fmt::format(
		"WGL extensions: {}\nGL extensions:",
		wglExtensions));
	GLint numExt;
	glGetIntegerv(GL_NUM_EXTENSIONS, &numExt);
	GAME_THROW_IF_GL_ERROR();
	for (GLint i = 0; i < numExt; ++i)
	{
		const char *ext = (const char *)glGetStringi(GL_EXTENSIONS, i);
		GAME_THROW_IF_GL_ERROR();
		GAME_DEBUG_OUTPUT(fmt::format(" {}", ext));
		if (!strcmp(ext, "GL_ARB_gl_spirv"))
			ArbSpirV = true;
		else if (!strcmp(ext, "GL_ARB_spirv_extensions"))
			ArbSpirVExt = true;
	}
	GAME_DEBUG_OUTPUT("\n");

	GAME_DEBUG_OUTPUT(fmt::format("ARB_gl_spirv: {}\n", ArbSpirV)); // GL 4.6
	GAME_DEBUG_OUTPUT(fmt::format("ARB_spirv_extensions: {}\n", ArbSpirVExt)); // GL 4.6

	if (ArbSpirV)
	{
		GAME_DEBUG_ASSERT(glShaderBinary);
		GAME_DEBUG_ASSERT(glSpecializeShader);
	}
}

void wmDestroy()
{
	if (MainDeviceContext)
	{
		ReleaseDC(MainWindow, MainDeviceContext);
		MainDeviceContext = NULL;
	}
	if (MainGlContext)
	{
		// Delete GL context
		wglDeleteContext(MainGlContext);
		MainGlContext = NULL;
	}
	MainWindow = NULL; // Evidently destroyed already
	PostQuitMessage(0); // Quit game
}

void setCmdLine(PWSTR lpCmdLine)
{ 
	int argc;
	LPWSTR *argvw = CommandLineToArgvW(lpCmdLine, &argc);
	GAME_THROW_LAST_ERROR_IF(!argvw);
	if (argc == 0) return;
	GAME_FINALLY([&]() -> void { LocalFree(argvw); });
	int len = sizeof(char *) * argc;
	for (int i = 0; i < argc; ++i)
	{
		int reqLen = WideCharToMultiByte(CP_UTF8, 0,
			argvw[i], -1,
			null, 0,
			null, null);
		GAME_THROW_LAST_ERROR_IF(!reqLen);
		len += reqLen;
	}
	char **argv = (char **)(new char[len]);
	char *argi = (char *)(&argv[argc]);
	ArgC = argc;
	ArgV = argv;
	for (int i = 0; i < argc; ++i)
	{
		argv[i] = argi;
		int resLen = WideCharToMultiByte(CP_UTF8, 0,
			argvw[i], -1,
			argi, len - (int)((ptrdiff_t)argi - (ptrdiff_t)argv),
			null, null);
		GAME_THROW_LAST_ERROR_IF(!resLen);
		argi += resLen;
	}
}

int main()
{
	try
	{
		{
			// Debugging
#ifndef NDEBUG
			_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

			// Cmd line
			game::setCmdLine(GetCommandLineW());
			GAME_FINALLY([&]() -> void { delete[](char *)ArgV; ArgV = null; });

			// Get module handle
			ModuleHandle = (HINSTANCE)GetModuleHandle(NULL);
			GAME_FINALLY([&]() -> void { ModuleHandle = NULL; });

			// Use icon from the executable, if any
			{
				WCHAR szExePath[MAX_PATH];
				GetModuleFileNameW(NULL, szExePath, MAX_PATH);
				MainIcon = ExtractIconW(NULL, szExePath, 0);
			}
			GAME_FINALLY([&]() -> void { MainIcon = NULL; });

			// Register the window class
			registerClass(L"PolyverseGame", windowProc);
			GAME_FINALLY([&]() -> void { UnregisterClassW(L"PolyverseGame", ModuleHandle); });

			// Create a window
			RECT r;
			SetRect(&r, 0, 0, 1280, 720);
			AdjustWindowRect(&r, WS_OVERLAPPEDWINDOW, FALSE);
			MainWindow = CreateWindowW(
				L"PolyverseGame",
				L"Game",
				WS_OVERLAPPEDWINDOW,
				CW_USEDEFAULT, CW_USEDEFAULT,
				(r.right - r.left), (r.bottom - r.top),
				0, NULL, ModuleHandle, 0);
			RETHROW_WND_PROC_EXCEPTION();
			GAME_THROW_LAST_ERROR_IF(!MainWindow);
			GAME_DEBUG_ASSERT(MainGlContext);
			GAME_DEBUG_ASSERT(!s_DummyGlContext);
			GAME_FINALLY([&]() -> void { if (MainWindow) { DestroyWindow(MainWindow); }});

			// Show the main window
			ShowWindow(MainWindow, SW_SHOWNORMAL);

			init();
			GAME_FINALLY([&]() -> void { release(); });

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
						RETHROW_WND_PROC_EXCEPTION();
					}
					else
					{
						update();
						render();
					}
				}
				catch (GlException &ex)
				{
					showMessageBox(ex.what(), "Game Exception"sv, MessageBoxStyle::Error);
					GLenum lastFlag = ex.flag();
					int guardCount = 0;
					while (GLenum flag = glGetError())
					{
						if (flag != lastFlag)
						{
							GlException ex2(flag, ex.file(), ex.line());
							showMessageBox(ex2.what(), "Game Exception"sv, MessageBoxStyle::Error);
							lastFlag = ex2.flag();
						}
						else
						{
							++guardCount;
							if (guardCount > 4096)
							{
								// Critical failure
								// Message box not shown, glGetError not resetting
								GAME_DEBUG_BREAK();
								return EXIT_FAILURE;
							}
						}
					}
				}
				catch (Exception &ex)
				{
					showMessageBox(ex.what(), "Game Exception"sv, MessageBoxStyle::Error);
				}
				catch (...)
				{
					showMessageBox("A system exception occured."sv, "Game Exception"sv, MessageBoxStyle::Error);
				}
			} while (msg.message != WM_QUIT);
		}

		// Done
		GAME_DEBUG_ASSERT(!MainGlContext);
		return EXIT_SUCCESS;
	}
	catch (GlException &ex)
	{
		showMessageBox(ex.what(), "Fatal Game Exception"sv, MessageBoxStyle::Error);
		GLenum lastFlag = ex.flag();
		int guardCount = 0;
		while (GLenum flag = glGetError())
		{
			if (flag != lastFlag)
			{
				GlException ex2(flag, ex.file(), ex.line());
				showMessageBox(ex2.what(), "Fatal Game Exception"sv, MessageBoxStyle::Error);
				lastFlag = ex2.flag();
			}
			else
			{
				++guardCount;
				if (guardCount > 4096)
				{
					// Critical failure
					// Message box not shown, glGetError not resetting
					GAME_DEBUG_BREAK();
					return EXIT_FAILURE;
				}
			}
		}
	}
	catch (Exception &ex)
	{
		showMessageBox(ex.what(), "Fatal Game Exception"sv, MessageBoxStyle::Error);
	}
	catch (...)
	{
		showMessageBox("A system exception occured."sv, "Fatal Game Exception"sv, MessageBoxStyle::Error);
	}
	return EXIT_FAILURE;
}

} /* anonymous namespace */

}

// int main()
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	return game::main();
}

/* end of file */
