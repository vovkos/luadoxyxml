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

void
Value::clear()
{
	m_valueKind = ValueKind_Empty;
	m_table = NULL;
	m_source.clear();
}

void
Value::setFirstToken(
	const Token::Pos& pos,
	ValueKind valueKind
	)
{
	ASSERT(m_valueKind == ValueKind_Empty);

	m_firstTokenPos = pos;
	m_lastTokenPos = pos;
	m_valueKind = valueKind;
	m_source = sl::StringRef(pos.m_p, pos.m_length);
}

void
Value::appendSource(
	const Token::Pos& pos,
	ValueKind valueKind
	)
{
	ASSERT(m_valueKind != ValueKind_Empty);

	m_lastTokenPos = pos;
	m_valueKind = valueKind;

	if (valueKind != ValueKind_Table)
		m_table = NULL;
	else
		ASSERT(m_table);

	ASSERT(!m_source.isEmpty());
	const char* p = m_source.cp();
	const char* end = pos.m_p + pos.m_length;
	m_source = sl::StringRef(p, end - p);
}

//..............................................................................

ModuleItem::ModuleItem()
{
	m_itemKind = ModuleItemKind_Undefined;
	m_module = NULL;
	m_doxyBlock = NULL;
}

void
ModuleItem::printDoxygenFilterComment(const sl::StringRef& indent)
{
	if (m_doxyBlock)
		printf("%s/*! %s */\n", indent.sz(), m_doxyBlock->getSource().getTrimmedString().sz());
}

//..............................................................................

sl::String
Variable::createDoxyRefId()
{
	sl::String refId;

	switch (m_itemKind)
	{
	case ModuleItemKind_Field:
		refId = "field_";
		break;

	case ModuleItemKind_FunctionParam:
		refId = "param_";
		break;

	default:
		refId =
			isLuaStruct() ? "struct_" :
			isLuaEnum() ? "enum_" :
			"variable_";
	}

	refId += m_name;
	refId.makeLowerCase();

	return m_module->m_doxyModule.adjustRefId(refId);
}

bool
Variable::isLuaStruct()
{
	if (!m_initializer.m_table)
		return false;

	m_module->m_doxyModule.getHost()->getItemBlock(this);
	return m_doxyBlock->getInternalDescription().find(":luastruct:") != -1;
}

bool
Variable::isLuaEnum()
{
	if (!m_initializer.m_table)
		return false;

	m_module->m_doxyModule.getHost()->getItemBlock(this);
	return m_doxyBlock->getInternalDescription().find(":luaenum:") != -1;
}

bool
Variable::generateDocumentation(
	const sl::StringRef& outputDir,
	sl::String* itemXml,
	sl::String* indexXml
	)
{
	return
		isLuaStruct() ? generateLuaStructDocumentation(outputDir, itemXml, indexXml) :
		isLuaEnum() ? generateLuaEnumDocumentation(outputDir, itemXml, indexXml) :
		generateVariableDocumentation(outputDir, itemXml, indexXml);
}

void
Variable::generateDoxygenFilterOutput(const sl::StringRef& indent)
{
	printDoxygenFilterComment();

	if (isLuaStruct())
		generateLuaStructDoxygenFilterOutput(indent);
	else if (isLuaEnum())
		generateLuaEnumDoxygenFilterOutput(indent);
	else
		generateVariableDoxygenFilterOutput(indent);
}

