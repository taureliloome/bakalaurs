%{

extern "C"
{
    int yyparse(void);
    int yylex(void);  
    int yywrap()
    {
            return 1;
    }

}

#include <stdio.h>
#include <string.h>
#include "communication.h"
#include "transformer_if.h"

#define YYSTYPE char *
extern char *yytext;
#define YYDEBUG_LEXER_TEXT yytext
int yydebug;
 
void yyerror(const char *str)
{
        fprintf(stderr,"error: %s\n",str);
}

static Communicator *self = NULL;
TransformerIf transformerIf(MSG_DEBUG3);
int main()
{
    yyparse();
    
    self = Communicator::getInstance(false,"YACC");
    
    if ( !self->ConnectToServer("127.0.0.1","1337") ){
        delete self;
        return 0;
    }
	
    self->SendMessage(self->getServerConnPtr(), transformerIf.getBuffPtr(), transformerIf.getAndResetBuffLen(), 0);
    self->communicate();
    delete self;
    return 0;
}

%}


%token	IDENTIFIER I_CONSTANT F_CONSTANT STRING_LITERAL FUNC_NAME FUNC_NAME_BRACE SIZEOF INCLUDE
%token	PTR_OP INC_OP DEC_OP LEFT_OP RIGHT_OP LE_OP GE_OP EQ_OP NE_OP
%token	AND_OP OR_OP MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN ADD_ASSIGN
%token	SUB_ASSIGN LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN
%token	XOR_ASSIGN OR_ASSIGN
%token	TYPEDEF_NAME ENUMERATION_CONSTANT

%token	TYPEDEF EXTERN STATIC AUTO REGISTER INLINE
%token	CONST RESTRICT VOLATILE
%token	BOOL CHAR SHORT INT LONG SIGNED UNSIGNED FLOAT DOUBLE VOID
%token	COMPLEX IMAGINARY 
%token	STRUCT UNION ENUM ELLIPSIS

%token	CASE DEFAULT IF ELSE SWITCH WHILE DO FOR GOTO CONTINUE BREAK RETURN

%token	ALIGNAS ALIGNOF ATOMIC GENERIC NORETURN STATIC_ASSERT THREAD_LOCAL

%start translation_unit

%%
		
primary_expression
	: IDENTIFIER 
	| constant
	| string { transformerIf.addParam("string","",yytext, 76); }
	| '(' expression ')'
	| generic_selection
	;

constant
	: I_CONSTANT		/* includes character_constant */
	| F_CONSTANT
	| ENUMERATION_CONSTANT	/* after it has been defined as such */
	;

enumeration_constant		/* before it has been defined as such */
	: IDENTIFIER { transformerIf.addKey("enum",21); }
	;

string
	: STRING_LITERAL 
	| FUNC_NAME { transformerIf.addKey("fnc_name",22); }
	;

generic_selection
	: GENERIC '(' assignment_expression ',' generic_assoc_list ')'
	;

generic_assoc_list
	: generic_association
	| generic_assoc_list ',' generic_association
	;

generic_association
	: type_name ':' assignment_expression
	| DEFAULT ':' assignment_expression
	;

postfix_expression
	: primary_expression 
	| postfix_expression '[' expression ']'
	| postfix_expression '(' { transformerIf.addKey("fnc_srt",23); transformerIf.addToBuff(0); transformerIf.addKey("fnc_end",73); transformerIf.addToBuff(1); } ')' 
	| postfix_expression '('
		{ transformerIf.addKey("func_srt",24); transformerIf.addToBuff(2);}
		argument_expression_list ')'
	  	{ transformerIf.addKey("fnc_end",25); transformerIf.addToBuff(3); }
	| postfix_expression '.' IDENTIFIER
	| postfix_expression PTR_OP IDENTIFIER { transformerIf.addKey("ptr",26); }
	| postfix_expression INC_OP	{ transformerIf.addKey("++",27); }
	| postfix_expression DEC_OP { transformerIf.addKey("--",28); }
	| '(' type_name ')' '{' initializer_list '}'
	| '(' type_name ')' '{' initializer_list ',' '}'
	;

argument_expression_list
	: assignment_expression
	| argument_expression_list ',' assignment_expression
	;

unary_expression
	: postfix_expression
	| INC_OP unary_expression
	| DEC_OP unary_expression
	| unary_operator cast_expression
	| SIZEOF unary_expression
	| SIZEOF '(' type_name ')'
	| ALIGNOF '(' type_name ')'
	;

