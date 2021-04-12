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

#include "gl_exception.h"

namespace game {

namespace /* anonymous */ {

std::string_view getGlErrorString(GLenum flag)
{
	switch (flag)
	{
	case GL_NO_ERROR:
		return "GL_NO_ERROR (" GAME_STR(GL_NO_ERROR) ")\nNo error has been recorded."sv;
	case GL_INVALID_ENUM:
		return "GL_INVALID_ENUM (" GAME_STR(GL_INVALID_ENUM) ")\nAn unacceptable value is specified for an enumerated argument."sv;
	case GL_INVALID_VALUE:
		return "GL_INVALID_VALUE (" GAME_STR(GL_INVALID_VALUE) ")\nA numeric argument is out of range."sv;
	case GL_INVALID_OPERATION:
		return "GL_INVALID_OPERATION (" GAME_STR(GL_INVALID_OPERATION) ")\nThe specified operation is not allowed in the current state."sv;
	case GL_INVALID_FRAMEBUFFER_OPERATION:
		return "GL_INVALID_FRAMEBUFFER_OPERATION (" GAME_STR(GL_INVALID_FRAMEBUFFER_OPERATION) ")\nThe framebuffer object is not complete."sv;
	case GL_OUT_OF_MEMORY:
		return "GL_OUT_OF_MEMORY (" GAME_STR(GL_OUT_OF_MEMORY) ")\nThere is not enough memory left to execute the command."sv;
	case GL_STACK_UNDERFLOW:
		return "GL_STACK_UNDERFLOW (" GAME_STR(GL_STACK_UNDERFLOW) ")\nAn attempt has been made to perform an operation that would cause an internal stack to underflow."sv;
	case GL_STACK_OVERFLOW:
		return "GL_STACK_OVERFLOW (" GAME_STR(GL_STACK_OVERFLOW) ")\nAn attempt has been made to perform an operation that would cause an internal stack to overflow."sv;
	}
	return std::string_view(); // "Unknown OpenGL error flag"sv;
}

std::string_view getMessage(const std::string_view staticMessage, const GLenum flag, const std::string_view file, const int line) noexcept
{
	const std::string_view unknownPre = "("sv;
	const std::string_view unknownPost = ")\nUnknown OpenGL error flag"sv;
	const std::string_view fileTxt = "File: "sv;
	const std::string_view lineTxt = ", line: "sv;
	const ptrdiff_t maxLen = (!staticMessage.empty() ? (staticMessage.size() + 1) : (unknownPre.size() + unknownPost.size() + sizeof(flag) * 2 + 1)) // Message // \n
		+ fileTxt.size() + file.size() + lineTxt.size() + 11 + 1; // File: // a.cpp // , line: // 0 // \0
	char *buf = new (std::nothrow) char[maxLen];
	if (!buf)
		return std::string_view();
	ptrdiff_t i = 0;
	if (!staticMessage.empty())
	{
		memcpy(buf /* &buf[i] */, staticMessage.data(), staticMessage.size());
		i += staticMessage.size();
		if ((!i || buf[i - 1] != '\n') && file.size())
		{
			buf[i] = '\n';
			++i;
		}
	}
	else
	{
		memcpy(&buf[i], unknownPre.data(), unknownPre.size());
		i += unknownPre.size();
		sprintf_s(&buf[i], maxLen - i, "%08x", flag);
		i += 8;
		memcpy(&buf[i], unknownPost.data(), unknownPost.size());
		i += unknownPost.size();
		if (file.size())
		{
			buf[i] = '\n';
			++i;
		}
	}
	if (file.size())
	{
		memcpy(&buf[i], fileTxt.data(), fileTxt.size());
		i += fileTxt.size();
		memcpy(&buf[i], file.data(), file.size());
		i += file.size();
		memcpy(&buf[i], lineTxt.data(), lineTxt.size());
		i += lineTxt.size();
		i += sprintf_s(&buf[i], maxLen - i, "%d", line);
	}
	buf[i] = '\0';
	return std::string_view(buf, i);
}

std::string_view copyString(const std::string_view str) noexcept
{
	if (str.empty())
		return std::string_view();
	char *buf = new (std::nothrow) char[str.size() + 1];
	if (!buf)
		return std::string_view();
	memcpy(buf, str.data(), str.size());
	buf[str.size()] = '\0';
	return std::string_view(buf, str.size());
}

} /* anonymous namespace */

GlException::GlException(const GLenum flag, const StringView file, const int line) noexcept 
	: base("Unknown OpenGL exception"sv, 1),
	m_Flag(flag), m_File(file), m_Line(line),
	m_StaticMessage(getGlErrorString(flag)), 
	m_Message(getMessage(m_StaticMessage.sv(), flag, file.sv(), line))
{
	// no-op
}

GlException::~GlException()
{
	delete[] m_Message.Data;
}

GlException::GlException(const GlException &other) noexcept : base(other),
m_Flag(other.m_Flag), m_File(other.m_File), m_Line(other.m_Line),
m_StaticMessage(other.m_StaticMessage), m_Message(copyString(other.m_Message.sv()))
{

}

GlException &GlException::operator=(GlException const &other) noexcept
{
	if (this != &other) 
	{
		this->~GlException();
		new (this) GlException(other);
	}
	return *this;
}

[[nodiscard]] std::string_view GlException::what() const
{
	return m_Message.Data ? m_Message.sv() : (m_StaticMessage.Data ? m_StaticMessage.sv() : base::what());
}

} /* namespace game */

/* end of file */
