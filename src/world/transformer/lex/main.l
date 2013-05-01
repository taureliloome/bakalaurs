%{
#include <stdio.h>
#include "y.tab.h"
%}

%%
\#include               return T_INCLUDE;
int						return T_INT;
char					return T_CHAR;
return					return T_RETURN;
for						return T_FOR;
do						return T_DO;
while					return T_WHILE;
\<						return T_LT;
\>						return T_GT;
==						return T_EQ;
!=						return T_NE;
=						return T_IS;
if						return T_IF;
else					return T_ELSE;
0x[a-fA-F0-9]*			return T_HEX;
[a-zA-Z][a-zA-Z0-9]*    return T_WORD;
[a-zA-Z0-9\/.-]+		return T_FILE;
[0-9]*					return T_DEC;
\(						return T_OBRACKET;
\)						return T_EBRACKET;
\"                      return T_QUOTE;
\{                      return T_OBRACE;
\}                      return T_EBRACE;
\&\&					return T_AND;
\|\|					return T_OR;
;                       return T_SEMICOLON;
,						return T_COMMA;
.						return T_DOT;
\n                      /* ignore EOL */;
[ \t]+                  /* ignore whitespace */;
%%