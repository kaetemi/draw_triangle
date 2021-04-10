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
	return "Unknown GL error flag"sv; // FIXME: Include flag value! (But Exception will no longer be literal...)
}

} /* anonymous namespace */

GlException::GlException(GLenum flag) : m_Flag(flag), Exception(getGlErrorString(flag), 1)
{
	// no-op
}

GlException::~GlException()
{
	// no-op
}

} /* namespace game */

/* end of file */
