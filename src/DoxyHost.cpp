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
#include "DoxyHost.h"
#include "Module.h"
#include "Parser.llk.h"

//..............................................................................

dox::Block*
DoxyHost::findItemBlock(handle_t item0)
{
	ModuleItem* item = (ModuleItem*)item0;
	return item->m_doxyBlock;
}

dox::Block*
DoxyHost::getItemBlock(handle_t item0)
{
	ModuleItem* item = (ModuleItem*)item0;
	if (!item->m_doxyBlock)
		item->m_doxyBlock = m_module->m_doxyModule.createBlock(item);

	return item->m_doxyBlock;
}

void
DoxyHost::setItemBlock(
	handle_t item0,
	dox::Block* block
	)
{
	ModuleItem* item = (ModuleItem*)item0;
	item->m_doxyBlock = block;

	if (block)
		block->m_item = item;
}

sl::String
DoxyHost::createItemRefId(handle_t item0)
{
	ModuleItem* item = (ModuleItem*)item0;
	return item->createDoxyRefId();
}

sl::StringRef
DoxyHost::getItemCompoundElementName(handle_t item0)
{
	ModuleItem* item = (ModuleItem*)item0;
	bool isCompoundFile = item->m_itemKind == ModuleItemKind_Variable && ((Variable*)item)->isLuaStruct();
	return isCompoundFile ? "innerclass" : NULL;
};

handle_t
DoxyHost::findItem(
	const sl::StringRef& name,
	size_t overloadIdx
	)
{
	return m_module->findItem(name);
}

handle_t
DoxyHost::getCurrentNamespace()
{
	return (handle_t)(intptr_t)m_parser->m_scopeLevel;
}

bool
DoxyHost::generateGlobalNamespaceDocumentation(
	const sl::StringRef& outputDir,
	sl::String* itemXml,
	sl::String* indexXml
	)
{
	return m_module->generateGlobalNamespaceDocumentation(outputDir, itemXml, indexXml);
}

bool
DoxyHost::processCustomCommand(
	const sl::StringRef& commandName,
	const sl::StringRef& param,
	dox::BlockData* block
	)
{
	bool isParamUsed = false;

	CustomCommand command = CustomCommandNameMap::findValue(commandName, CustomCommand_Undefined);
	switch (command)
	{
	case CustomCommand_LuaStruct:
		block->m_internalDescription += ":luastruct:";
		break;

	case CustomCommand_LuaEnum:
		block->m_internalDescription += ":luaenum:";
		break;

	case CustomCommand_LuaBaseType:
		block->m_internalDescription += ":luabasetype(" + param + ")";
		isParamUsed = true;
		break;
	}

	return isParamUsed;
}

//..............................................................................
