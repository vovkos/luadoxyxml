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
