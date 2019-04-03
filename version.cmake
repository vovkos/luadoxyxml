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

set(LUADOXYXML_VERSION_MAJOR     1)
set(LUADOXYXML_VERSION_MINOR     0)
set(LUADOXYXML_VERSION_REVISION  0)
set(LUADOXYXML_VERSION_TAG        )

set(LUADOXYXML_VERSION_FULL "${LUADOXYXML_VERSION_MAJOR}.${LUADOXYXML_VERSION_MINOR}.${LUADOXYXML_VERSION_REVISION}")

if(NOT "${LUADOXYXML_VERSION_TAG}" STREQUAL "")
	set(LUADOXYXML_VERSION_TAG_SUFFIX  " ${LUADOXYXML_VERSION_TAG}")
	set(LUADOXYXML_VERSION_FILE_SUFFIX "${LUADOXYXML_VERSION_FULL}-${LUADOXYXML_VERSION_TAG}")
else()
	set(LUADOXYXML_VERSION_TAG_SUFFIX)
	set(LUADOXYXML_VERSION_FILE_SUFFIX "${LUADOXYXML_VERSION_FULL}")
endif()

string(TIMESTAMP LUADOXYXML_VERSION_YEAR  "%Y")
string(TIMESTAMP LUADOXYXML_VERSION_MONTH "%m")
string(TIMESTAMP LUADOXYXML_VERSION_DAY   "%d")

set(LUADOXYXML_VERSION_COMPANY "Tibbo Technology Inc")
set(LUADOXYXML_VERSION_YEARS   "2019-${LUADOXYXML_VERSION_YEAR}")

#...............................................................................
