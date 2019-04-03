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

struct ModuleItem: sl::ListLink
{
	virtual
	~ModuleItem()
	{
	}
};

//..............................................................................

struct FunctionName
{
	sl::String m_first;
	sl::BoxList<sl::String> m_list;
	bool m_isMethod;

	FunctionName()
	{
		m_isMethod = false;
	}
};

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

struct Function: ModuleItem
{
	FunctionName m_name;
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
