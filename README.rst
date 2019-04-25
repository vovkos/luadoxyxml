.. .............................................................................
..
..  This file is part of the LuaDoxyXML toolkit.
..
..  LuaDoxyXML is distributed under the MIT license.
..  For details see accompanying license.txt file,
..  the public copy of which is also available at:
..  http://tibbo.com/downloads/archive/luadoxyxml/license.txt
..
.. .............................................................................

LuaDoxyXML
==========
.. image:: https://travis-ci.org/vovkos/luadoxxml.svg?branch=master
	:target: https://travis-ci.org/vovkos/luadoxxml
.. image:: https://ci.appveyor.com/api/projects/status/oo9ql7v3gbvkxh3l?svg=true
	:target: https://ci.appveyor.com/project/vovkos/luadoxxml

Abstract
--------

LuaDoxyXML is a tool for extracting `Doxygen <http://www.stack.nl/~dimitri/doxygen/>`_-style comments from **Lua** source files and outputting those as **Doxygen XML**.

It is indended to be used at the first stage (front-end) in the `Doxyrest pipeline <https://github.com/vovkos/doxyrest>`_ for generating beautiful `Sphinx <http://www.sphinx-doc.org>`_-based documentation for Lua APIs and libraries.

Quick HOWTO
-----------

There are two modes of operation for LuaDoxyXML.

Direct mode
~~~~~~~~~~~

In this mode, ``luadoxyxml`` parses one or more Lua files specified via the command-line and generates Doxygen XML database files directly.

.. rubric:: Example

.. code:: none

	$ luadoxyxml -o xml/index.xml main.lua utils.lua

This will generate an XML database out of doxy-comments in ``main.lua`` and ``utils.lua`` and place the resulting XML files into the ``xml/`` subdirectory.

Please note, that in *direct mode* only a small subset of Doxygen `special commands <http://www.doxygen.nl/manual/commands.html>`__ is supported:

* ``\\var``
* ``\\fn``
* ``\\defgroup``/``\\addtogroup``
* ``\\ingroup``
* ``\\title``
* ``\\brief``
* ``\\see``/``\\sa``

If you use **reStructuredText** inside doxy-comments, you don't really need any of the Doxygen formatting commands -- in this case the *direct mode* is the right choice. However, if you *do need* advanced Doxygen commands, then you can use Doxygen as the main processor and ``luadoxyxml`` as a *filter* as described in the next session.

Doxygen filter mode
~~~~~~~~~~~~~~~~~~~

In this mode, ``luadoxyxml`` is used to pre-process ``.lua`` files and output *pseudo-C* declarations and comments which are then handled by Doxygen. The resulting Doxygen XML database will be emitted by Doxygen, and Doxy-comments will be parsed by Doxygen, too -- which means *ALL* Doxygen `special commands <http://www.doxygen.nl/manual/commands.html>`__ will be supported.

To activate this mode, pass ``--doxygen-filter`` via the command-line.

Please note that it's not necessary to do this pre-processing manually for each ``.lua`` file in your project. ``Doxyfile`` has a setting called ``FILTER_PATTERNS`` which allows for automatic invokation of user-defined pre-processors for all or some source files. To make this work for Lua project, your ``Doxyfile`` should contain the following:

.. code:: bash

	# scan the project directory for Lua files
	FILE_PATTERNS = *.lua

	# ... or specify files directly
	INPUT = main.lua utils.lua

	# for each Lua file, invoke luadoxyxml
	FILTER_PATTERNS = *.lua="<path-to-luadoxyxml> --doxygen-filter"

	# specify how and where to emit XML
	GENERATE_XML = YES
	XML_PROGRAMLISTING = NO
	XML_OUTPUT = <doxygen-xml-dir>

When ``Doxyfile`` is prepared, just invoke Doxygen:

.. code:: none

	$ doxygen Doxyfile

This will generate XML database which can then be used in the usual Doxyrest pipeline.

Generating HTML from XML
~~~~~~~~~~~~~~~~~~~~~~~~

Once you have generated a Doxygen XML database, you can use it to build beautiful HTML documentation out of it using Doxyrest and Sphinx.

.. code:: none

	$ doxyrest \
		<doxygen-xml-dir>/index.xml \
		-o <doxyrest-rst-dir>/index.rst \
		-f index.rst.in \
		-F <path-to-doxyrest-frame-dir>/lua \
		-F <path-to-doxyrest-frame-dir>/common

	$ sphinx-quickstart

	$ sphinx-build \
		-b html \
		<doxyrest-rst-dir> \
		<final-html-dir>

Lua tables types
~~~~~~~~~~~~~~~~

Lua uses dynamic duck-typing, so there are no type definitions. However, most Lua programs usually expect table variables and/or arguments to contain certain fields, i.e. belong to a some *duck-type*.

To document these table *duck-types*, LuaDoxyXML provides the ``\\luastruct`` command which can be used as such:

.. code:: lua

	--[[!
		\luastruct
		\brief This is not a variable, this is a table-type.

		Detailed description of ``MyType`` follows here...
	]]

	MyType = {
		field1, --!< field1 documentation
		field2, --!< field2 documentation


		--! field3 documentation

		field3,

		--[[!
			field4 documentation
		]]

		field4,
	}

