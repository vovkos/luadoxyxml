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

class Module;
class Parser;

//..............................................................................

class DoxyHost: public dox::Host
{
protected:
	Module* m_module;
	Parser* m_parser;

public:
	DoxyHost()
	{
		setup(NULL, NULL);
	}

	void
	setup(
		Module* module,
		Parser* parser
		)
	{
		m_module = module;
		m_parser = parser;
	}

	virtual
	dox::Block*
	findItemBlock(handle_t item);

	virtual
	dox::Block*
	getItemBlock(handle_t item);

	virtual
	void
	setItemBlock(
		handle_t item,
		dox::Block* block
		);

	virtual
	sl::String
	createItemRefId(handle_t item);

	virtual
	sl::StringRef
	getItemCompoundElementName(handle_t item);

	virtual
	handle_t
	findItem(
		const sl::StringRef& name,
		size_t overloadIdx
		);

	virtual
	handle_t
	getCurrentNamespace();

	virtual
	bool
	generateGlobalNamespaceDocumentation(
		const sl::StringRef& outputDir,
		sl::String* itemXml,
		sl::String* indexXml
		);

	virtual
	bool
	processCustomCommand(
		const sl::StringRef& command,
		const sl::StringRef& param,
		dox::BlockData* block
		);
};

//..............................................................................

enum CustomCommand
{
	CustomCommand_Undefined,
	CustomCommand_LuaStruct,
	CustomCommand_LuaEnum,
	CustomCommand_LuaBaseType,
};

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

AXL_SL_BEGIN_STRING_HASH_TABLE(CustomCommandNameMap, CustomCommand)
	AXL_SL_HASH_TABLE_ENTRY("luastruct",   CustomCommand_LuaStruct)
	AXL_SL_HASH_TABLE_ENTRY("luaenum",     CustomCommand_LuaEnum)
	AXL_SL_HASH_TABLE_ENTRY("luabasetype", CustomCommand_LuaBaseType)
AXL_SL_END_STRING_HASH_TABLE()

//..............................................................................
