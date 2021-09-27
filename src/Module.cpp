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
#include "Module.h"

//..............................................................................

Value::Value() {
	m_valueKind = ValueKind_Empty;
	m_table = NULL;
	m_function = NULL;
}

Value::Value(Table* table) {
	m_valueKind = ValueKind_Table;
	m_table = table;
	m_function = NULL;
}

Value::Value(Function* function) {
	m_valueKind = ValueKind_Function;
	m_table = NULL;
	m_function = function;
}

void
Value::clear() {
	m_valueKind = ValueKind_Empty;
	m_table = NULL;
	m_source.clear();
}

void
Value::setFirstToken(
	const Token::Pos& pos,
	ValueKind valueKind
) {
	ASSERT(m_valueKind == ValueKind_Empty);

	m_firstTokenPos = pos;
	m_lastTokenPos = pos;
	m_valueKind = valueKind;
	m_source = sl::StringRef(pos.m_p, pos.m_length);
}

void
Value::appendSource(const Token::Pos& pos) {
	ASSERT(m_valueKind != ValueKind_Empty);
	ASSERT(!m_source.isEmpty());

	const char* p = m_source.cp();
	const char* end = pos.m_p + pos.m_length;
	m_source = sl::StringRef(p, end - p);
	m_lastTokenPos = pos;
}

//..............................................................................

void
Table::addField(Variable* field) {
	m_fieldArray.append(field);

	if (!field->m_name.isEmpty())
		m_fieldMap[field->m_name] = field;

	field->m_table = this;
}

//..............................................................................

ModuleItem::ModuleItem() {
	m_itemKind = ModuleItemKind_Undefined;
	m_module = NULL;
	m_table = NULL;
	m_isLocal = false;
	m_doxyBlock = NULL;
}

void
ModuleItem::printDoxygenFilterComment(const sl::StringRef& indent) {
	if (m_doxyBlock)
		printf("%s/*! %s */\n", indent.sz(), m_doxyBlock->getSource().getTrimmedString().sz());
}

bool
ModuleItem::generateCompoundMemberDocumentation(
	const sl::StringRef& outputDir,
	sl::String* memberXml,
	sl::String* compoundXml,
	sl::String* sectionXml,
	sl::String* indexXml
) {
	return
		generateDocumentation(outputDir, memberXml, indexXml) &&
		sectionXml->append(*memberXml) != -1;
}

//..............................................................................

Variable::Variable() {
	m_itemKind = ModuleItemKind_Variable;
	m_variableKind = VariableKind_Undefined;
}

sl::String
Variable::createDoxyRefId() {
	sl::String refId;

	switch (m_variableKind) {
	case VariableKind_Enum:
		refId = "enum_";
		break;

	case VariableKind_Module:
		refId = "module_";
		break;

	case VariableKind_Class:
		refId = "class_";
		break;

	case VariableKind_Struct:
		refId = "struct_";
		break;

	default:
		switch (m_itemKind) {
		case ModuleItemKind_Field:
			refId = "field_";
			break;

		case ModuleItemKind_FunctionParam:
			refId = "param_";
			break;

		default:
			refId = "variable_";
		}
	}

	refId += m_name;
	refId.makeLowerCase();
	return m_module->m_doxyModule.adjustRefId(refId);
}

void
Variable::setInitializer(const Value& value) {
	m_initializer = value;

	if (value.m_valueKind == ValueKind_Table)
		value.m_table->m_lvalue = this;
}

VariableKind
Variable::ensureVariableKind() {
	if (m_variableKind)
		return m_variableKind;

	if (m_initializer.m_valueKind != ValueKind_Table) {
		m_variableKind = VariableKind_Normal;
		return m_variableKind;
	}

	const sl::String* internalDescription = &ensureDoxyBlock()->getInternalDescription();
	if (internalDescription->find(":luaenum:") != -1)
		m_variableKind = VariableKind_Enum;
	else if (internalDescription->find(":luamodule:") != -1)
		m_variableKind = VariableKind_Module;
	else if (internalDescription->find(":luaclass:") != -1)
		m_variableKind = VariableKind_Class;
	else if (internalDescription->find(":luastruct:") != -1)
		m_variableKind = VariableKind_Struct;
	else
		m_variableKind = VariableKind_Normal;

	return m_variableKind;
}