unary_operator
	: '&' { transformerIf.addKey("&",29); }
	| '*' { transformerIf.addKey("*",30); }
	| '+' { transformerIf.addKey("+",31); }
	| '-' { transformerIf.addKey("-",32); }
	| '~' { transformerIf.addKey("~",33); }
	| '!' { transformerIf.addKey("!",34); }
	;

cast_expression
	: unary_expression
	| '(' type_name ')' cast_expression
	;

multiplicative_expression
	: cast_expression
	| multiplicative_expression '*' cast_expression
	| multiplicative_expression '/' cast_expression
	| multiplicative_expression '%' cast_expression
	;

additive_expression
	: multiplicative_expression
	| additive_expression '+' multiplicative_expression
	| additive_expression '-' multiplicative_expression
	;

shift_expression
	: additive_expression
	| shift_expression LEFT_OP additive_expression
	| shift_expression RIGHT_OP additive_expression
	;

relational_expression
	: shift_expression
	| relational_expression '<' shift_expression { transformerIf.addKey("<",35); }
	| relational_expression '>' shift_expression { transformerIf.addKey(">",36); }
	| relational_expression LE_OP shift_expression
	| relational_expression GE_OP shift_expression
	;

equality_expression
	: relational_expression
	| equality_expression EQ_OP relational_expression
	| equality_expression NE_OP relational_expression
	;

and_expression
	: equality_expression
	| and_expression '&' equality_expression
	;

exclusive_or_expression
	: and_expression
	| exclusive_or_expression '^' and_expression
	;

inclusive_or_expression
	: exclusive_or_expression
	| inclusive_or_expression '|' exclusive_or_expression
	;

logical_and_expression
	: inclusive_or_expression
	| logical_and_expression AND_OP inclusive_or_expression
	;

logical_or_expression
	: logical_and_expression
	| logical_or_expression OR_OP logical_and_expression
	;

conditional_expression
	: logical_or_expression
	| logical_or_expression '?' expression ':' conditional_expression
	;

assignment_expression
	: conditional_expression
	| unary_expression assignment_operator assignment_expression
	;

assignment_operator
	: '=' {transformerIf.addKey("=",74); } 
	| MUL_ASSIGN {transformerIf.addKey("*=",37);}
	| DIV_ASSIGN {transformerIf.addKey("/=",38);}
	| MOD_ASSIGN {transformerIf.addKey("%=",39);}
	| ADD_ASSIGN {transformerIf.addKey("+=",40);}
	| SUB_ASSIGN {transformerIf.addKey("-=",41);}
	| LEFT_ASSIGN {transformerIf.addKey("<<=",42);}
	| RIGHT_ASSIGN {transformerIf.addKey(">>=",43);}
	| AND_ASSIGN {transformerIf.addKey("&=",44);}
	| XOR_ASSIGN {transformerIf.addKey("^=",45);}
	| OR_ASSIGN {transformerIf.addKey("|=",46);}
	;

expression
	: assignment_expression
	| expression ',' assignment_expression
	;

constant_expression
	: conditional_expression	/* with constraints */
	;

declaration
	: declaration_specifiers ';'
	| declaration_specifiers init_declarator_list ';' { transformerIf.clearBufs(); }
	| static_assert_declaration
	;

declaration_specifiers
	: storage_class_specifier declaration_specifiers
	| storage_class_specifier
	| type_specifier declaration_specifiers
	| type_specifier
	| type_qualifier declaration_specifiers
	| type_qualifier
	| function_specifier declaration_specifiers
	| function_specifier
	| alignment_specifier declaration_specifiers
	| alignment_specifier
	;

init_declarator_list
	: init_declarator { transformerIf.addToBuff(79, false); } 
	| init_declarator_list ',' init_declarator
	;

init_declarator
	: declarator '=' initializer 
	| declarator
	;

storage_class_specifier
	: TYPEDEF	/* identifiers must be flagged as TYPEDEF_NAME */
	| EXTERN
	| STATIC
	| THREAD_LOCAL
	| AUTO
	| REGISTER
	;

