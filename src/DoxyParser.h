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

#include "DoxyLexer.h"

class DoxyGroup;
class DoxyBlock;

struct Module;
struct ModuleItem;
struct Namespace;

//..............................................................................

class DoxyParser
{
protected:
	struct GroupStackEntry
	{
		intptr_t m_namespaceId;
		DoxyGroup* m_group;
	};

	enum DescriptionKind
	{
		DescriptionKind_Detailed = 0,
		DescriptionKind_Brief,
		DescriptionKind_SeeAlso,
	};

	enum BlockTargetKind
	{
		BlockTargetKind_None = 0,
		BlockTargetKind_Member,
		BlockTargetKind_Compound,
	};

protected:
	Module* m_module;

	DoxyBlock* m_block;
	DoxyBlock* m_parentBlock;
	sl::Array<GroupStackEntry> m_groupStack;
	BlockTargetKind m_blockTargetKind;
	DescriptionKind m_descriptionKind;
	sl::String m_firstIndent;
	sl::String m_indent;
	sl::String m_overloadName;
	size_t m_overloadIdx;

public:
	DoxyParser(Module* module);

	DoxyBlock*
	popBlock();

	void
	addComment(
		const sl::StringRef& comment,
		const lex::LineCol& lineCol,
		bool canAppend,
		ModuleItem* lastDeclaredItem
		);

protected:
	void
	setBlockTarget(
		DoxyTokenKind token,
		const sl::StringRef& name
		);
};

//..............................................................................