bool
Variable::generateDocumentation(
	const sl::StringRef& outputDir,
	sl::String* itemXml,
	sl::String* indexXml
) {
	VariableKind variableKind = getVariableKind();

	switch (variableKind) {
	case VariableKind_Enum:
		return generateLuaEnumDocumentation(outputDir, itemXml, indexXml);

	case VariableKind_Class:
	case VariableKind_Struct:
	case VariableKind_Module:
		return generateLuaClassDocumentation(outputDir, itemXml, indexXml);

	default:
		return generateVariableDocumentation(outputDir, itemXml, indexXml);
	}
}

bool
Variable::generateCompoundMemberDocumentation(
	const sl::StringRef& outputDir,
	sl::String* memberXml,
	sl::String* compoundXml,
	sl::String* sectionXml,
	sl::String* indexXml
) {
	bool result = generateDocumentation(outputDir, memberXml, indexXml);
	if (!result)
		return false;

	if (!isLuaClass()) {
		sectionXml->append(*memberXml);
		return true;
	}

	static const char compoundFileHdr[] =
		"<?xml version='1.0' encoding='UTF-8' standalone='no'?>\n"
		"<doxygen>\n";

	static const char compoundFileTerm[] = "</doxygen>\n";

	sl::String refId = ensureDoxyBlock()->getRefId();
	sl::String fileName = sl::String(outputDir) + "/" + refId + ".xml";

	io::File compoundFile;
	result =
		compoundFile.open(fileName, io::FileFlag_Clear) &&
		compoundFile.write(compoundFileHdr, lengthof(compoundFileHdr)) != -1 &&
		compoundFile.write(*memberXml, memberXml->getLength()) != -1 &&
		compoundFile.write(compoundFileTerm, lengthof(compoundFileTerm)) != -1;

	if (!result)
		return false;

	compoundXml->appendFormat("<innerclass refid='%s'/>\n", refId.sz());
	return true;
}

void
Variable::generateDoxygenFilterOutput(const sl::StringRef& indent) {
	printDoxygenFilterComment();

	VariableKind variableKind = getVariableKind();

	switch (variableKind) {
	case VariableKind_Enum:
			return generateLuaEnumDoxygenFilterOutput(indent);

	case VariableKind_Class:
	case VariableKind_Struct:
	case VariableKind_Module:
		return generateLuaClassDoxygenFilterOutput(indent);

	default:
		return generateVariableDoxygenFilterOutput(indent);
	}
}

size_t
Variable::buildLuaBaseTypeNameList(sl::BoxList<sl::String>* list) {
	static char token[] = ":luabasetype(";

	sl::String text = m_doxyBlock->getInternalDescription();
	size_t pos = 0;
	char sep = ':';

	for (size_t i = 0;; i++) {
		pos = text.find(token, pos);
		if (pos == -1)
			break;

		pos += lengthof(token);
		size_t pos2 = text.find(')', pos);
		if (pos2 == -1)
			break;

		list->insertTail(text.getSubString(pos, pos2 - pos));
		pos = pos2 + 1;
	}

	return true;
}

bool
Variable::generateVariableDocumentation(
	const sl::StringRef& outputDir,
	sl::String* itemXml,
	sl::String* indexXml
) {
	ensureDoxyBlock();

	itemXml->format(
		"<memberdef kind='variable' id='%s' %s>\n",
		m_doxyBlock->getRefId ().sz(),
		m_isLocal ? " static='yes'" : ""
	);

	itemXml->appendFormat("<name>%s</name>\n", m_name.sz());

	if (!m_initializer.m_source.isEmpty())
		itemXml->appendFormat("<initializer>= %s</initializer>\n", m_initializer.m_source.sz());

	itemXml->append(m_doxyBlock->getImportString());
	itemXml->append(m_doxyBlock->getDescriptionString());
	itemXml->append(getLocationString());
	itemXml->append("</memberdef>\n");

	return true;
}

bool
Variable::generateLuaBaseTypeDocumentation(sl::String* itemXml) {
	sl::BoxList<sl::String> baseTypeNameList;
	buildLuaBaseTypeNameList(&baseTypeNameList);

	sl::BoxIterator<sl::String> it = baseTypeNameList.getHead();
	for (; it; it++) {
		const sl::String& baseTypeName = *it;
		ModuleItem* baseType = findBaseType(baseTypeName);
		if (!baseType) {
			fprintf(stderr, "\\luabasetype %s not found\n", baseTypeName.sz());
			continue;
		}

		baseType->ensureDoxyBlock();

		itemXml->appendFormat(
			"<basecompoundref refid='%s'>%s</basecompoundref>\n",
			baseType->m_doxyBlock->getRefId().sz(),
			baseTypeName.sz()
		);
	}

	return true;
}