bool
Variable::generateVariableDocumentation(
	const sl::StringRef& outputDir,
	sl::String* itemXml,
	sl::String* indexXml
	)
{
	m_module->m_doxyModule.getHost()->getItemBlock(this);

	itemXml->format("<memberdef kind='variable' id='%s'>\n", m_doxyBlock->getRefId ().sz());
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
Variable::generateLuaStructDocumentation(
	const sl::StringRef& outputDir,
	sl::String* itemXml,
	sl::String* indexXml
	)
{
	ASSERT(m_initializer.m_table && m_doxyBlock);

	indexXml->appendFormat(
		"<compound kind='struct' refid='%s'><name>%s</name></compound>\n",
		m_doxyBlock->getRefId().sz(),
		m_name.sz()
		);

	itemXml->format(
		"<compounddef kind='struct' id='%s' language='Lua'>\n"
		"<compoundname>%s</compoundname>\n",
		m_doxyBlock->getRefId().sz(),
		m_name.sz()
		);

	itemXml->append("<sectiondef>\n");

	sl::String fieldXml;
	size_t count = m_initializer.m_table->m_fieldArray.getCount();
	for (size_t i = 0; i < count; i++)
	{
		Variable* field = m_initializer.m_table->m_fieldArray[i];
		if (field->m_name.isEmpty())
			continue;

		field->generateDocumentation(outputDir, &fieldXml, indexXml);
		itemXml->append(fieldXml);
	}

	itemXml->append("</sectiondef>\n");

	sl::String footnoteXml = m_doxyBlock->getFootnoteString();
	if (!footnoteXml.isEmpty())
	{
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
	)
{
	ASSERT(m_initializer.m_table && m_doxyBlock);

	itemXml->format(
		"<memberdef kind='enum' id='%s' language='Lua'>\n"
		"<name>%s</name>\n",
		m_doxyBlock->getRefId().sz(),
		m_name.sz()
		);

	sl::String fieldXml;
	size_t count = m_initializer.m_table->m_fieldArray.getCount();
	for (size_t i = 0; i < count; i++)
	{
		Variable* field = m_initializer.m_table->m_fieldArray[i];
		if (field->m_initializer.isEmpty())
			continue;

		dox::Block* fieldDoxyBlock = m_module->m_doxyModule.getHost()->getItemBlock(field);

		itemXml->appendFormat("<enumvalue id='%s'>\n", fieldDoxyBlock->getRefId ().sz());
		itemXml->appendFormat("<name>%s_%d</name>\n", m_name.sz(), i);
		itemXml->appendFormat("<initializer>= %s</initializer>\n", field->m_initializer.m_source.sz());
		itemXml->append(fieldDoxyBlock->getDescriptionString());
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
Variable::generateVariableDoxygenFilterOutput(const sl::StringRef& indent)
{
	printf("%sint %s", indent.sz(), m_name.sz());

	if (m_itemKind != ModuleItemKind_FunctionParam)
		printf(";\n");
}

void
Variable::generateLuaStructDoxygenFilterOutput(const sl::StringRef& indent)
{
	printf("struct %s\n{\n", m_name.sz());

	size_t count = m_initializer.m_table->m_fieldArray.getCount();
	for (size_t i = 0; i < count; i++)
	{
		Variable* field = m_initializer.m_table->m_fieldArray[i];
		if (!field->m_name.isEmpty())
			field->generateDoxygenFilterOutput("\t");
	}

	printf("};\n\n");
}

void
Variable::generateLuaEnumDoxygenFilterOutput(const sl::StringRef& indent)
{
	printf("enum %s\n{\n", m_name.sz());

	size_t count = m_initializer.m_table->m_fieldArray.getCount();
	for (size_t i = 0; i < count; i++)
	{
		Variable* field = m_initializer.m_table->m_fieldArray[i];
		if (field->m_initializer.isEmpty())
			continue;

		field->printDoxygenFilterComment("\t");
		printf("\t%s_%d = %s,\n", m_name.sz(), i, field->m_initializer.m_source.sz());
	}

	printf("};\n\n");
}

//..............................................................................

FunctionName&
FunctionName::operator = (const FunctionName& src)
{
	m_first = src.m_first;
	m_isMethod = src.m_isMethod;

	sl::ConstBoxIterator<sl::StringRef> it = src.m_list.getHead();
	for (; it; it++)
		m_list.insertTail(*it);

	return *this;
}

sl::StringRef
FunctionName::getFullName() const
{
	if (m_list.isEmpty())
		return m_first;

	sl::ConstBoxIterator<sl::StringRef> it = m_list.getHead();
	sl::ConstBoxIterator<sl::StringRef> end = m_list.getTail();

	sl::String fullName = m_first;
	for (; it != end; it++)
	{
		fullName += '.';
		fullName += *it;
	}

	fullName += m_isMethod ? '.' : ':';
	fullName += *end;
	return fullName;
}

//..............................................................................

sl::String
Function::createDoxyRefId()
{
	sl::String refId = "function_" + m_name.getFullName();
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
	)
{
	dox::Block* doxyBlock = m_module->m_doxyModule.getHost()->getItemBlock(this);

	itemXml->format("<memberdef kind='function' id='%s'>\n", doxyBlock->getRefId ().sz());
	itemXml->appendFormat("<name>%s</name>\n", m_name.getFullName().sz());

	size_t count = m_paramArray.m_array.getCount();
	for (size_t i = 0; i < count; i++)
	{
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

	itemXml->append(doxyBlock->getImportString());
	itemXml->append(doxyBlock->getDescriptionString());
	itemXml->append(getLocationString());
	itemXml->append("</memberdef>\n");
	return true;
}

void
Function::generateDoxygenFilterOutput(const sl::StringRef& indent)
{
	printDoxygenFilterComment();
	printf("int %s(\n", m_name.getFullName().sz());

	if (!m_paramArray.m_array.isEmpty())
	{
		m_paramArray.m_array[0]->generateDoxygenFilterOutput("\t");

		size_t count = m_paramArray.m_array.getCount();
		for (size_t i = 1; i < count; i++)
		{
			printf(",\n");
			m_paramArray.m_array[i]->generateDoxygenFilterOutput("\t");
		}

		printf(m_paramArray.m_isVarArg ? ",\n\t...\n\t" : "\n\t");
	}
	else if (m_paramArray.m_isVarArg)
	{
		printf("...");
	}

	printf(");\n\n");
}

//..............................................................................

bool
Module::generateGlobalNamespaceDocumentation(
	const sl::StringRef& outputDir,
	sl::String* itemXml,
	sl::String* indexXml
	)
{
	bool result;

	dox::Host* host = m_doxyModule.getHost();

	*indexXml = "<compound kind='file' refid='global'><name>global</name></compound>\n";

	*itemXml =
		"<compounddef kind='file' id='global' language='Lua'>\n"
		"<compoundname>global</compoundname>\n";

	static char compoundFileHdr[] =
		"<?xml version='1.0' encoding='UTF-8' standalone='no'?>\n"
		"<doxygen>\n";

	static char compoundFileTerm[] = "</doxygen>\n";

	sl::String sectionDef;
	sl::String memberXml;

	sl::StringHashTableIterator<ModuleItem*> it = m_itemMap.getHead();
	for (; it; it++)
	{
		ModuleItem* item = it->m_value;

		result = item->generateDocumentation(outputDir, &memberXml, indexXml);
		if (!result)
			return false;

		if (memberXml.isEmpty())
			continue;

		dox::Block* doxyBlock = host->getItemBlock(item);
		dox::Group* doxyGroup = doxyBlock->getGroup();
		if (doxyGroup)
			doxyGroup->addItem(item);

		bool isCompoundFile =
			item->m_itemKind == ModuleItemKind_Variable &&
			((Variable*)item)->isLuaStruct();

		if (!isCompoundFile)
		{
			sectionDef.append(memberXml);
		}
		else
		{
			sl::String refId = doxyBlock->getRefId();
			sl::String fileName = sl::String(outputDir) + "/" + refId + ".xml";

			io::File compoundFile;
			result =
				compoundFile.open(fileName, io::FileFlag_Clear) &&
				compoundFile.write(compoundFileHdr, lengthof(compoundFileHdr)) != -1 &&
				compoundFile.write(memberXml, memberXml.getLength()) != -1 &&
				compoundFile.write(compoundFileTerm, lengthof(compoundFileTerm)) != -1;

			if (!result)
				return false;

			itemXml->appendFormat("<innerclass refid='%s'/>\n", refId.sz());
		}
	}

	itemXml->append("<sectiondef>\n");
	itemXml->append(sectionDef);
	itemXml->append("</sectiondef>\n");
	itemXml->append("</compounddef>\n");
	return true;
}

void
Module::generateDoxygenFilterOutput()
{
	sl::StringHashTableIterator<ModuleItem*> it = m_itemMap.getHead();
	for (; it; it++)
		it->m_value->generateDoxygenFilterOutput();
}

//..............................................................................