type_specifier
	: VOID			{ transformerIf.addKey("void",47); }
	| CHAR			{ transformerIf.addKey("char",48); }
	| SHORT			{ transformerIf.addKey("short",49); }
	| INT 			{ transformerIf.addKey("int",50); }
	| LONG 			{ transformerIf.addKey("long",51); }
	| FLOAT 		{ transformerIf.addKey("float",52); }
	| DOUBLE 		{ transformerIf.addKey("double",53); }
	| SIGNED 		{ transformerIf.addKey("signed",54); }
	| UNSIGNED 		{ transformerIf.addKey("unsigned",55); }
	| BOOL 			{ transformerIf.addKey("bool",56); }
	| COMPLEX
	| IMAGINARY	  	/* non-mandated extension */
	| atomic_type_specifier
	| struct_or_union_specifier
	| enum_specifier
	| TYPEDEF_NAME		/* after it has been defined as such */
	;

struct_or_union_specifier
	: struct_or_union '{' struct_declaration_list '}'
	| struct_or_union IDENTIFIER '{' struct_declaration_list '}'
	| struct_or_union IDENTIFIER
	;

struct_or_union
	: STRUCT
	| UNION
	;

struct_declaration_list
	: struct_declaration
	| struct_declaration_list struct_declaration
	;

struct_declaration
	: specifier_qualifier_list ';'	/* for anonymous struct/union */
	| specifier_qualifier_list struct_declarator_list ';'
	| static_assert_declaration
	;

specifier_qualifier_list
	: type_specifier specifier_qualifier_list
	| type_specifier
	| type_qualifier specifier_qualifier_list
	| type_qualifier
	;

struct_declarator_list
	: struct_declarator
	| struct_declarator_list ',' struct_declarator
	;

struct_declarator
	: ':' constant_expression
	| declarator ':' constant_expression
	| declarator
	;

enum_specifier
	: ENUM '{' enumerator_list '}'
	| ENUM '{' enumerator_list ',' '}'
	| ENUM IDENTIFIER '{' enumerator_list '}'
	| ENUM IDENTIFIER '{' enumerator_list ',' '}'
	| ENUM IDENTIFIER
	;

enumerator_list
	: enumerator
	| enumerator_list ',' enumerator
	;

enumerator	/* identifiers must be flagged as ENUMERATION_CONSTANT */
	: enumeration_constant '=' constant_expression
	| enumeration_constant
	;

atomic_type_specifier
	: ATOMIC '(' type_name ')'
	;

type_qualifier
	: CONST
	| RESTRICT
	| VOLATILE
	| ATOMIC
	;

function_specifier
	: INLINE
	| NORETURN
	;

alignment_specifier
	: ALIGNAS '(' type_name ')'
	| ALIGNAS '(' constant_expression ')'
	;

declarator
	: pointer direct_declarator
	| direct_declarator
	;

direct_declarator
	: IDENTIFIER { transformerIf.addName(yytext,57); } 
	| '(' declarator ')'
	| direct_declarator '[' ']'
	| direct_declarator '[' '*' ']'
	| direct_declarator '[' STATIC type_qualifier_list assignment_expression ']'
	| direct_declarator '[' STATIC assignment_expression ']'
	| direct_declarator '[' type_qualifier_list '*' ']'
	| direct_declarator '[' type_qualifier_list STATIC assignment_expression ']'
	| direct_declarator '[' type_qualifier_list assignment_expression ']'
	| direct_declarator '[' type_qualifier_list ']'
	| direct_declarator '[' assignment_expression ']'
	| direct_declarator '(' parameter_type_list ')'
	| direct_declarator '(' ')'
	| direct_declarator '(' identifier_list ')'
	;

pointer
	: '*' type_qualifier_list pointer
	| '*' type_qualifier_list
	| '*' pointer
	| '*'
	;

type_qualifier_list
	: type_qualifier
	| type_qualifier_list type_qualifier
	;


parameter_type_list
	: parameter_list ',' ELLIPSIS
	| parameter_list
	;

parameter_list
	: parameter_declaration
	| parameter_list ',' parameter_declaration
	;

parameter_declaration
	: declaration_specifiers declarator
	| declaration_specifiers abstract_declarator
	| declaration_specifiers
	;

identifier_list
	: IDENTIFIER
	| identifier_list ',' IDENTIFIER
	;

type_name
	: specifier_qualifier_list abstract_declarator
	| specifier_qualifier_list
	;

abstract_declarator
	: pointer direct_abstract_declarator
	| pointer
	| direct_abstract_declarator
	;

