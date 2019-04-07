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
#include "LuaParser.llk.h"

//..............................................................................

LuaParser::LuaParser(Module* module)
{
	m_module = module;
	m_lastDeclaredItem = NULL;
	m_scopeLevel = 0;
}

bool
LuaParser::variableDeclaration(
	const Token::Pos& pos,
	const sl::StringRef& name
	)
{
	printf("(ln: %d col: %d): variable declaration: %s\n", pos.m_line, pos.m_col, name.sz());

	Variable* variable = AXL_MEM_NEW(Variable);
	variable->m_pos = pos;
	variable->m_name = name;
	m_module->m_variableList.insertTail(variable);
	return true;
}

bool
LuaParser::functionDeclaration(
	const Token::Pos& pos,
	FunctionName* name,
	FunctionArgList* argList
	)
{
	printf("(ln: %d col: %d): function declaration: %s(", pos.m_line, pos.m_col, name->m_first.sz());

	sl::BoxIterator<sl::StringRef> it = argList->m_list.getHead();
	if (it)
		printf("%s", it->sz());

	for (it++; it; it++)
		printf(", %s", it->sz());

	if (argList->m_isVarArg)
	{
		if (argList->m_list.isEmpty())
			printf(", ");

		printf("...");
	}

	printf(")\n");

	Function* function = AXL_MEM_NEW(Function);
	function->m_pos = pos;
	sl::takeOver(&function->m_name, name);
	sl::takeOver(&function->m_argList, argList);
	m_module->m_functionList.insertTail(function);
	return true;
}

//..............................................................................
