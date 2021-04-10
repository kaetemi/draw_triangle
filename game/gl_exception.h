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

#pragma once
#ifndef GAME_GL_EXCEPTION_H
#define GAME_GL_EXCEPTION_H

#include "platform.h"
#include "exception.h"

namespace game {

class GlException : public Exception
{
public:
	using base = Exception;

	GlException(const GLenum flag, const StringView file, const int line) noexcept;
	inline GlException(const GLenum flag, const std::string_view file, const int line) noexcept : GlException(flag, StringView(file), line) { }
	virtual ~GlException() noexcept;

	GlException(const GlException &other) noexcept;
	GlException &operator=(GlException const &other) noexcept;

	[[nodiscard]] virtual char const *what() const;

	inline std::string_view file() const { return m_File.sv(); };
	inline int line() const { return m_Line; };
	
private:
	GLenum m_Flag;
	StringView m_File; // String view, but guaranteed NUL-terminated
	int m_Line;
	StringView m_StaticMessage; // String view, but guaranteed empty or NUL-terminated
	StringView m_Message; // String view, but guaranteed empty or NUL-terminated
	
};

} /* namespace game */

#define GAME_THROW_GL_ERROR(flag) throw GlException((flag), __FILE__, __LINE__)
#define GAME_THROW_IF_GL_ERROR() while (GLenum flag = glGetError()) { GAME_THROW_GL_ERROR(flag); }

#endif /* #ifndef GAME_GL_EXCEPTION_H */

/* end of file */
