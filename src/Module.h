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
struct Variable;
struct Function;

//..............................................................................

enum ValueKind {
	ValueKind_Empty,
	ValueKind_Expression,
	ValueKind_Constant,
	ValueKind_Variable,
	ValueKind_Function,
	ValueKind_Table,
};

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

struct Value {
	Token::Pos m_firstTokenPos;
	Token::Pos m_lastTokenPos;
	ValueKind m_valueKind;
	sl::StringRef m_source;
	Table* m_table;
	Function* m_function;

	Value();
	Value(Table* table);
	Value(Function* function);

	bool
	isEmpty() const {
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
		ValueKind valueKind
	) {
		appendSource(pos);
		m_valueKind = valueKind;
	}

	void
	appendSource(const Token::Pos& pos);
};

//..............................................................................

struct Table: sl::ListLink {
	Variable* m_lvalue;
	sl::Array<Variable*> m_fieldArray;
	sl::StringHashTable<Variable*> m_fieldMap;

	Table() {
		m_lvalue = NULL;
	}

	Variable*
	findField(const sl::StringRef& name) {
		return m_fieldMap.findValue(name, NULL);
	}

	void
	addField(Variable* field);
};

//..............................................................................

enum ModuleItemKind {
	ModuleItemKind_Undefined,
	ModuleItemKind_Variable,
	ModuleItemKind_Field,
	ModuleItemKind_FunctionParam,
	ModuleItemKind_Function,
};

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

struct ModuleItem: sl::ListLink {
	ModuleItemKind m_itemKind;
	Module* m_module;
	Table* m_table;
	bool m_isLocal;
	sl::StringRef m_name;
	sl::String m_fileName;
	Token::Pos m_pos;
	dox::Block* m_doxyBlock;

	ModuleItem();

	virtual
	~ModuleItem() {}

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
	bool
	generateCompoundMemberDocumentation(
		const sl::StringRef& outputDir,
		sl::String* memberXml,
		sl::String* compoundXml,
		sl::String* sectionXml,
		sl::String* indexXml
	);

	virtual
	void
	generateDoxygenFilterOutput(const sl::StringRef& indent = "") = 0;

	sl::String
	getLocationString() {
		return sl::formatString("<location file='%s' line='%d' col='%d'/>\n",
			m_fileName.sz(),
			m_pos.m_line + 1,
			m_pos.m_col + 1
		);
	}

	void
	printDoxygenFilterComment(const sl::StringRef& indent = "");

protected:
	dox::Block*
	prepareDoxyBlock();
};

//..............................................................................

enum VariableKind {
	VariableKind_Undefined,
	VariableKind_Normal,
	VariableKind_Enum,
	VariableKind_Class,
	VariableKind_Struct,
	VariableKind_Module,
};

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

struct Variable: ModuleItem {
	VariableKind m_variableKind;
	Value m_index;
	Value m_initializer;

	Variable();

	VariableKind
	getVariableKind() {
		return m_variableKind ? m_variableKind : ensureVariableKind();
	}

	bool
	isLuaClass() {
		return getVariableKind() >= VariableKind_Class;
	}

	void
	setInitializer(const Value& value);

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
	bool
	generateCompoundMemberDocumentation(
		const sl::StringRef& outputDir,
		sl::String* memberXml,
		sl::String* compoundXml,
		sl::String* sectionXml,
		sl::String* indexXml
	);

	virtual
	void
	generateDoxygenFilterOutput(const sl::StringRef& indent);

protected:
	VariableKind
	ensureVariableKind();

	ModuleItem*
	findBaseType(const sl::StringRef& name);

	size_t
	buildLuaBaseTypeNameList(sl::BoxList<sl::String>* list);

	bool
	generateVariableDocumentation(
		const sl::StringRef& outputDir,
		sl::String* itemXml,
		sl::String* indexXml
	);

	bool
	generateLuaBaseTypeDocumentation(sl::String* itemXml);

	bool
	generateLuaClassDocumentation(
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

	bool
	generateLuaBaseTypeDoxygenFilterOutput(const sl::StringRef& indent);

	void
	generateLuaClassDoxygenFilterOutput(const sl::StringRef& indent);

	void
	generateLuaEnumDoxygenFilterOutput(const sl::StringRef& indent);
};

//..............................................................................

struct FunctionName {
	sl::BoxList<sl::StringRef> m_list;
	sl::StringRef m_name;
	bool m_isMethod;

	FunctionName() {
		m_isMethod = false;
	}
};

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

struct FunctionParamArray {
	sl::Array<Variable*> m_array;
	bool m_isVarArg;

	FunctionParamArray() {
		m_isVarArg = false;
	}
};

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

struct Function: ModuleItem {
	FunctionParamArray m_paramArray;
	bool m_isMethod;

	Function();

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

class Module {
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
		m_doxyModule(doxyHost) {}

	dox::Host* getDoxyHost() {
		return m_doxyModule.getHost();
	}

	ModuleItem*
	findItem(const sl::StringRef& name) {
		return m_itemMap.findValue(name, NULL);
	}

	Variable*
	createVariable(
		const sl::StringRef& name,
		ModuleItemKind itemKind = ModuleItemKind_Variable
	);

	Variable*
	createTableVariable(
		const sl::StringRef& name,
		ModuleItemKind itemKind = ModuleItemKind_Variable
	);

	Function*
	createFunction(const sl::StringRef& name = sl::StringRef());

	Table*
	createTable();

	Table*
	findTable(const sl::StringRef& name);

	Table*
	findTableField(
		Table* table,
		const sl::StringRef& name
	);

	bool
	addSource(const sl::String& source) {
		return m_sourceList.insertTail(source) != NULL;
	}

	bool
	generateGlobalNamespaceDocumentation(
		const sl::StringRef& outputDir,
		sl::String* globalXml,
		sl::String* indexXml
	);

	void
	generateDoxygenFilterOutput();
};

//..............................................................................

inline
dox::Block*
ModuleItem::ensureDoxyBlock() {
	return m_doxyBlock ? m_doxyBlock : m_module->getDoxyHost()->getItemBlock(this);
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

inline
ModuleItem*
Variable::findBaseType(const sl::StringRef& name) {
	return m_table ?
		(ModuleItem*)m_table->findField(name) :
		m_module->findItem(name);
}

//..............................................................................
