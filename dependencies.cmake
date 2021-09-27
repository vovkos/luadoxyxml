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

set(
	AXL_PATH_LIST

	AXL_CMAKE_DIR
	GRACO_CMAKE_DIR
	RAGEL_EXE
)

set(
	AXL_IMPORT_LIST

	REQUIRED
		axl
		ragel
		graco
	OPTIONAL
		sphinx
		latex
	)

set(
	AXL_IMPORT_DIR_LIST

	${GRACO_CMAKE_DIR}
)

#...............................................................................
