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
	ValueKind_Empty,
	ValueKind_Expression,
	ValueKind_Constant,
	ValueKind_Variable,
	ValueKind_Table,
};

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

struct Value
{
	Token::Pos m_firstTokenPos;
	Token::Pos m_lastTokenPos;
	ValueKind m_valueKind;
	sl::StringRef m_source;
	Table* m_table;

	Value()
	{
		m_valueKind = ValueKind_Empty;
		m_table = NULL;
	}

	bool
	isEmpty() const
	{
		return m_valueKind == ValueKind_Empty;
	}

	void
	clear();

	void
	setFirstToken(
		const Token::Pos& pos,
		ValueKind valueKind = ValueKind_Expression
		);

	void
	appendSource(
		const Token::Pos& pos,
		ValueKind valueKind = ValueKind_Expression
		);
};

//..............................................................................

enum ModuleItemKind
{
	ModuleItemKind_Undefined,
	ModuleItemKind_Variable,
	ModuleItemKind_Field,
	ModuleItemKind_FunctionParam,
	ModuleItemKind_Function,
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

	dox::Block*
	ensureDoxyBlock();

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

	virtual
	void
	generateDoxygenFilterOutput(const sl::StringRef& indent = "") = 0;

	sl::String
	getLocationString()
	{
		return sl::formatString("<location file='%s' line='%d' col='%d'/>\n",
			m_fileName.sz(),
			m_pos.m_line + 1,
			m_pos.m_col + 1
			);
	}

	void
	printDoxygenFilterComment(const sl::StringRef& indent = "");
};

//..............................................................................

struct Variable: ModuleItem
{
	sl::StringRef m_name;
	Value m_index;
	Value m_initializer;

	Variable()
	{
		m_itemKind = ModuleItemKind_Variable;
	}

	bool
	isLuaStruct();

	bool
	isLuaEnum();

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

	virtual
	void
	generateDoxygenFilterOutput(const sl::StringRef& indent);

protected:
	bool
	generateVariableDocumentation(
		const sl::StringRef& outputDir,
		sl::String* itemXml,
		sl::String* indexXml
		);

	bool
	generateLuaBaseTypeDocumentation(sl::String* itemXml);

	bool
	generateLuaStructDocumentation(
		const sl::StringRef& outputDir,
		sl::String* itemXml,
		sl::String* indexXml
		);

	bool
	generateLuaEnumDocumentation(
		const sl::StringRef& outputDir,
		sl::String* itemXml,
		sl::String* indexXml
		);

	void
	generateVariableDoxygenFilterOutput(const sl::StringRef& indent);

	void
	generateLuaStructDoxygenFilterOutput(const sl::StringRef& indent);

	void
	generateLuaEnumDoxygenFilterOutput(const sl::StringRef& indent);
};

//..............................................................................

struct Table: sl::ListLink
{
	sl::Array<Variable*> m_fieldArray;
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

#if (_AXL_CPP_HAS_RVALUE_REF)
	FunctionName(FunctionName&& src)
	{
		operator = (src);
	}
#endif

	FunctionName&
	operator = (const FunctionName& src);

#if (_AXL_CPP_HAS_RVALUE_REF)
	FunctionName&
	operator = (FunctionName&& src)
	{
		sl::takeOver(this, &src);
		return *this;
	}
#endif

	sl::StringRef
	getFullName() const;
};

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

struct FunctionParamArray
{
	sl::Array<Variable*> m_array;
	bool m_isVarArg;

	FunctionParamArray()
	{
		m_isVarArg = false;
	}
};

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

struct Function: ModuleItem
{
	FunctionName m_name;
	FunctionParamArray m_paramArray;

	Function()
	{
		m_itemKind = ModuleItemKind_Function;
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

	virtual
	void
	generateDoxygenFilterOutput(const sl::StringRef& indent);
};

//..............................................................................

class Module
{
	friend class Parser;

protected:
	sl::List<Table> m_tableList;
	sl::List<ModuleItem> m_itemList;
	sl::StringHashTable<ModuleItem*> m_itemMap;
	sl::BoxList<sl::String> m_sourceList;

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
	addSource(const sl::String& source)
	{
		return m_sourceList.insertTail(source) != NULL;
	}

	bool
	generateGlobalNamespaceDocumentation(
		const sl::StringRef& outputDir,
		sl::String* itemXml,
		sl::String* indexXml
		);

	void
	generateDoxygenFilterOutput();
};

//..............................................................................
