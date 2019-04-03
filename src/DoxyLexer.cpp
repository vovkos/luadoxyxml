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
#include "DoxyLexer.h"
#include "DoxyLexer.rl.cpp"

//..............................................................................

DoxyToken*
DoxyLexer::createTextToken(DoxyTokenKind tokenKind)
{
	Token* token = createToken(tokenKind);
	token->m_data.m_string = sl::StringRef(ts, te - ts);
	return token;
}

DoxyToken*
DoxyLexer::createNewLineToken()
{
	ASSERT(*ts == '\n');

	Token* token = createToken('\n');
	token->m_data.m_string = sl::StringRef(ts + 1, te - ts - 1);
	return token;
}

//..............................................................................
