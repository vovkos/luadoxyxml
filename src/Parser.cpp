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
#include "Parser.llk.h"

//..............................................................................

Parser::Parser(Module* module):
	m_doxyParser(&module->m_doxyModule)
{
	m_module = module;
	m_lastDeclaredItem = NULL;
	m_scopeLevel = 0;
}

Variable*
Parser::variableDeclaration(
	const Token::Pos& pos,
	const sl::StringRef& name
	)
{
	Variable* variable = AXL_MEM_NEW(Variable);
	variable->m_name = name;
	finalizeDeclaration(pos, name, variable);
	return variable;
}

Function*
Parser::functionDeclaration(
	const Token::Pos& pos,
	FunctionName* name,
	FunctionArgList* argList
	)
{
	Function* function = AXL_MEM_NEW(Function);
	sl::takeOver(&function->m_name, name);
	sl::takeOver(&function->m_argList, argList);
	finalizeDeclaration(pos, function->m_name.getFullName(), function);
	return function;
}

void
Parser::finalizeDeclaration(
	const Token::Pos& pos,
	const sl::StringRef& name,
	ModuleItem* item
	)
{
	item->m_module = m_module;
	item->m_fileName = m_fileName;
	item->m_pos = pos;

	dox::Block* block = m_doxyParser.popBlock();
	if (block)
	{
		item->m_doxyBlock = block;
		block->m_item = item;
	}

	m_module->m_itemList.insertTail(item);
	m_module->m_itemMap[name] = item;

	m_lastDeclaredItem = item;
}

//..............................................................................
