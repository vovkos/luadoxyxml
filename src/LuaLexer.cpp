//..............................................................................
//
//  This file is part of the LuaDoxyXML toolkit.
//
//  LuaDoxyXML is distributed under the MIT license.
//  For details see accompanying license.txt file,
//  the public copy of which is also available at:
//  http://tibbo.com/downloads/archive/luadoxyxml/license.txt
//
//..............................................................................

#include "pch.h"
#include "LuaLexer.h"
#include "LuaLexer.rl.cpp"

//..............................................................................

LuaToken*
LuaLexer::createStringToken(
	int tokenKind,
	size_t left,
	size_t right
	)
{
	LuaToken* token = createToken(tokenKind);
	ASSERT(token->m_pos.m_length >= left + right);

	size_t length = token->m_pos.m_length - (left + right);
	token->m_data.m_string = sl::StringRef(ts + left, length);
	return token;
}

LuaToken*
LuaLexer::createIntegerToken(
	int radix,
	size_t left
	)
{
	LuaToken* token = createToken(LuaTokenKind_Number);
	token->m_data.m_double = (double) _strtoui64(ts + left, NULL, radix);
	return token;
}

LuaToken*
LuaLexer::createFpToken()
{
	LuaToken* token = createToken(LuaTokenKind_Number);
	token->m_data.m_double = strtod(ts, NULL);
	return token;
}

LuaToken*
LuaLexer::createDoxyCommentToken()
{
	ASSERT(te - ts >= 3 && ts[0] == '-');

	size_t left = 3;
	size_t right = 0;

	if (ts[2] == '[') // multiline lua comment: --[[!
	{
		ASSERT(ts[1] == '*' && te[-1] == '/' && te[-2] == '*');
		left = 5;
		right = 4;
	}

	LuaToken* token = createStringToken(LuaTokenKind_DoxyComment, left, right);
	token->m_channelMask = LuaTokenChannelMask_DoxyComment;
	return token;
}

//..............................................................................
