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
#include "Lexer.h"
#include "Lexer.rl.cpp"

//..............................................................................

Token*
Lexer::createStringToken(
	int tokenKind,
	size_t left,
	size_t right
) {
	Token* token = createToken(tokenKind);
	ASSERT(token->m_pos.m_length >= left + right);

	size_t length = token->m_pos.m_length - (left + right);
	token->m_data.m_string = sl::StringRef(ts + left, length);
	return token;
}

Token*
Lexer::createIntegerToken(
	int radix,
	size_t left
) {
	Token* token = createToken(TokenKind_Number);
	token->m_data.m_double = (double) _strtoui64(ts + left, NULL, radix);
	return token;
}

Token*
Lexer::createFpToken() {
	Token* token = createToken(TokenKind_Number);
	token->m_data.m_double = strtod(ts, NULL);
	return token;
}

Token*
Lexer::createDoxyCommentToken(int tokenKind) {
	ASSERT(te - ts >= 3 && ts[0] == '-' && ts[1] == '-');

	size_t left = 0;
	size_t right = 0;

	switch (tokenKind) {
	case TokenKind_DoxyComment_sl: // --!
		ASSERT(ts[2] == '!');
		left = 3;
		break;

	case TokenKind_DoxyComment_ml: // --[[!
		ASSERT(ts[2] == '[' && ts[3] == '[' && ts[4] == '!');
		ASSERT(te[-1] == ']' && te[-2] == ']');
		left = 5;
		right = 2;
		break;

	default:
		ASSERT(false);
	}

	return createStringToken(tokenKind, left, right);
}

//..............................................................................