bool
Variable::generateLuaClassDocumentation(
	const sl::StringRef& outputDir,
	sl::String* itemXml,
	sl::String* indexXml
) {
	ASSERT(m_initializer.m_table && m_doxyBlock);

	VariableKind variableKind = getVariableKind();
	const char* doxyCompoundKind;
	switch (variableKind) {
	case VariableKind_Module:
		doxyCompoundKind = "namespace";
		break;

	case VariableKind_Struct:
		doxyCompoundKind = "struct";
		break;

	default:
		doxyCompoundKind = "class";
		break;
	}

	indexXml->appendFormat(
		"<compound kind='%s' refid='%s'><name>%s</name></compound>\n",
		doxyCompoundKind,
		m_doxyBlock->getRefId().sz(),
		m_name.sz()
	);

	itemXml->format(
		"<compounddef kind='%s' id='%s' language='Lua'>\n"
		"<compoundname>%s</compoundname>\n",
		doxyCompoundKind,
		m_doxyBlock->getRefId().sz(),
		m_name.sz()
	);

	generateLuaBaseTypeDocumentation(itemXml);

	sl::String fieldXml;
	sl::String sectionDef;

	size_t count = m_initializer.m_table->m_fieldArray.getCount();
	for (size_t i = 0; i < count; i++) {
		Variable* field = m_initializer.m_table->m_fieldArray[i];
		if (field->m_name.isEmpty())
			continue;

		if (field->m_initializer.m_valueKind != ValueKind_Function) {
			field->generateCompoundMemberDocumentation(outputDir, &fieldXml, itemXml, &sectionDef, indexXml);
		} else {
			if (field->m_initializer.m_function->m_name.isEmpty()) {
				field->m_initializer.m_function->m_name = field->m_name;
				field->m_initializer.m_function->m_table = m_initializer.m_table;
			}

			field->m_initializer.m_function->generateDocumentation(outputDir, &fieldXml, indexXml);
			sectionDef.append(fieldXml);
		}
	}

	itemXml->append("<sectiondef>\n");
	itemXml->append(sectionDef);
	itemXml->append("</sectiondef>\n");

	sl::String footnoteXml = m_doxyBlock->getFootnoteString();
	if (!footnoteXml.isEmpty()) {
		itemXml->append("<sectiondef>\n");
		itemXml->append(footnoteXml);
		itemXml->append("</sectiondef>\n");
	}

	itemXml->append(m_doxyBlock->getImportString());
	itemXml->append(m_doxyBlock->getDescriptionString());
	itemXml->append(getLocationString());
	itemXml->append("</compounddef>\n");

	return true;
}

bool
Variable::generateLuaEnumDocumentation(
	const sl::StringRef& outputDir,
	sl::String* itemXml,
	sl::String* indexXml
) {
	ASSERT(m_initializer.m_table && m_doxyBlock);

	itemXml->format(
		"<memberdef kind='enum' id='%s' language='Lua'>\n"
		"<name>%s</name>\n",
		m_doxyBlock->getRefId().sz(),
		m_name.sz()
	);

	sl::String fieldXml;
	size_t count = m_initializer.m_table->m_fieldArray.getCount();
	for (size_t i = 0; i < count; i++) {
		Variable* field = m_initializer.m_table->m_fieldArray[i];
		if (field->m_initializer.isEmpty())
			continue;

		field->ensureDoxyBlock();

		itemXml->appendFormat("<enumvalue id='%s'>\n", field->m_doxyBlock->getRefId ().sz());
		itemXml->appendFormat("<name>%s_%d</name>\n", m_name.sz(), i);
		itemXml->appendFormat("<initializer>= %s</initializer>\n", field->m_initializer.m_source.sz());
		itemXml->append(field->m_doxyBlock->getDescriptionString());
		itemXml->append(field->getLocationString());
		itemXml->append("</enumvalue>\n");
	}

	sl::String footnoteXml = m_doxyBlock->getFootnoteString();
	if (!footnoteXml.isEmpty())
		itemXml->append(footnoteXml);

	itemXml->append(m_doxyBlock->getImportString());
	itemXml->append(m_doxyBlock->getDescriptionString());
	itemXml->append(getLocationString());
	itemXml->append("</memberdef>\n");

	return true;
}

