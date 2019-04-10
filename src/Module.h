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

//..............................................................................

struct ModuleItem: sl::ListLink
{
	Module* m_module;
	sl::String m_fileName;
	Token::Pos m_pos;
	dox::Block* m_doxyBlock;

	ModuleItem()
	{
		m_module = NULL;
		m_doxyBlock = NULL;
	}

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
	sl::String m_name;
	sl::String m_initializer;

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

class Module
{
	friend class Parser;

protected:
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
