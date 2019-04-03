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

enum DoxyTokenKind
{
	DoxyTokenKind_Eof = 0,
	DoxyTokenKind_Error = -1,
	DoxyTokenKind_Text = 256,
	DoxyTokenKind_OtherCommand,
	DoxyTokenKind_OpeningBrace,
	DoxyTokenKind_ClosingBrace,

	DoxyTokenKind_Import,
	DoxyTokenKind_Enum,
	DoxyTokenKind_EnumValue,
	DoxyTokenKind_Struct,
	DoxyTokenKind_Union,
	DoxyTokenKind_Class,
	DoxyTokenKind_Alias,
	DoxyTokenKind_Variable,
	DoxyTokenKind_Field,
	DoxyTokenKind_Function,
	DoxyTokenKind_Overload,
	DoxyTokenKind_Property,
	DoxyTokenKind_Event,
	DoxyTokenKind_Typedef,
	DoxyTokenKind_Namespace,
	DoxyTokenKind_Group,
	DoxyTokenKind_InGroup,
	DoxyTokenKind_SubGroup,
	DoxyTokenKind_Title,
	DoxyTokenKind_Brief,
	DoxyTokenKind_SeeAlso,
	DoxyTokenKind_Footnote,
};

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

AXL_LEX_BEGIN_TOKEN_NAME_MAP(DoxyTokenName)

	AXL_LEX_TOKEN_NAME(DoxyTokenKind_Eof,           "eof")
	AXL_LEX_TOKEN_NAME(DoxyTokenKind_Error,         "error")
	AXL_LEX_TOKEN_NAME(DoxyTokenKind_Text,          "text")
	AXL_LEX_TOKEN_NAME(DoxyTokenKind_OtherCommand,  "other-command")
	AXL_LEX_TOKEN_NAME(DoxyTokenKind_OpeningBrace,  "@{")
	AXL_LEX_TOKEN_NAME(DoxyTokenKind_ClosingBrace,  "@}")

	AXL_LEX_TOKEN_NAME(DoxyTokenKind_Import,        "\\import")
	AXL_LEX_TOKEN_NAME(DoxyTokenKind_Enum,          "\\enum")
	AXL_LEX_TOKEN_NAME(DoxyTokenKind_EnumValue,     "\\enumvalue")
	AXL_LEX_TOKEN_NAME(DoxyTokenKind_Struct,        "\\struct")
	AXL_LEX_TOKEN_NAME(DoxyTokenKind_Union,         "\\union")
	AXL_LEX_TOKEN_NAME(DoxyTokenKind_Class,         "\\class")
	AXL_LEX_TOKEN_NAME(DoxyTokenKind_Alias,         "\\alias")
	AXL_LEX_TOKEN_NAME(DoxyTokenKind_Variable,      "\\variable")
	AXL_LEX_TOKEN_NAME(DoxyTokenKind_Field,         "\\field")
	AXL_LEX_TOKEN_NAME(DoxyTokenKind_Function,      "\\function")
	AXL_LEX_TOKEN_NAME(DoxyTokenKind_Overload,      "\\overload")
	AXL_LEX_TOKEN_NAME(DoxyTokenKind_Property,      "\\property")
	AXL_LEX_TOKEN_NAME(DoxyTokenKind_Event,         "\\event")
	AXL_LEX_TOKEN_NAME(DoxyTokenKind_Typedef,       "\\typedef")
	AXL_LEX_TOKEN_NAME(DoxyTokenKind_Namespace,     "\\namespace")
	AXL_LEX_TOKEN_NAME(DoxyTokenKind_Group,         "\\group")
	AXL_LEX_TOKEN_NAME(DoxyTokenKind_InGroup,       "\\ingroup")
	AXL_LEX_TOKEN_NAME(DoxyTokenKind_SubGroup,      "\\subgroup")
	AXL_LEX_TOKEN_NAME(DoxyTokenKind_Title,         "\\title")
	AXL_LEX_TOKEN_NAME(DoxyTokenKind_Brief,         "\\brief")
	AXL_LEX_TOKEN_NAME(DoxyTokenKind_SeeAlso,       "\\see")
	AXL_LEX_TOKEN_NAME(DoxyTokenKind_Footnote,      "\\footnote")

AXL_LEX_END_TOKEN_NAME_MAP();

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

typedef lex::RagelToken<DoxyTokenKind, DoxyTokenName, lex::StdTokenData> DoxyToken;

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

class DoxyLexer: public lex::RagelLexer<DoxyLexer, DoxyToken>
{
	friend class lex::RagelLexer<DoxyLexer, DoxyToken>;

protected:
	DoxyToken*
	createTextToken(DoxyTokenKind tokenKind);

	DoxyToken*
	createNewLineToken();

	// implemented in *.rl

	void
	init();

	void
	exec();
};

//..............................................................................
