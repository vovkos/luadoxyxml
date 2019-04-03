#...............................................................................
#
#  This file is part of the LuaDoxyXML toolkit.
#
#  LuaDoxyXML is distributed under the MIT license.
#  For details see accompanying license.txt file,
#  the public copy of which is also available at:
#  http://tibbo.com/downloads/archive/luadoxyxml/license.txt
#
#...............................................................................

#
# app folder
#

set(
	APP_H_LIST
	CmdLine.h
	LuaLexer.h
	DoxyLexer.h
	Module.h
	version.h.in
	)

set(
	APP_CPP_LIST
	main.cpp
	CmdLine.cpp
	LuaLexer.cpp
	DoxyLexer.cpp
	Module.cpp
	)

set(
	APP_RL_LIST
	LuaLexer.rl
	DoxyLexer.rl
	)

set(
	APP_LLK_LIST
	LuaParser.llk
	)

source_group(
	app
	FILES
	${APP_H_LIST}
	${APP_CPP_LIST}
	${APP_RL_LIST}
	${APP_LLK_LIST}
	)

#. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
#
# res folder
#

set(
	RES_RC_LIST
	res.rc
	)

source_group(
	res
	FILES
	${RES_RC_LIST}
	)

#. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
#
# gen folder
#

set(GEN_DIR ${CMAKE_CURRENT_BINARY_DIR}/gen)
file(MAKE_DIRECTORY ${GEN_DIR})

axl_push_and_set(CMAKE_CURRENT_BINARY_DIR ${GEN_DIR})

configure_file(
	version.h.in
	${GEN_DIR}/version.h
	)

add_ragel_step(
	LuaLexer.rl.cpp
	LuaLexer.rl
	)

add_ragel_step(
	DoxyLexer.rl.cpp
	DoxyLexer.rl
	)

add_graco_double_step(
	LuaParser.llk.h
	LuaParser.llk.cpp
	CppParser.h.in
	CppParser.cpp.in
	LuaParser.llk
	)

axl_pop(CMAKE_CURRENT_BINARY_DIR)

set(
	GEN_H_LIST
	${GEN_DIR}/version.h
	${GEN_DIR}/LuaParser.llk.h
	)

set(
	GEN_CPP_LIST
	${GEN_DIR}/LuaLexer.rl.cpp
	${GEN_DIR}/DoxyLexer.rl.cpp
	# ${GEN_DIR}/LuaParser.llk.cpp  # currently, cmake can't handle double-file output
	)

axl_exclude_from_build(${GEN_CPP_LIST})  # include "*.rl.cpp" & "*.llk.cpp" manually

source_group(
	gen
	FILES
	${GEN_H_LIST}
	${GEN_CPP_LIST}
	)

#. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
#
# pch folder
#

set(PCH_H   pch.h)
set(PCH_CPP pch.cpp)

source_group(
	pch
	FILES
	${PCH_H}
	${PCH_CPP}
	)

#...............................................................................
#
# luadoxyxml (lua-to-doxygen-xml) documentation comments extraction tool
#

include_directories(
	${GRACO_INC_DIR}
	${AXL_INC_DIR}
	${GEN_DIR}
	)

link_directories(
	${AXL_LIB_DIR}
	)

add_executable(
	luadoxyxml
	${PCH_H}
	${PCH_CPP}
	${APP_H_LIST}
	${APP_CPP_LIST}
	${APP_RL_LIST}
	${APP_LLK_LIST}
	${RES_RC_LIST}
	${GEN_H_LIST}
	${GEN_CPP_LIST}
	)

axl_set_pch(
	luadoxyxml
	${PCH_H}
	${PCH_CPP}
	)

target_link_libraries(
	luadoxyxml
	axl_lex
	axl_io
	axl_core
	)

if(UNIX)
	target_link_libraries(
		luadoxyxml
		pthread
		dl
		)

	if(NOT APPLE)
		target_link_libraries(
			luadoxyxml
			rt
			)
	endif()
endif()

#...............................................................................
