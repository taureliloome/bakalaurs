%{
#include <stdio.h>
#ifdef LEX
#	define DO(__STR) printf("%s ", #__STR);
#else
#ifdef YACC
#	include "y.tab.h"
#	define DO(__STR) return __STR;
#else
#	define DO(__STR) ;
#endif

#endif
%}

%%
\#include               DO(T_INCLUDE);
int						DO(T_INT);
char					DO(T_CHAR);
return					DO(T_RETURN);
for						DO(T_FOR);
do						DO(T_DO);
while					DO(T_WHILE);
\<						DO(T_LT);
\>						DO(T_GT);
==						DO(T_EQ);
!=						DO(T_NE);
=						DO(T_IS);
if						DO(T_IF);
else					DO(T_ELSE);
[.]						DO(T_DOT);
[0-9]*					DO(T_DEC);
0x[a-fA-F0-9]*			DO(T_HEX);
[a-zA-Z][a-zA-Z0-9]*    DO(T_WORD);
[a-zA-Z0-9\/.-]+		DO(T_FILE);
\(						DO(T_OBRACKET);
\)						DO(T_EBRACKET);
\"                      DO(T_QUOTE);
\{                      DO(T_OBRACE);
\}                      DO(T_EBRACE);
\&\&					DO(T_AND);
\|\|					DO(T_OR);
;                       DO(T_SEMICOLON);
,						DO(T_COMMA);
\n                      /* ignore EOL */;
[ \t]+                  /* ignore whitespace */;
%%