void
Variable::generateVariableDoxygenFilterOutput(const sl::StringRef& indent) {
	printf(
		"%s%sint %s",
		indent.sz(),
		m_isLocal ? "static " : "",
		m_name.sz()
	);

	if (m_itemKind != ModuleItemKind_FunctionParam)
		printf(";\n");
}

bool
Variable::generateLuaBaseTypeDoxygenFilterOutput(const sl::StringRef& indent) {
	sl::BoxList<sl::String> baseTypeNameList;
	buildLuaBaseTypeNameList(&baseTypeNameList);

	sl::BoxIterator<sl::String> it = baseTypeNameList.getHead();
	for (size_t i = 0; it; it++, i++)
		printf("%s\t%c %s\n", indent.sz(), i ? ',' : ':', it->sz());

	return true;
}

void
Variable::generateLuaClassDoxygenFilterOutput(const sl::StringRef& indent) {
	ASSERT(m_initializer.m_table && m_doxyBlock);

	VariableKind variableKind = getVariableKind();
	const char* cppKeyword;
	switch (variableKind) {
	case VariableKind_Module:
		cppKeyword = "namespace";
		break;

	case VariableKind_Struct:
		cppKeyword = "struct";
		break;

	default:
		cppKeyword = "class";
		break;
	}

	printf("%s %s\n", cppKeyword, m_name.sz());
	generateLuaBaseTypeDoxygenFilterOutput(indent);
	printf("{\n");

	size_t count = m_initializer.m_table->m_fieldArray.getCount();
	for (size_t i = 0; i < count; i++) {
		Variable* field = m_initializer.m_table->m_fieldArray[i];
		if (field->m_name.isEmpty())
			continue;

		if (field->m_initializer.m_valueKind != ValueKind_Function) {
			field->generateDoxygenFilterOutput("\t");
		} else {
			if (field->m_initializer.m_function->m_name.isEmpty()) {
				field->m_initializer.m_function->m_name = field->m_name;
				field->m_initializer.m_function->m_table = m_initializer.m_table;
			}

			field->m_initializer.m_function->generateDoxygenFilterOutput("\t");
		}
	}

	printf("};\n\n");
}

void
Variable::generateLuaEnumDoxygenFilterOutput(const sl::StringRef& indent) {
	ASSERT(m_initializer.m_table && m_doxyBlock);

	printf("enum %s\n{\n", m_name.sz());

	size_t count = m_initializer.m_table->m_fieldArray.getCount();
	for (size_t i = 0; i < count; i++) {
		Variable* field = m_initializer.m_table->m_fieldArray[i];
		if (field->m_initializer.isEmpty())
			continue;

		field->printDoxygenFilterComment("\t");
		printf("\t%s_%d = %s,\n", m_name.sz(), i, field->m_initializer.m_source.sz());
	}

	printf("};\n\n");
}

//..............................................................................

Function::Function() {
	m_itemKind = ModuleItemKind_Function;
	m_isMethod = false;
}

sl::String
Function::createDoxyRefId() {
	sl::String refId = "function_" + m_name;
	refId.replace('.', '_');
	refId.replace(':', '_');
	refId.makeLowerCase();

	return m_module->m_doxyModule.adjustRefId(refId);
}

bool
Function::generateDocumentation(
	const sl::StringRef& outputDir,
	sl::String* itemXml,
	sl::String* indexXml
) {
	ensureDoxyBlock();

	itemXml->format(
		"<memberdef kind='function' id='%s'%s%s>\n",
		m_doxyBlock->getRefId ().sz(),
		m_isLocal ? " static='yes'" : "",
		m_isMethod ? " virt='virtual'" : ""
	);

	itemXml->appendFormat("<name>%s</name>\n", m_name.sz());

	size_t count = m_paramArray.m_array.getCount();
	for (size_t i = 0; i < count; i++) {
		Variable* arg = m_paramArray.m_array[i];

		itemXml->appendFormat(
			"<param>\n"
			"<declname>%s</declname>\n",
			arg->m_name.sz()
		);

		if (!arg->m_initializer.m_source.isEmpty())
			itemXml->appendFormat(
				"<defval>%s</defval>\n",
				arg->m_initializer.m_source.sz()
			);

		if (arg->m_doxyBlock)
			itemXml->append(arg->m_doxyBlock->getDescriptionString());

		itemXml->append("</param>\n");
	}

	if (m_paramArray.m_isVarArg)
		itemXml->append(
			"<param>\n"
			"<type>...</type>\n"
			"</param>\n"
		);

	itemXml->append(m_doxyBlock->getImportString());
	itemXml->append(m_doxyBlock->getDescriptionString());
	itemXml->append(getLocationString());
	itemXml->append("</memberdef>\n");
	return true;
}

