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

#include "LuaLexer.h"

//..............................................................................

struct ModuleItem: sl::ListLink
{
	LuaToken::Pos m_pos;
	sl::StringRef m_doxyComment;

	virtual
	~ModuleItem()
	{
	}
};

//..............................................................................

struct FunctionName
{
	sl::StringRef m_first;
	sl::BoxList<sl::StringRef> m_list;
	bool m_isMethod;

	FunctionName()
	{
		m_isMethod = false;
	}

	FunctionName(const FunctionName& src)
	{
		operator = (src);
	}

	FunctionName(FunctionName&& src)
	{
		operator = (src);
	}

	FunctionName&
	operator = (const FunctionName& src);

	FunctionName&
	operator = (FunctionName&& src)
	{
		sl::takeOver(this, &src);
		return *this;
	}
};

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

struct FunctionArgList
{
	sl::BoxList<sl::StringRef> m_list;
	bool m_isVarArg;

	FunctionArgList()
	{
		m_isVarArg = false;
	}

	FunctionArgList(const FunctionArgList& src)
	{
		operator = (src);
	}

	FunctionArgList(FunctionArgList&& src)
	{
		operator = (src);
	}

	FunctionArgList&
	operator = (const FunctionArgList& src);

	FunctionArgList&
	operator = (FunctionArgList&& src)
	{
		sl::takeOver(this, &src);
		return *this;
	}
};

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

struct Function: ModuleItem
{
	FunctionName m_name;
	FunctionArgList m_argList;
};

//..............................................................................

struct Variable: ModuleItem
{
	sl::String m_name;
	sl::String m_initializer;
};

//..............................................................................

struct Module
{
	sl::List<Variable> m_variableList;
	sl::List<Function> m_functionList;
};

//..............................................................................
