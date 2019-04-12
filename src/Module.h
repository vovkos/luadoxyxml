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

#include "Lexer.h"

class Module;
struct Table;

//..............................................................................

enum ValueKind
{
	ValueKind_Undefined,
	ValueKind_Variable,
	ValueKind_Table,
};

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

struct Value
{
	ValueKind m_valueKind;
	sl::StringRef m_source;
	Table* m_table;

	Value()
	{
		m_valueKind = ValueKind_Undefined;
		m_table = NULL;
	}

	void
	clear();
};

//..............................................................................

enum ModuleItemKind
{
	ModuleItemKind_Undefined,
	ModuleItemKind_Variable,
	ModuleItemKind_Function,
	ModuleItemKind_Table,
	ModuleItemKind_Field,
};

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

struct ModuleItem: sl::ListLink
{
	ModuleItemKind m_itemKind;
	Module* m_module;
	sl::String m_fileName;
	Token::Pos m_pos;
	dox::Block* m_doxyBlock;

	ModuleItem();

	virtual
	~ModuleItem()
	{
	}

	virtual
	sl::String
	createDoxyRefId() = 0;

	virtual
	bool
	generateDocumentation(
		const sl::StringRef& outputDir,
		sl::String* itemXml,
		sl::String* indexXml
		) = 0;

	sl::String
	getDoxyLocationString()
	{
		return sl::formatString("<location file='%s' line='%d' col='%d'/>\n",
			m_fileName.sz(),
			m_pos.m_line + 1,
			m_pos.m_col + 1
			);
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

	sl::StringRef
	getFullName() const;
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

	Function()
	{
		m_itemKind = ModuleItemKind_Variable;
	}

	virtual
	sl::String
	createDoxyRefId();

	virtual
	bool
	generateDocumentation(
		const sl::StringRef& outputDir,
		sl::String* itemXml,
		sl::String* indexXml
		);
};

//..............................................................................

struct Variable: ModuleItem
{
	sl::StringRef m_name;
	Value m_initializer;

	Variable()
	{
		m_itemKind = ModuleItemKind_Variable;
	}

	bool
	isTableType();

	virtual
	sl::String
	createDoxyRefId();

	virtual
	bool
	generateDocumentation(
		const sl::StringRef& outputDir,
		sl::String* itemXml,
		sl::String* indexXml
		);

protected:
	bool
	generateVariableDocumentation(
		const sl::StringRef& outputDir,
		sl::String* itemXml,
		sl::String* indexXml
		);

	bool
	generateTableTypeDocumentation(
		const sl::StringRef& outputDir,
		sl::String* itemXml,
		sl::String* indexXml
		);
};

//..............................................................................

struct Field: Variable
{
	size_t m_index;

	Field()
	{
		m_itemKind = ModuleItemKind_Field;
		m_index = 0;
	}

	virtual
	sl::String
	createDoxyRefId();
};

//..............................................................................

struct Table: sl::ListLink
{
	sl::Array<Field*> m_fieldArray;

	bool
	generateDocumentation(
		const sl::StringRef& outputDir,
		sl::String* itemXml,
		sl::String* indexXml
		);
};

//..............................................................................

class Module
{
	friend class Parser;

protected:
	sl::List<Table> m_tableList;
	sl::List<ModuleItem> m_itemList;
	sl::StringHashTable<ModuleItem*> m_itemMap;

public:
	dox::Module m_doxyModule;

public:
	Module(dox::Host* doxyHost):
		m_doxyModule(doxyHost)
	{
	}

	dox::Host* getDoxyHost()
	{
		return m_doxyModule.getHost();
	}

	ModuleItem*
	findItem(const sl::StringRef& name)
	{
		return m_itemMap.findValue(name, NULL);
	}

	bool
	generateGlobalNamespaceDocumentation(
		const sl::StringRef& outputDir,
		sl::String* itemXml,
		sl::String* indexXml
		);
};

//..............................................................................
