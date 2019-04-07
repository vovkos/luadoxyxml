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

#pragma once

#include "DoxyLexer.h"
#include "DoxyBlock.h"
#include "DoxyGroup.h"

struct ModuleItem;
struct ModuleItemDecl;
struct Module;

//..............................................................................

class DoxyMgr
{
protected:
	struct Target: sl::ListLink
	{
		DoxyBlock* m_block;
		DoxyTokenKind m_tokenKind;
		sl::StringRef m_itemName;
		size_t m_overloadIdx;
	};

protected:
	Module* m_module;

	sl::List<DoxyBlock> m_blockList;
	sl::List<DoxyGroup> m_groupList;
	sl::List<DoxyFootnote> m_footnoteList;
	sl::StringHashTable<size_t> m_refIdMap;
	sl::StringHashTable<DoxyGroup*> m_groupMap;
	sl::List<Target> m_targetList;

public:
	DoxyMgr();

	Module*
	getModule()
	{
		return m_module;
	}

	void
	clear();

	sl::ConstList<DoxyBlock>
	getBlockList()
	{
		return m_blockList;
	}

	sl::ConstList<DoxyGroup>
	getGroupList()
	{
		return m_groupList;
	}

	DoxyGroup*
	getGroup(const sl::StringRef& name);

	DoxyBlock*
	createBlock();

	DoxyBlock*
	getDoxyBlock(
		ModuleItem* item,
		ModuleItemDecl* decl
		);

	template <typename T>
	DoxyBlock*
	getDoxyBlock(T* item)
	{
		return getDoxyBlock(item, item);
	};

	DoxyBlock*
	setDoxyBlock(
		ModuleItem* item,
		ModuleItemDecl* decl,
		DoxyBlock* block
		);

	template <typename T>
	DoxyBlock*
	setDoxyBlock(
		T* item,
		DoxyBlock* block
		)
	{
		return setDoxyBlock(item, item, block);
	};

	DoxyFootnote*
	createFootnote();

	sl::String
	adjustRefId(const sl::StringRef& refId);

	void
	setBlockTarget(
		DoxyBlock* block,
		DoxyTokenKind tokenKind,
		const sl::StringRef& itemName,
		size_t overloadIdx
		);

	bool
	resolveBlockTargets();

	void
	deleteEmptyGroups();

	bool
	generateGroupDocumentation(
		const sl::StringRef& outputDir,
		sl::String* indexXml
		);
};

//..............................................................................