direct_abstract_declarator
	: '(' abstract_declarator ')'
	| '[' ']'
	| '[' '*' ']'
	| '[' STATIC type_qualifier_list assignment_expression ']'
	| '[' STATIC assignment_expression ']'
	| '[' type_qualifier_list STATIC assignment_expression ']'
	| '[' type_qualifier_list assignment_expression ']'
	| '[' type_qualifier_list ']'
	| '[' assignment_expression ']'
	| direct_abstract_declarator '[' ']'
	| direct_abstract_declarator '[' '*' ']'
	| direct_abstract_declarator '[' STATIC type_qualifier_list assignment_expression ']'
	| direct_abstract_declarator '[' STATIC assignment_expression ']'
	| direct_abstract_declarator '[' type_qualifier_list assignment_expression ']'
	| direct_abstract_declarator '[' type_qualifier_list STATIC assignment_expression ']'
	| direct_abstract_declarator '[' type_qualifier_list ']'
	| direct_abstract_declarator '[' assignment_expression ']'
	| '(' ')'
	| '(' parameter_type_list ')'
	| direct_abstract_declarator '(' ')'
	| direct_abstract_declarator '(' parameter_type_list ')'
	;

initializer
	: '{' initializer_list '}'
	| '{' initializer_list ',' '}'
	| { transformerIf.addVal(yytext,59); transformerIf.addToBuff(5, false); }  assignment_expression 
	;

initializer_list
	: designation initializer
	| initializer
	| initializer_list ',' designation initializer
	| initializer_list ',' initializer
	;

designation
	: designator_list '='
	;

designator_list
	: designator
	| designator_list designator
	;

designator
	: '[' constant_expression ']'
	| '.' IDENTIFIER
	;

static_assert_declaration
	: STATIC_ASSERT '(' constant_expression ',' STRING_LITERAL ')' ';'
	;

statement
	: labeled_statement
	| compound_statement
	| expression_statement
	| selection_statement
	| iteration_statement
	| jump_statement
	;

labeled_statement
	: IDENTIFIER ':' statement
	| CASE constant_expression ':' statement
	| DEFAULT ':' statement
	;

compound_statement
	: '{' { transformerIf.addKey("block_st",60); transformerIf.addToBuff(6); transformerIf.addKey("block_en",61); transformerIf.addToBuff(7); } '}' 
	| '{' { transformerIf.addKey("block_st",62); transformerIf.addToBuff(8); transformerIf.addName(yytext,62); } block_item_list { transformerIf.addKey("block_en",63); transformerIf.addToBuff(10); } '}'
	;

block_item_list
	: block_item
	| block_item_list block_item
	;

block_item
	: declaration
	| statement
	;

expression_statement
	: ';'
	| expression ';'
	;

selection_statement
	: IF '(' expression ')' statement ELSE statement
	| IF '(' expression ')' statement
	| SWITCH '(' expression ')' statement
	;

iteration_statement
	: WHILE '(' expression ')' statement { transformerIf.addKey("while",64); transformerIf.addToBuff(11); }
	| DO statement WHILE '(' expression ')' ';' { transformerIf.addKey("do_while",65); transformerIf.addToBuff(12); }
	| FOR '(' expression_statement expression_statement ')' statement { transformerIf.addKey("for",66); transformerIf.addToBuff(13); }
	| FOR '(' expression_statement expression_statement expression ')' statement  { transformerIf.addKey("for",67); transformerIf.addToBuff(14); }
	| FOR '(' declaration expression_statement ')' statement { transformerIf.addKey("for",68); transformerIf.addToBuff(15); }
	| FOR '(' declaration expression_statement expression ')' statement { transformerIf.addKey("for",69); transformerIf.addToBuff(16); }
	;

jump_statement
	: GOTO IDENTIFIER ';' { transformerIf.addKey("goto",70); transformerIf.addToBuff(17); }
	| CONTINUE ';' { transformerIf.addKey("continue",70); transformerIf.addToBuff(17); }
	| BREAK ';' { transformerIf.addKey("break",71); transformerIf.addToBuff(18); }
	| RETURN { transformerIf.addKey("return",72); }';' { transformerIf.addToBuff(19); }
	| RETURN { transformerIf.addParam("return",yytext,yytext,78); } expression ';'
	;

translation_unit
	: external_declaration
	| translation_unit external_declaration
	;

external_declaration
	: function_definition
	| declaration
	;
	
function_definition
	: declaration_specifiers declarator {transformerIf.addToBuff(4); }  declaration_list compound_statement 
	| declaration_specifiers declarator { transformerIf.addToBuff(75); } compound_statement 
	;

declaration_list
	: declaration
	| declaration_list declaration
	;
%%














