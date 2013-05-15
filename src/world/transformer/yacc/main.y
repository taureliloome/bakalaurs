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

#include "communication.h"
#include <stdio.h>
#include <string.h>

#define YYSTYPE char *
extern char *yytext;
#define YYDEBUG_LEXER_TEXT yytext
int yydebug;

static const  size_t MSG_BUFFER_SIZE = 1024;
static char buff[MSG_BUFFER_SIZE] = { '\0' };
static int buff_len = 0; 
 
void yyerror(const char *str)
{
        fprintf(stderr,"error: %s\n",str);
}

static Communicator *self = NULL;
int main()
{
    yyparse();
    
    self = Communicator::getInstance(false,"YACC");
    /*
    FILE *fd = fopen("./server.out", "w+");
    if ( fd )
        setOutputType(fd);
    */
    if ( !self->ConnectToServer("127.0.0.1","1337") ){
        delete self;
        return 0;
    }

    char buf[] = "ping\n";
    self->SendMessage(self->getServerConnPtr(), buff, buff_len, 0);
    self->communicate();
    delete self;
    return 0;
}

void send(const char *str, int endline)
{
	if (str && strlen(str) != 0) {
		size_t len = strlen(str);
		if ( len+buff_len > MSG_BUFFER_SIZE ){
			fprintf(stderr,"Full buffer\n");
			return;
		}
		char *a = &buff[buff_len];
		buff_len += len;
		sprintf(a, "%s", str);
	}
	if ( endline == 1 && buff_len + 1 < MSG_BUFFER_SIZE){
		buff[buff_len++] = '\n';
	}
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
	| string
	| '(' expression ')'
	| generic_selection
	;

constant
	: I_CONSTANT		/* includes character_constant */
	| F_CONSTANT
	| ENUMERATION_CONSTANT	/* after it has been defined as such */
	;

enumeration_constant		/* before it has been defined as such */
	: IDENTIFIER { send("ENUM CONST ",0); }
	;

string
	: STRING_LITERAL { send("STRING ",0); }
	| FUNC_NAME { send("FUNC_NAME",0); }
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
	| postfix_expression '(' { send("func_call ", 1);} ')' 
	| postfix_expression '(' { send("func_call ", 0);} argument_expression_list ')' { send("", 1);}
	| postfix_expression '.' IDENTIFIER
	| postfix_expression PTR_OP IDENTIFIER { send("PTR",0); }
	| postfix_expression INC_OP	{ send("++",0); }
	| postfix_expression DEC_OP { send("--",0); }
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
	: '&' { send("&",0); }
	| '*' { send("*",0); }
	| '+' { send("+",0); }
	| '-' { send("-",0); }
	| '~' { send("~",0); }
	| '!' { send("!",0); }
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
	| relational_expression '<' shift_expression { send("<",0); }
	| relational_expression '>' shift_expression { send(">",0); }
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
	: conditional_expression {send("",0);}
	| unary_expression assignment_operator assignment_expression
	;

assignment_operator
	: '=' {send("=",0);}
	| MUL_ASSIGN {send("*=",0);}
	| DIV_ASSIGN {send("/=",0);}
	| MOD_ASSIGN {send("%=",0);}
	| ADD_ASSIGN {send("+=",0);}
	| SUB_ASSIGN {send("-=",0);}
	| LEFT_ASSIGN {send("<<=",0);}
	| RIGHT_ASSIGN {send(">>=",0);}
	| AND_ASSIGN {send("&=",0);}
	| XOR_ASSIGN {send("^=",0);}
	| OR_ASSIGN {send("|=",0);}
	;

expression
	: assignment_expression
	| expression ',' assignment_expression
	;

constant_expression
	: conditional_expression	/* with constraints */
	;

declaration
	: declaration_specifiers ';' { send("",1); }
	| declaration_specifiers init_declarator_list ';' { send("",1); }
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
	: init_declarator
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
	: VOID			{ send("void ",0); }
	| CHAR			{ send("char ",0); }
	| SHORT			{ send("short ",0); }
	| INT 			{ send("int ",0); }
	| LONG 			{ send("long ",0); }
	| FLOAT 		{ send("float ",0); }
	| DOUBLE 		{ send("double ",0); }
	| SIGNED 		{ send("signed ",0); }
	| UNSIGNED 		{ send("unsigned ",0); }
	| BOOL 			{ send("bool ",0); }
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
	: IDENTIFIER
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
	: IDENTIFIER { printf("identifier_list "); }
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
	| assignment_expression
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
	: '{' { send("oBlock",1); send("eBlock",1);} '}' 
	| '{' { send("oBlock",1);} block_item_list { send("eBlock",1); } '}'
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
	: WHILE '(' expression ')' statement { send("WHILE",1); }
	| DO statement WHILE '(' expression ')' ';' { send("DO-WHILE",1); }
	| FOR '(' expression_statement expression_statement ')' statement { send("FOR",1); }
	| FOR '(' expression_statement expression_statement expression ')' statement  { send("FOR",1); }
	| FOR '(' declaration expression_statement ')' statement { send("FOR",1); }
	| FOR '(' declaration expression_statement expression ')' statement { send("FOR",1); }
	;

jump_statement
	: GOTO IDENTIFIER ';'
	| CONTINUE ';' { send("CONTINUE",1); }
	| BREAK ';' { send("BREAK",1); }
	| RETURN { send("RETURN",1); }';' { send("RETURN",1); }
	| RETURN { send("RETURN",0); } expression ';' { send("",1); } 
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
	: declaration_specifiers declarator { send("func_decl ",1); } declaration_list compound_statement
	| declaration_specifiers declarator { send("func_decl ",1); } compound_statement 
	;

declaration_list
	: declaration
	| declaration_list declaration
	;
%%














