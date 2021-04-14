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

#include "message_box.h"

namespace game {

namespace /* anonymous */ {

UINT s_APC = GetACP();

} /* anonymous namespace */

bool showMessageBox(std::string_view message, std::string_view title, MessageBoxStyle style)
{
	HWND window = GetActiveWindow();
	bool fullscreen = window && (GetWindowLongW(window, GWL_STYLE) & WS_POPUP);
	bool minimized = fullscreen && ShowWindow(window, SW_MINIMIZE);
	GAME_FINALLY([&]() -> void {
		if (minimized)
			ShowWindow(window, SW_RESTORE);
	});
	if (s_APC == CP_UTF8 && !(&message[0])[message.size()] && !(&title[0])[title.size()])
	{
		return MessageBoxA(window, &message[0], &title[0], (UINT)style | ((fullscreen || !window) ? MB_SYSTEMMODAL : MB_TASKMODAL)) != 0;
	}
	else
	{
		wchar_t *wmessage = (wchar_t *)_malloca((message.size() + 1) * 2);
		GAME_FINALLY([&]() -> void { _freea(wmessage); });
		wchar_t *wtitle = (wchar_t *)_malloca((title.size() + 1) * 2);
		GAME_FINALLY([&]() -> void { _freea(wtitle); });

		if (wmessage)
		{
			int wideLen = MultiByteToWideChar(CP_UTF8, 0,
				&message[0], (int)message.size(),
				wmessage, (int)message.size() * 2);
			wmessage[wideLen] = 0;
		}

		if (wtitle)
		{
			int wideLen = MultiByteToWideChar(CP_UTF8, 0,
				&title[0], (int)title.size(),
				wtitle, (int)title.size() * 2);
			wtitle[wideLen] = 0;
		}

		return MessageBoxW(window, 
			wmessage ? wmessage : L"Out of memory",
			wtitle ? wtitle : L"Game",
			(UINT)style | ((fullscreen || !window) ? MB_SYSTEMMODAL : MB_TASKMODAL)) != 0;
	}
}

} /* namespace game */

/* end of file */
