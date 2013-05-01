%{
#include <stdio.h>
#include <string.h>
#define YYSTYPE char *
extern char *yytext;
#define YYDEBUG_LEXER_TEXT yytext
int yydebug;
 
void yyerror(const char *str)
{
        fprintf(stderr,"error: %s\n",str);
}
 
int yywrap()
{
        return 1;
} 
  
main()
{
		yydebug= 1;
        yyparse();
} 

%}

%token T_INCLUDE 
%token T_INT T_CHAR 
%token T_RETURN T_QUOTE
%token T_WORD T_DEC T_HEX; 
%token T_EQ T_NE T_LT T_GT T_IS
%token T_OBRACKET T_EBRACKET T_OBRACE T_EBRACE 
%token T_SEMICOLON T_COMMA T_DOT T_FILE
%token T_FOR T_WHILE T_DO 
%token T_AND T_OR
%token T_ELSE T_IF

%%
commands: 	| commands command;
command: 	function | function_call | variable | fork | cycle | include | return | T_SEMICOLON{ printf(";\n"); };
variable:	var_def | var_use;
block:		T_OBRACE commands T_EBRACE | T_SEMICOLON{ printf(";\n"); };
cycle:		for | while | do_while;
fork:		if | if else_if | if else_if else;
compare:	
			T_EQ { printf("==");}
			| 
			T_LT { printf("<");}
			|
			T_GT { printf(">");}
			|
			T_NE { printf("!=");} 
			;
return:
			T_RETURN variables T_SEMICOLON
			{
				printf("return\n");
			}
			;
type:
			T_CHAR { printf("char "); }
			| 
			T_INT { printf("int "); }
			;
function:
		type T_WORD T_OBRACKET T_EBRACKET block
		{
			printf("function\n");
		}
function_call:
		T_WORD T_OBRACKET variables T_EBRACKET T_SEMICOLON
		{
			printf("function call\n");
		}

statment:
		T_OBRACKET variable compare variable T_EBRACKET
		|
		T_OBRACKET variable T_EBRACKET;
		
include:
		T_INCLUDE T_QUOTE T_FILE T_QUOTE
		{
			printf("lib include\n");
		}
		|
		T_INCLUDE T_LT T_FILE T_GT
		{
			printf("local include\n");
		}
		;
		
/* variables */
var_def:
		type T_WORD
		{
			printf("defintion of variable\n");
		}
		;
var_use:	
		T_WORD
		{
			printf("variable ");
		}
		;
variables:
		variables T_COMMA var_use
		|
		variables T_COMMA T_QUOTE T_WORD T_QUOTE
		{
			printf("char str ");
		}
		|
		T_HEX { printf("hex "); }
		|
		T_DEC { printf("dec "); }
		|
		var_use
		|
		T_COMMA T_QUOTE T_WORD T_QUOTE
		{
			printf("char str ");
		}
		;
/* fork */
if:
		T_IF statment block
		{
			printf("if\n");
		}
		;
else_if:
		T_ELSE T_IF statment block
		{
			printf("else if\n");
		}
		;
else:
		T_ELSE block 
		{
			printf("else\n");
		}

/* cycles */
for:
		T_FOR T_OBRACKET T_WORD T_SEMICOLON statment T_SEMICOLON T_WORD T_EBRACKET block
		{
			printf("for\n");
		};
while: 
		T_WHILE statment block

do_while: 
		T_DO statment block T_WHILE T_SEMICOLON
		{
			printf("do_while\n");
		};
%%















