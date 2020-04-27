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
Parser::declareVariable(
	const Token::Pos& pos,
	const sl::StringRef& name,
	ModuleItemKind itemKind
	)
{
	bool isGlobalName = itemKind == ModuleItemKind_Variable;
	Variable* variable = m_module->createVariable(name, itemKind);
	finalizeDeclaration(pos, variable, isGlobalName);
	return variable;
}

size_t
Parser::declareLocalVariables(
	const Token::Pos& pos,
	const sl::BoxList<sl::StringRef>& nameList,
	const sl::BoxList<Value>& initializerList
	)
{
	size_t count = 0;
	sl::ConstBoxIterator<sl::StringRef> it1 = nameList.getHead();
	sl::ConstBoxIterator<Value> it2 = initializerList.getHead();
	for (; it1 && it2; it1++, it2++, count++)
	{
		Variable* variable = declareVariable(pos, *it1);
		variable->m_isLocal = true;
		variable->m_initializer = *it2;
	}

	return count;
}

size_t
Parser::initializeVariables(
	const sl::ArrayRef<Variable*>& variableArray,
	const sl::BoxList<Value>& initializerList
	)
{
	size_t i = 0;
	size_t variableCount = variableArray.getCount();
	sl::ConstBoxIterator<Value> it = initializerList.getHead();
	for (; i < variableCount && it; i++)
		variableArray[i]->m_initializer = *it;

	return i;
}

Variable*
Parser::declareIndexedField(
	const Token::Pos& pos,
	const Value& index
	)
{
	Variable* field = declareVariable(pos, NULL, ModuleItemKind_Field);
	field->m_index = index;
	return field;
}

Variable*
Parser::declareUnnamedField(
	const Token::Pos& pos,
	const Value& initializer
	)
{
	Variable* field = declareVariable(pos, NULL, ModuleItemKind_Field);
	field->setInitializer(initializer);
	return field;
}

Function*
Parser::declareFunction(
	const Token::Pos& pos,
	FunctionName* name,
	bool isLocal
	)
{
	Function* function = m_module->createFunction(name->m_name);
	function->m_isLocal = isLocal;

	if (name->m_list.isEmpty())
	{
		finalizeDeclaration(pos, function, true);
		return function;
	}

	sl::BoxIterator<sl::StringRef> it = name->m_list.getHead();
	Table* table = m_module->findTable(*it);
	for (it++; table && it; it++)
		table = m_module->findTableField(table, *it);

	if (!table)
		return NULL; // parent module/class must have been declared first

	function->m_isMethod = name->m_isMethod;
	Variable* field = m_module->createVariable(name->m_name, ModuleItemKind_Field);
	field->setInitializer(function);
	table->addField(field);
	finalizeDeclaration(pos, function);
	return function;
}

Function*
Parser::declareFunction(const Token::Pos& pos)
{
	Function* function = m_module->createFunction();
	finalizeDeclaration(pos, function);
	return function;
}

void
Parser::finalizeDeclaration(
	const Token::Pos& pos,
	ModuleItem* item,
	bool isGlobalName
	)
{
	item->m_fileName = m_fileName;
	item->m_pos = pos;

	dox::Block* block = m_doxyParser.popBlock();
	if (block)
	{
		item->m_doxyBlock = block;
		block->m_item = item;
	}

	if (isGlobalName && !item->m_name.isEmpty())
	{
		sl::StringHashTableIterator<ModuleItem*> it = m_module->m_itemMap.visit(item->m_name);
		if (!it->m_value) // keep the original declaration
			it->m_value = item;
	}

	m_lastDeclaredItem = item;
}

//..............................................................................
