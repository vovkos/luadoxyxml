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
	m_valueKind = ValueKind_Undefined;
	m_table = NULL;
	m_source.clear();
}

//..............................................................................

ModuleItem::ModuleItem()
{
	m_itemKind = ModuleItemKind_Undefined;
	m_module = NULL;
	m_doxyBlock = NULL;
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

FunctionArgList&
FunctionArgList::operator = (const FunctionArgList& src)
{
	m_isVarArg = src.m_isVarArg;

	sl::ConstBoxIterator<sl::StringRef> it = src.m_list.getHead();
	for (; it; it++)
		m_list.insertTail(*it);

	return *this;
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
	itemXml->append(doxyBlock->getImportString());
	itemXml->append(doxyBlock->getDescriptionString());
	itemXml->append(getDoxyLocationString());
	itemXml->append("</memberdef>\n");

	return true;
}

//..............................................................................

sl::String
Variable::createDoxyRefId()
{
	sl::String refId = isTableType() ? "struct_" : "variable_";
	refId += m_name;
	refId.makeLowerCase();

	return m_module->m_doxyModule.adjustRefId(refId);
}

bool
Variable::isTableType()
{
	if (!m_initializer.m_table)
		return false;

	m_module->m_doxyModule.getHost()->getItemBlock(this);
	return m_doxyBlock->getInternalDescription().find(":tabletype:") != -1;
}

bool
Variable::generateDocumentation(
	const sl::StringRef& outputDir,
	sl::String* itemXml,
	sl::String* indexXml
	)
{
	return isTableType()?
		generateTableTypeDocumentation(outputDir, itemXml, indexXml) :
		generateVariableDocumentation(outputDir, itemXml, indexXml);
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
	itemXml->append(getDoxyLocationString());
	itemXml->append("</memberdef>\n");

	return true;
}

bool
Variable::generateTableTypeDocumentation(
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
		"<compounddef kind='struct' id='%s' language='Jancy'>\n"
		"<compoundname>%s</compoundname>\n",
		m_doxyBlock->getRefId().sz(),
		m_name.sz()
		);

	itemXml->append("<sectiondef>\n");

	sl::String fieldXml;
	size_t count = m_initializer.m_table->m_fieldArray.getCount();
	for (size_t i = 0; i < count; i++)
	{
		Field* field = m_initializer.m_table->m_fieldArray[i];
		if (!field->m_name.isEmpty())
		{
			field->generateDocumentation(outputDir, &fieldXml, indexXml);
			itemXml->append(fieldXml);
		}
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
	itemXml->append(getDoxyLocationString());
	itemXml->append("</compounddef>\n");

	return true;
}
//..............................................................................

sl::String
Field::createDoxyRefId()
{
	sl::String refId = "field_" + m_name;
	refId.makeLowerCase();

	return m_module->m_doxyModule.adjustRefId(refId);
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
			((Variable*)item)->isTableType();

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

//..............................................................................
