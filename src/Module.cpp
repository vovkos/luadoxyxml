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
	sl::String refId = "variable_" + m_name;
	refId.makeLowerCase();

	return m_module->m_doxyModule.adjustRefId(refId);
}

bool
Variable::generateDocumentation(
	const sl::StringRef& outputDir,
	sl::String* itemXml,
	sl::String* indexXml
	)
{
	dox::Block* doxyBlock = m_module->m_doxyModule.getHost()->getItemBlock(this);

	itemXml->format("<memberdef kind='variable' id='%s'>\n", doxyBlock->getRefId ().sz());
	itemXml->appendFormat("<name>%s</name>\n", m_name.sz());

	if (!m_initializer.isEmpty())
		itemXml->appendFormat("<initializer>= %s</initializer>\n", m_initializer.sz());

	itemXml->append(doxyBlock->getImportString());
	itemXml->append(doxyBlock->getDescriptionString());
	itemXml->append(getDoxyLocationString());
	itemXml->append("</memberdef>\n");

	return true;
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

	sl::Iterator<ModuleItem> it = m_itemList.getHead();
	for (; it; it++)
	{
		ModuleItem* item = *it;

		result = item->generateDocumentation(outputDir, &memberXml, indexXml);
		if (!result)
			return false;

		if (memberXml.isEmpty())
			continue;

		dox::Block* doxyBlock = host->getItemBlock(item);
		dox::Group* doxyGroup = doxyBlock->getGroup();
		if (doxyGroup)
			doxyGroup->addItem(item);

		bool isCompoundFile = false; // not yet

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

	itemXml->append(sectionDef);
	itemXml->append("</compounddef>\n");
	return true;
}

//..............................................................................