void
Function::generateDoxygenFilterOutput(const sl::StringRef& indent) {
	printDoxygenFilterComment();

	printf(
		"%s%s%sint %s(",
		indent.sz(),
		m_isLocal ? "static " : "",
		m_isMethod ? "virtual " : "",
		m_name.sz()
	);

	if (m_paramArray.m_array.isEmpty()) {
		printf(m_paramArray.m_isVarArg ? "...);\n" : ");\n");
		return;
	}

	char buffer[256];
	sl::String paramIndent(rc::BufKind_Stack, buffer, sizeof(buffer));
	paramIndent = indent + '\t';

	size_t count = m_paramArray.m_array.getCount();
	for (size_t i = 0; i < count; i++) {
		printf(i ? ",\n" : "\n");
		m_paramArray.m_array[i]->generateDoxygenFilterOutput(paramIndent);
	}

	if (m_paramArray.m_isVarArg)
		printf(",\n%s...\n%s);\n", paramIndent.sz(), paramIndent.sz());
	else
		printf("\n%s);\n", paramIndent.sz());
}

//..............................................................................

Variable*
Module::createVariable(
	const sl::StringRef& name,
	ModuleItemKind itemKind
) {
	Variable* variable = AXL_MEM_NEW(Variable);
	variable->m_itemKind = itemKind;
	variable->m_module = this;
	variable->m_name = name;
	m_itemList.insertTail(variable);
	return variable;
}

Variable*
Module::createTableVariable(
	const sl::StringRef& name,
	ModuleItemKind itemKind
) {
	Variable* variable = createVariable(name, itemKind);
	variable->setInitializer(createTable());
	return variable;
}

Function*
Module::createFunction(const sl::StringRef& name) {
	Function* function = AXL_MEM_NEW(Function);
	function->m_module = this;
	function->m_name = name;
	m_itemList.insertTail(function);
	return function;
}

Table*
Module::createTable() {
	Table* table = AXL_MEM_NEW(Table);
	m_tableList.insertTail(table);
	return table;
}

Table*
Module::findTable(const sl::StringRef& name) {
	sl::StringHashTableIterator<ModuleItem*> it = m_itemMap.find(name);
	if (!it || it->m_value->m_itemKind != ModuleItemKind_Variable)
		return NULL;

	Variable* variable = (Variable*)it->m_value;
	return variable->m_initializer.m_table;
}

Table*
Module::findTableField(
	Table* table,
	const sl::StringRef& name
) {
	sl::StringHashTableIterator<Variable*> it = table->m_fieldMap.find(name);
	if (!it)
		return NULL;

	Variable* field = it->m_value;
	return field->m_initializer.m_table;
}

bool
Module::generateGlobalNamespaceDocumentation(
	const sl::StringRef& outputDir,
	sl::String* globalXml,
	sl::String* indexXml
) {
	bool result;

	dox::Host* host = m_doxyModule.getHost();

	*indexXml = "<compound kind='file' refid='global'><name>global</name></compound>\n";

	*globalXml =
		"<compounddef kind='file' id='global' language='Lua'>\n"
		"<compoundname>global</compoundname>\n";

	sl::String itemXml;
	sl::String sectionDef;

	sl::StringHashTableIterator<ModuleItem*> it = m_itemMap.getHead();
	for (; it; it++) {
		ModuleItem* item = it->m_value;

		result = item->generateCompoundMemberDocumentation(outputDir, &itemXml, globalXml, &sectionDef, indexXml);
		if (!result)
			return false;

		dox::Block* doxyBlock = item->ensureDoxyBlock();
		dox::Group* doxyGroup = doxyBlock->getGroup();
		if (doxyGroup)
			doxyGroup->addItem(item);
	}

	globalXml->append("<sectiondef>\n");
	globalXml->append(sectionDef);
	globalXml->append("</sectiondef>\n");
	globalXml->append("</compounddef>\n");
	return true;
}

void
Module::generateDoxygenFilterOutput() {
	sl::StringHashTableIterator<ModuleItem*> it = m_itemMap.getHead();
	for (; it; it++)
		it->m_value->generateDoxygenFilterOutput();
}

//..............................................................................
