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

#pragma once

//..............................................................................

enum LuaTokenKind
{
	// common tokens

	LuaTokenKind_Eof        = 0,
	LuaTokenKind_Error      = -1,
	LuaTokenKind_Identifier = 256,
	LuaTokenKind_Number,
	LuaTokenKind_String,
	LuaTokenKind_DoxyComment_sl,
	LuaTokenKind_DoxyComment_ml,

	// keywords

	LuaTokenKind_Goto,
	LuaTokenKind_Break,
	LuaTokenKind_Return,
	LuaTokenKind_Do,
	LuaTokenKind_End,
	LuaTokenKind_While,
	LuaTokenKind_Repeat,
	LuaTokenKind_Until,
	LuaTokenKind_If,
	LuaTokenKind_Then,
	LuaTokenKind_ElseIf,
	LuaTokenKind_Else,
	LuaTokenKind_For,
	LuaTokenKind_Function,
	LuaTokenKind_Local,
	LuaTokenKind_In,
	LuaTokenKind_Nil,
	LuaTokenKind_False,
	LuaTokenKind_True,
	LuaTokenKind_Or,
	LuaTokenKind_And,
	LuaTokenKind_Not,

	// special tokens

	LuaTokenKind_Context,
	LuaTokenKind_Ellipsis,
	LuaTokenKind_Le,
	LuaTokenKind_Ge,
	LuaTokenKind_Ne,
	LuaTokenKind_Eq,
	LuaTokenKind_Shl,
	LuaTokenKind_Shr,
	LuaTokenKind_Concat,
	LuaTokenKind_FloorDiv,
};

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

enum LuaTokenChannelMask
{
	LuaTokenChannelMask_Main        = lex::TokenChannelMask_Main, // 0x01,
	LuaTokenChannelMask_DoxyComment = 0x02,
	LuaTokenChannelMask_All         = -1,
};


// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

AXL_LEX_BEGIN_TOKEN_NAME_MAP(LuaTokenName)

	// common tokens

	AXL_LEX_TOKEN_NAME(LuaTokenKind_Eof,            "eof")
	AXL_LEX_TOKEN_NAME(LuaTokenKind_Error,          "error")
	AXL_LEX_TOKEN_NAME(LuaTokenKind_Identifier,     "identifier")
	AXL_LEX_TOKEN_NAME(LuaTokenKind_Number,         "number")
	AXL_LEX_TOKEN_NAME(LuaTokenKind_String,         "string")
	AXL_LEX_TOKEN_NAME(LuaTokenKind_DoxyComment_sl, "doxy-comment-sl")
	AXL_LEX_TOKEN_NAME(LuaTokenKind_DoxyComment_ml, "doxy-comment-ml")

	// keywords

	AXL_LEX_TOKEN_NAME(LuaTokenKind_Goto,     "goto")
	AXL_LEX_TOKEN_NAME(LuaTokenKind_Break,    "break")
	AXL_LEX_TOKEN_NAME(LuaTokenKind_Return,   "return")
	AXL_LEX_TOKEN_NAME(LuaTokenKind_Do,       "do")
	AXL_LEX_TOKEN_NAME(LuaTokenKind_End,      "end")
	AXL_LEX_TOKEN_NAME(LuaTokenKind_While,    "while")
	AXL_LEX_TOKEN_NAME(LuaTokenKind_Repeat,   "repeat")
	AXL_LEX_TOKEN_NAME(LuaTokenKind_Until,    "until")
	AXL_LEX_TOKEN_NAME(LuaTokenKind_If,       "if")
	AXL_LEX_TOKEN_NAME(LuaTokenKind_Then,     "then")
	AXL_LEX_TOKEN_NAME(LuaTokenKind_ElseIf,   "elseif")
	AXL_LEX_TOKEN_NAME(LuaTokenKind_Else,     "else")
	AXL_LEX_TOKEN_NAME(LuaTokenKind_For,      "for")
	AXL_LEX_TOKEN_NAME(LuaTokenKind_Function, "function")
	AXL_LEX_TOKEN_NAME(LuaTokenKind_Local,    "local")
	AXL_LEX_TOKEN_NAME(LuaTokenKind_In,       "in")
	AXL_LEX_TOKEN_NAME(LuaTokenKind_Nil,      "nil")
	AXL_LEX_TOKEN_NAME(LuaTokenKind_False,    "false")
	AXL_LEX_TOKEN_NAME(LuaTokenKind_True,     "true")
	AXL_LEX_TOKEN_NAME(LuaTokenKind_Or,       "or")
	AXL_LEX_TOKEN_NAME(LuaTokenKind_And,      "and")
	AXL_LEX_TOKEN_NAME(LuaTokenKind_Not,      "not")

	// special tokens

	AXL_LEX_TOKEN_NAME(LuaTokenKind_Context,  "::")
	AXL_LEX_TOKEN_NAME(LuaTokenKind_Ellipsis, "...")
	AXL_LEX_TOKEN_NAME(LuaTokenKind_Le,       "<=")
	AXL_LEX_TOKEN_NAME(LuaTokenKind_Ge,       ">=")
	AXL_LEX_TOKEN_NAME(LuaTokenKind_Ne,       "~=")
	AXL_LEX_TOKEN_NAME(LuaTokenKind_Eq,       "==")
	AXL_LEX_TOKEN_NAME(LuaTokenKind_Shl,      "<<")
	AXL_LEX_TOKEN_NAME(LuaTokenKind_Shr,      ">>")
	AXL_LEX_TOKEN_NAME(LuaTokenKind_Concat,   "..")
	AXL_LEX_TOKEN_NAME(LuaTokenKind_FloorDiv, "//")

AXL_LEX_END_TOKEN_NAME_MAP();

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

typedef lex::RagelToken<LuaTokenKind, LuaTokenName, lex::StdTokenData> LuaToken;

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

class LuaLexer: public lex::RagelLexer<LuaLexer, LuaToken>
{
	friend class lex::RagelLexer<LuaLexer, LuaToken>;

protected:
	LuaToken*
	createStringToken(
		int LuaTokenKind,
		size_t left = 0,
		size_t right = 0
		);

	LuaToken*
	createIntegerToken(
		int radix = 10,
		size_t left = 0
		);

	LuaToken*
	createFpToken();

	LuaToken*
	createDoxyCommentToken(int tokenKind);

	// implemented in *.rl

	void
	init();

	void
	exec();
};

//..............................................................................
