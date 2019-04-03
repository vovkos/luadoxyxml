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

// warning C4065: switch statement contains 'default' but no 'case' labels

#pragma warning(disable: 4065)

//..............................................................................

%%{

machine lua;
write data;

#. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
#
# prepush / postpop (for fcall/fret)
#

prepush
{
	stack = prePush();
}

postpop
{
	postPop();
}

#. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
#
# standard definitions
#

dec    = [0-9];
hex    = [0-9a-fA-F];
oct    = [0-7];
bin    = [01];
id     = [_a-zA-Z] [_a-zA-Z0-9]*;
ws     = [ \t\r]+;
nl     = '\n' @{ newLine(p + 1); };
lc_nl  = '\\' '\r'? nl;
esc    = '\\' [^\n];
lit_dq = '"' ([^"\n\\] | esc)* (["\\] | nl);
lit_sq = "'" ([^'\n\\] | esc)* (['\\] | nl);

#. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
#
# main machine
#

main := |*

'goto'           { createToken(LuaTokenKind_Goto); };
'break'          { createToken(LuaTokenKind_Break); };
'return'         { createToken(LuaTokenKind_Return); };
'do'             { createToken(LuaTokenKind_Do); };
'end'            { createToken(LuaTokenKind_End); };
'while'          { createToken(LuaTokenKind_While); };
'repeat'         { createToken(LuaTokenKind_Repeat); };
'until'          { createToken(LuaTokenKind_Until); };
'if'             { createToken(LuaTokenKind_If); };
'then'           { createToken(LuaTokenKind_Then); };
'elseif'         { createToken(LuaTokenKind_ElseIf); };
'else'           { createToken(LuaTokenKind_Else); };
'for'            { createToken(LuaTokenKind_For); };
'function'       { createToken(LuaTokenKind_Function); };
'local'          { createToken(LuaTokenKind_Local); };
'in'             { createToken(LuaTokenKind_In); };
'nil'            { createToken(LuaTokenKind_Nil); };
'false'          { createToken(LuaTokenKind_False); };
'true'           { createToken(LuaTokenKind_True); };
'or'             { createToken(LuaTokenKind_Or); };
'and'            { createToken(LuaTokenKind_And); };
'not'            { createToken(LuaTokenKind_Not); };

'::'             { createToken(LuaTokenKind_Context); };
'...'            { createToken(LuaTokenKind_Ellipsis); };
'<='             { createToken(LuaTokenKind_Le); };
'>='             { createToken(LuaTokenKind_Ge); };
'~='             { createToken(LuaTokenKind_Ne); };
'=='             { createToken(LuaTokenKind_Eq); };
'<<'             { createToken(LuaTokenKind_Shl); };
'>>'             { createToken(LuaTokenKind_Shr); };
'..'             { createToken(LuaTokenKind_Concat); };
'//'             { createToken(LuaTokenKind_FloorDiv); };

id               { createStringToken(LuaTokenKind_Identifier); };
lit_sq | lit_dq  { createStringToken(LuaTokenKind_String, 1, 1); };
dec+             { createIntegerToken (10); };
'0' [xX] hex+    { createIntegerToken (16, 2); };
dec+ ('.' dec*) | ([eE] [+\-]? dec+)
				 { createFpToken (); };

'--!' [^\n]*     { createDoxyCommentToken(); };
'--[[!' (any | nl)* :>> '--]]'
				 { createDoxyCommentToken(); };

'--' [^\n]*      ;
'--[[' (any | nl)* :>> '--]]'
				 ;

ws | nl ;
print            { createToken(ts[0]); };
any              { createErrorToken(ts[0]); };

*|;

}%%

//..............................................................................

void
LuaLexer::init()
{
	%% write init;
}

void
LuaLexer::exec()
{
	%% write exec;
}

//..............................................................................
