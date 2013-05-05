O   [0-7]
D   [0-9]
NZ  [1-9]
L   [a-zA-Z_]
A   [a-zA-Z_0-9]
H   [a-fA-F0-9]
HP  (0[xX])
E   ([Ee][+-]?{D}+)
P   ([Pp][+-]?{D}+)
FS  (f|F|l|L)
IS  (((u|U)(l|L|ll|LL)?)|((l|L|ll|LL)(u|U)?))
CP  (u|U|L)
SP  (u8|u|U|L)
ES  (\\(['"\?\\abfnrtv]|[0-7]{1,3}|x[a-fA-F0-9]+))
WS  [ \t\v\n\f]

%{
#include <stdio.h>

#ifdef LEX
#	define TYPEDEF_NAME 0
#	define ENUMERATION_CONSTANT 1
#	define IDENTIFIER 0
#	define RET(__STR) printf("%s ", #__STR);
#	define yyerror(__STR) RET(__STR)
#	define sym_type(identifier) IDENTIFIER /* with no symbol table, fake it */
#else
#   ifdef YACC
#	    include "y.tab.h"
	    extern int yyerror(const char *);  /* prints grammar violation message */
	    extern int sym_type(const char *);  /* returns type from symbol table */
#	    define sym_type(identifier) IDENTIFIER /* with no symbol table, fake it */
#	    define RET(__STR) do { /* printf("%s",yytext); */ return __STR; } while(0)
#   else
#	    define RET(__STR) ;
#endif

#endif
static void comment(void);
static int determine(int in);
static int check_type(void);
/*{L}{A}*"("					{ RET(FUNC_NAME_BRACE); }*/
%}

%%
"/*"                    	{ comment(); }
"//".*                  	{ /* consume //-comment */ }
"#include <"{A}*"."{A}*">"	{ /* ignore */ } 
"#include \""{A}*"."{A}*"\"" { /* ignore */ } 
"auto"						{ RET(AUTO); }
"break"						{ RET(BREAK); }
"case"						{ RET(CASE); }

"char"						{ RET(CHAR); }
"int"						{ RET(INT); }
"short"						{ RET(SHORT); }
"long"						{ RET(LONG); }
"void"						{ RET(VOID); }

"signed"					{ RET(SIGNED); }
"unsigned"					{ RET(UNSIGNED); }

"const"						{ RET(CONST); }
"continue"					{ RET(CONTINUE); }
"default"					{ RET(DEFAULT); }
"do"						{ RET(DO); }
"double"					{ RET(DOUBLE); }
"else"						{ RET(ELSE); }
"enum"						{ RET(ENUM); }
"extern"					{ RET(EXTERN); }
"float"						{ RET(FLOAT); }
"for"						{ RET(FOR); }
"goto"						{ RET(GOTO); }
"if"						{ RET(IF); }
"inline"					{ RET(INLINE); }
"register"					{ RET(REGISTER); }
"restrict"					{ RET(RESTRICT); }
"return"					{ RET(RETURN); }
"sizeof"					{ RET(SIZEOF); }
"static"					{ RET(STATIC); }
"struct"					{ RET(STRUCT); }
"switch"					{ RET(SWITCH); }
"typedef"					{ RET(TYPEDEF); }
"union"						{ RET(UNION); }
"volatile"					{ RET(VOLATILE); }
"while"						{ RET(WHILE); }
"_Alignas"                  { RET(ALIGNAS); }
"_Alignof"                  { RET(ALIGNOF); }
"_Atomic"                   { RET(ATOMIC); }
"_Bool"                     { RET(BOOL); }
"_Complex"                  { RET(COMPLEX); }
"_Generic"                  { RET(GENERIC); }
"_Imaginary"                { RET(IMAGINARY); }
"_Noreturn"                 { RET(NORETURN); }
"_Static_assert"            { RET(STATIC_ASSERT); }
"_Thread_local"             { RET(THREAD_LOCAL); }
"__func__"                  { RET(FUNC_NAME); }
{L}{A}*						{ check_type(); RET(IDENTIFIER); }

{HP}{H}+{IS}?				{ RET(I_CONSTANT); }
{NZ}{D}*{IS}?				{ RET(I_CONSTANT); }
"0"{O}*{IS}?				{ RET(I_CONSTANT); }
{CP}?"'"([^'\\\n]|{ES})+"'"	{ RET(I_CONSTANT); }

{D}+{E}{FS}?				{ RET(F_CONSTANT); }
{D}*"."{D}+{E}?{FS}?		{ RET(F_CONSTANT); }
{D}+"."{E}?{FS}?			{ RET(F_CONSTANT); }
{HP}{H}+{P}{FS}?			{ RET(F_CONSTANT); }
{HP}{H}*"."{H}+{P}{FS}?		{ RET(F_CONSTANT); }
{HP}{H}+"."{P}{FS}?			{ RET(F_CONSTANT); }

({SP}?\"([^"\\\n]|{ES})*\"{WS}*)+	{ RET(STRING_LITERAL); }

"..."						{ RET(ELLIPSIS); }
">>="						{ RET(RIGHT_ASSIGN); }
"<<="						{ RET(LEFT_ASSIGN); }
"+="						{ RET(ADD_ASSIGN); }
"-="						{ RET(SUB_ASSIGN); }
"*="						{ RET(MUL_ASSIGN); }
"/="						{ RET(DIV_ASSIGN); }
"%="						{ RET(MOD_ASSIGN); }
"&="						{ RET(AND_ASSIGN); }
"^="						{ RET(XOR_ASSIGN); }
"|="						{ RET(OR_ASSIGN); }
">>"						{ RET(RIGHT_OP); }
"<<"						{ RET(LEFT_OP); }
"++"						{ RET(INC_OP); }
"--"						{ RET(DEC_OP); }
"->"						{ RET(PTR_OP); }
"&&"						{ RET(AND_OP); }
"||"						{ RET(OR_OP); }
"<="						{ RET(LE_OP); }
">="						{ RET(GE_OP); }
"=="						{ RET(EQ_OP); }
"!="						{ RET(NE_OP); }
";"							{ RET(';'); }
("{"|"<%")					{ RET('{'); }
("}"|"%>")					{ RET('}'); }
","							{ RET(','); }
":"							{ RET(':'); }
"="							{ RET('='); }
"("							{ RET('('); }
")"							{ RET(')'); }
("["|"<:")					{ RET('['); }
("]"|":>")					{ RET(']'); }
"."							{ RET('.'); }
"&"							{ RET('&'); }
"!"							{ RET('!'); }
"~"							{ RET('~'); }
"-"							{ RET('-'); }
"+"							{ RET('+'); }
"*"							{ RET('*'); }
"/"							{ RET('/'); }
"%"							{ RET('%'); }
"<"							{ RET('<'); }
">"							{ RET('>'); }
"^"							{ RET('^'); }
"|"							{ RET('|'); }
"?"							{ RET('?'); }
{WS}						{ /* whitespace separates tokens */ }
.							{ /* discard bad characters */ }

%%

int yywrap(void)        /* called at end of input */
{
    return 1;           /* terminate now */
}

static void comment(void)
{
    int c;

    while ((c = input()) != 0)
    {
        if (c == '*')
        {
            while ((c = input()) == '*')
                ;

            if (c == '/')
                return;

            if (c == 0)
                break;
        }
    }
    yyerror("unterminated comment");
}

static int check_type(void)
{
    switch (sym_type(yytext))
    {
    case TYPEDEF_NAME:                /* previously defined */
        return TYPEDEF_NAME;
    case ENUMERATION_CONSTANT:        /* previously defined */
        return ENUMERATION_CONSTANT;
    default:                          /* includes undefined */
        return IDENTIFIER;
    }
}