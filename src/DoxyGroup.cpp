//..............................................................................
//
//  This file is part of the Jancy toolkit.
//
//  Jancy is distributed under the MIT license.
//  For details see accompanying license.txt file,
//  the public copy of which is also available at:
//  http://tibbo.com/downloads/archive/jancy/license.txt
//
//..............................................................................

#include "pch.h"
#include "DoxyGroup.h"
#include "Module.h"

//..............................................................................

bool
DoxyGroup::generateDocumentation(
	const sl::StringRef& outputDir,
	sl::String* itemXml,
	sl::String* indexXml
	)
{
	indexXml->appendFormat(
		"<compound kind='group' refid='%s'><name>%s</name></compound>\n",
		m_refId.sz(),
		m_name.sz()
		);

	itemXml->format(
		"<compounddef kind='group' id='%s' language='Jancy'>\n"
		"<compoundname>%s</compoundname>\n"
		"<title>",
		m_refId.sz(),
		m_name.sz()
		);

	appendXmlElementContents(itemXml, m_title);
	itemXml->append("</title>\n");

	sl::String sectionDef;

	size_t count = m_itemArray.getCount();
	for (size_t i = 0; i < count; i++)
	{
		ModuleItem* item = m_itemArray[i];

/*
		ModuleItemDecl* decl = item->getDecl();
		if (!decl)
			continue;

		ModuleItemKind itemKind = item->getItemKind();

		bool isCompoundFile =
			itemKind == ModuleItemKind_Namespace ||
			itemKind == ModuleItemKind_Type && ((Type*)item)->getTypeKind() != TypeKind_Enum;

		bool isNamespace = itemKind == ModuleItemKind_Namespace;
		DoxyBlock* doxyBlock = item->getModule()->m_doxyMgr.getDoxyBlock(item, decl);
*/
		AXL_TODO("extract information on is-compound, is-namespace, create a doxy block")

		bool isCompoundFile = false;
		bool isNamespace = false;

		DoxyBlock* doxyBlock = NULL;
		sl::String refId = doxyBlock->getRefId();

		if (!isCompoundFile)
		{
			sectionDef.appendFormat("<memberdef id='%s'/>", refId.sz());
			sectionDef.append('\n');
		}
		else
		{
			const char* elemName = isNamespace ? "innernamespace" : "innerclass";
			itemXml->appendFormat("<%s refid='%s'/>", elemName, refId.sz());
			itemXml->append('\n');
		}
	}

	sectionDef += getFootnoteString();

	if (!sectionDef.isEmpty())
	{
		itemXml->append("<sectiondef>\n");
		itemXml->append(sectionDef);
		itemXml->append("</sectiondef>\n");
	}

	sl::BoxIterator<DoxyGroup*> groupIt = m_groupList.getHead();
	for (; groupIt; groupIt++)
	{
		DoxyGroup* group = *groupIt;
		itemXml->appendFormat("<innergroup refid='%s'/>", group->m_refId.sz());
		itemXml->append('\n');
	}

	itemXml->append(getImportString());
	itemXml->append(getDescriptionString());
	itemXml->append("</compounddef>\n");

	return true;
}

//..............................................................................
