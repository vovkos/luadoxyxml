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
#include "CmdLine.h"
#include "LuaLexer.h"
#include "LuaParser.llk.h"
#include "LuaParser.llk.cpp"
#include "version.h"

#define _PRINT_USAGE_IF_NO_ARGUMENTS 1
#define _PRINT_MODULE 0

//..............................................................................

void
printVersion()
{
	printf(
		"luadoxyxml v%d.%d.%d (%s%s)\n",
		VERSION_MAJOR,
		VERSION_MINOR,
		VERSION_REVISION,
		AXL_CPU_STRING,
		AXL_DEBUG_SUFFIX
		);
}

void
printUsage()
{
	printVersion();

	sl::String helpString = CmdLineSwitchTable::getHelpString();
	printf("Usage: luadoxyxml [options] <source.lua>...\n%s", helpString.sz());
}

bool
run(CmdLine* cmdLine)
{
	bool result;

	io::SimpleMappedFile file;
	Module module;
	LuaLexer lexer;
	LuaParser parser(&module);

	sl::ConstBoxIterator<sl::String> it = cmdLine->m_inputFileNameList.getHead();
	for (; it; it++)
	{
		const sl::String& fileName = *it;
		printf("Parsing %s...\n", fileName.sz());

		result = file.open(fileName, io::FileFlag_ReadOnly);
		if (!result)
			return false;

		sl::StringRef source((const char*)file.p(), file.getMappingSize());
		lexer.create(fileName, source);
		parser.create(SymbolKind_block);

		for (;;)
		{
			const LuaToken* token = lexer.getToken();

			sl::StringRef comment;
			ModuleItem* lastDeclaredItem;

			switch (token->m_token)
			{
			case LuaTokenKind_Error:
				err::setFormatStringError("invalid character '\\x%02x'", (uchar_t) token->m_data.m_integer);
				lex::pushSrcPosError(fileName, token->m_pos);
				return false;

			case LuaTokenKind_DoxyComment_sl:
			case LuaTokenKind_DoxyComment_ml:
				comment = token->m_data.m_string;
				lastDeclaredItem = NULL;

				if (token->m_token == LuaTokenKind_DoxyComment_sl &&
					!comment.isEmpty() && comment[0] == '<')
				{
					lastDeclaredItem = parser.m_lastDeclaredItem;
					comment = comment.getSubString(1);
				}

				/* parser.m_doxyParser.addComment(
					comment,
					token->m_pos,
					isSingleLine,
					lastDeclaredItem
					); */

				break;

			default:
				result = parser.parseToken(token);
				if (!result)
				{
					lex::ensureSrcPosError(fileName, token->m_pos);
					return false;
				}
			}

			if (token->m_token == LuaTokenKind_Eof) // EOF token must be parsed
				break;

			lexer.nextToken();
		}
	}

	return true;
}

//..............................................................................

#if (_AXL_OS_WIN)
int
wmain(
	int argc,
	wchar_t* argv[]
	)
#else
int
main(
	int argc,
	char* argv[]
	)
#endif
{
	int result;

#if _AXL_OS_POSIX
	setvbuf(stdout, NULL, _IOLBF, 1024);
#endif

	g::getModule()->setTag("luadoxyxml");
	lex::registerParseErrorProvider();

	CmdLine cmdLine;
	CmdLineParser parser(&cmdLine);

#if _PRINT_USAGE_IF_NO_ARGUMENTS
	if (argc < 2)
	{
		printUsage();
		return 0;
	}
#endif

	result = parser.parse(argc, argv);
	if (!result)
	{
		printf("error parsing command line: %s\n", err::getLastErrorDescription().sz());
		return -1;
	}

	result = 0;

	if (cmdLine.m_flags & CmdLineFlag_Help)
		printUsage();
	else if (cmdLine.m_flags & CmdLineFlag_Version)
		printVersion();
	else
	{
		result = run(&cmdLine);
		if (!result)
		{
			printf("error: %s\n", err::getLastErrorDescription().sz());
			return -1;
		}
	}

	return result;
}

//..............................................................................
