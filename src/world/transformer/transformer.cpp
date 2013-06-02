#include "transformer.h"
extern int strcmp(const char * str1, const char * str2);

Transformer::Transformer() :
        Messenger(MSG_INFO) {
    primal = Primal::getInstance();
    prev = NULL;
    isParam = false;
    nodeIds = 0;
    memset(&root, 0, sizeof(root));
    strcpy(root.name, "ROOT");
}

Transformer::Transformer(msg_severity_t msg_lvl) :
        Messenger(msg_lvl) {
    primal = Primal::getInstance(msg_lvl);
    prev = NULL;
    isParam = false;
    nodeIds = 0;
    memset(&root, 0, sizeof(root));
}

Transformer::~Transformer() {
}

int Transformer::transform(transfer_t *msg, size_t total) {
    if (msg) {
        size_t rd = 0;
        nucleotide_t *nucleotide = NULL;
        total /= sizeof(transfer_t);
        while (rd < total) {
            debug("%s | %s | %s", msg[rd].key, msg[rd].name, msg[rd].val);

            nucleotide = createNucleotide(msg[rd].key, msg[rd].name, msg[rd].val);
            if (nucleotide) {
                //if (!primal->search(nucleotide)) {
                    debug("Adding %s %s to database", typeToStr(nucleotide->nucleobase[0]),
                            subtypeToStr(nucleotide->nucleobase[0]));
                    primal->insert(nucleotide);
                    //nucleotideToStr(nucleotide);
                    prev = nucleotide;
                //} else {
                //    debug("Node already exists, removing recently allocated one");
                //    free(nucleotide);
                //}
            }
            rd++;
        }
        return 0;
    } else {
        error("No message to transform was supplied");
    }
    return 1;
}

nucleotide_t *Transformer::createNucleotide(const char *type, const char *name, const char *val) {
    nucleotide_t *nucleotide = (nucleotide_t *) malloc(sizeof(nucleotide_t));
    nucleotide_t *top = backtrace.size() ? backtrace.top() : NULL;
    if (nucleotide == NULL)
        return NULL;
    memset(nucleotide, 0, sizeof(nucleotide_t));
    nucleotide_u _subtype;
    _subtype.raw = 0;
    nucleotide_type_e _type = strToType(type, &_subtype);
    //debug3("type:%u %u",nucleotide->type, subtype.raw);
    strcpy(nucleotide->name, name);

    nucleotide->sibling = NULL;

	switch (_type) {
	case NUCLEO_TYPE_BASE:
		nucleotide->subvalues.base.isSet = true;
		switch (_subtype.base) {
		case NUCLEO_BASE_STRING:
			strcpy(nucleotide->subvalues.base.val.str, val);
			break;
		case NUCLEO_BASE_VOID:
			break;
		case NUCLEO_BASE_SIGNED:
			sscanf(val, "%d", &nucleotide->subvalues.base.val.sg);
			break;
		case NUCLEO_BASE_INT:
			sscanf(val, "%d", &nucleotide->subvalues.base.val.i);
			break;
		case NUCLEO_BASE_CHAR:
			sscanf(val, "%c", &nucleotide->subvalues.base.val.c);
			break;
		case NUCLEO_BASE_FLOAT:
			sscanf(val, "%f", &nucleotide->subvalues.base.val.f);
			break;
		case NUCLEO_BASE_DOUBLE:
			sscanf(val, "%lf", &nucleotide->subvalues.base.val.d);
			break;
		case NUCLEO_BASE_UNSIGNED:
			sscanf(val, "%u", &nucleotide->subvalues.base.val.usg);
			break;
		case NUCLEO_BASE_BOOL:
			if (strcmp("true", val))
				nucleotide->subvalues.base.val.b = true;
			else
				nucleotide->subvalues.base.val.b = false;
			break;
		case NUCLEO_BASE_UNDEFINED:
			nucleotide->subvalues.base.isSet = false;

		}
		break;
	case NUCLEO_TYPE_CONTROL:
		switch (_subtype.control) {
		case NUCLEO_CONTROL_FUNCTION:
			break;
		}
		break;
	case NUCLEO_TYPE_LOOP:
		switch (_subtype.loop) {
		case NUCLEO_LOOP_DO:
		case NUCLEO_LOOP_WHILE:
		case NUCLEO_LOOP_FOR:
			break;
		}
		break;
	case NUCLEO_TYPE_JUMP:
		switch (_subtype.jump) {
		case NUCLEO_JUMP_RETURN:
		case NUCLEO_JUMP_BREAK:
		case NUCLEO_JUMP_CONTINUE:
		case NUCLEO_JUMP_GOTO:
			nucleotide->subvalues.jump.jump = NULL; // search for it!
			break;
		}
		break;
	case NUCLEO_TYPE_SUPPORT:
		switch (_subtype.support) {
		case NUCLEO_SUPPORT_FUNC_PARAM:
			isParam = true;
			if (prev && top != prev) {
				primal->update(prev, NUCLEO_TYPE_CONTROL, NUCLEO_CONTROL_FUNCTION);
				debug3("push to stack %p", nucleotide);
				backtrace.push(prev);
				free(nucleotide);
				nucleotide = NULL;
			}
			break;
		case NUCLEO_SUPPORT_BLOCK_START:
			isParam = false;
			if (prev && top != prev) {
				primal->update(prev, NUCLEO_TYPE_CONTROL, NUCLEO_CONTROL_FUNCTION);
				debug3("push to stack %p", nucleotide);
				backtrace.push(prev);
				free(nucleotide);
				nucleotide = NULL;
			}
			break;
		case NUCLEO_SUPPORT_BLOCK_END:
			debug3("pop from stack %p", backtrace.top());
			backtrace.pop();
			free(nucleotide);
			nucleotide = NULL;
			break;
		case NUCLEO_SUPPORT_FILE_START:
			info("file %s added to stack ", nucleotide->name);
			backtrace.push(nucleotide);
			break;
		case NUCLEO_SUPPORT_FILE_END:
			info("file %s removed from stack ", top->name);
			backtrace.pop();
			free(nucleotide);
			nucleotide = NULL;
			break;
		}
		break;
	case NUCLEO_TYPE_ASSIGNS:
		break;
	case NUCLEO_TYPE_COMPARE:
		break;
	case NUCLEO_TYPE_OPERATOR:
		break;
	case NUCLEO_TYPE_UNDEFINED:
		break;
	}

    if (nucleotide) {
        if (!top)
            top = &root;
        nucleotide->parent = top;
        if (createNucleobase(nucleotide, _type, _subtype)) {
            if (isParam) {
                if (!top->subvalues.control.param_fst) {
                    top->subvalues.control.param_fst = top->subvalues.control.param_lst =
                            nucleotide;
                } else {
                    top->subvalues.control.param_lst->sibling = nucleotide;
                    top->subvalues.control.param_lst = nucleotide;
                }
            } else {
                if (!top->subvalues.control.child_fst) {
                    top->subvalues.control.child_fst = nucleotide;
                    top->subvalues.control.child_lst = nucleotide;
                } else {
                    top->subvalues.control.child_lst->sibling = nucleotide;
                    top->subvalues.control.child_lst = nucleotide;
                }
            }
            debug2("%s - parent %s", nucleotide->name, nucleotide->parent->name);
            return nucleotide;
        }
        free(nucleotide);
    }
    return NULL;
}

/* NUCLEOBASE FUNCTIONS */
bool Transformer::createNucleobase(nucleotide_t * nucleo, nucleotide_type_e type,
        nucleotide_u subtype) {
    if (nucleo->nucleobase)
        free(nucleo->nucleobase);
    nucleo->nucleobase = (nucleobase_u *) malloc(sizeof(nucleobase_u));
    memset(nucleo->nucleobase, 0, sizeof(nucleobase_u));

    // setting the value of nucleobase
    if (nucleo->parent && nucleo->parent->nucleobase_count < MAX_NUCLEOBASE_COUNT) {
        nucleo->nucleobase[0].raw = ((uint64_t) (((uint8_t) type << 5) + subtype.raw) << 56);
        nucleo->nucleobase[0].raw += nucleo->parent->nucleobase_count;
    } else {
        error("unable to create nucleobase, due to length of parents child list");
        return false;
    }
    nucleo->nucleobase_count = 1;

    debug3("Nucleobase encoded %x %x to type %lx", type, subtype.raw, nucleo->nucleobase->raw);
    return attachNucleobase(nucleo->parent, nucleo);
}

bool Transformer::attachNucleobase(nucleotide_t *parent, nucleotide_t *child) {
    size_t size = sizeof(uint64_t) * (parent->nucleobase_count + child->nucleobase_count);
    nucleobase_u *tmp = (nucleobase_u *) malloc(size);
    memset(tmp, 0, size);
    //copy old value of parent and destroy it
    memcpy(tmp, parent->nucleobase, sizeof(uint64_t) * parent->nucleobase_count);
    free(parent->nucleobase);
    parent->nucleobase = tmp;

    //attach value of child to parent
    tmp = &parent->nucleobase[parent->nucleobase_count];
    memcpy(tmp, child->nucleobase, sizeof(uint64_t) * child->nucleobase_count);

    parent->nucleobase_count += child->nucleobase_count;
    debug3("Nucleobase extended to count %lu", parent->nucleobase_count);
    return true;
}

void Transformer::printNucleobase(nucleotide_t *nucleo){
    for ( int i = 0; i < nucleo->nucleobase_count; i ++){
        info("%lx\t\t\t",nucleo->nucleobase[i].raw);
    }
}

const char *Transformer::subtypeToStr(nucleobase_u data) {
    nucleotide_type_e type = (nucleotide_type_e) (data.raw >> 61);
    nucleotide_u subtype;
    subtype.raw = ((data.raw >> 56) & 0x1f);
    //debug2("type %x subtype %x ", type & 0x7, subtype.raw & 0x1f);
    switch (type & 0x7) {
    case NUCLEO_TYPE_BASE:
        return baseToStr(subtype.base);
        break;
    case NUCLEO_TYPE_CONTROL:
        return controlToStr(subtype.control);
        break;
    case NUCLEO_TYPE_LOOP:
        return loopToStr(subtype.loop);
        break;
    case NUCLEO_TYPE_JUMP:
        return jumpToStr(subtype.jump);
        break;
    case NUCLEO_TYPE_SUPPORT:
        return supportToStr(subtype.support);
        break;
    case NUCLEO_TYPE_ASSIGNS:
        return assignsToStr(subtype.assigns);
        break;
    case NUCLEO_TYPE_COMPARE:
        return compareToStr(subtype.compare);
        break;
    case NUCLEO_TYPE_OPERATOR:
        return operatorToStr(subtype.oper);
        break;
    }
    return "UNDEFINED";
}

const char *Transformer::typeToStr(nucleobase_u type) {
    nucleotide_type_e _type = (nucleotide_type_e) (type.raw >> 61);
    switch (_type & 0xf) {
    case NUCLEO_TYPE_BASE:
        return "BASE";
        break;
    case NUCLEO_TYPE_CONTROL:
        return "CONTROL";
        break;
    case NUCLEO_TYPE_LOOP:
        return "LOOP";
        break;
    case NUCLEO_TYPE_JUMP:
        return "JUMP";
        break;
    case NUCLEO_TYPE_SUPPORT:
        return "SUPPORT";
        break;
    case NUCLEO_TYPE_ASSIGNS:
        return "ASSIGNS";
        break;
    case NUCLEO_TYPE_COMPARE:
        return "COMPARE";
        break;
    case NUCLEO_TYPE_OPERATOR:
        return "OPERATOR";
        break;
    }
    return "UNDEFINED";
}

const char *Transformer::baseToStr(nucleotide_base_e base) {
    switch (base & 0xf) {
    case NUCLEO_BASE_VOID:
        return "BASE";
        break;
    case NUCLEO_BASE_CHAR:
        return "CHAR";
        break;
    case NUCLEO_BASE_SHORT:
        return "SHORT";
        break;
    case NUCLEO_BASE_INT:
        return "INT";
        break;
    case NUCLEO_BASE_LONG:
        return "LONG";
        break;
    case NUCLEO_BASE_FLOAT:
        return "FLOAT";
        break;
    case NUCLEO_BASE_DOUBLE:
        return "DOUBLE";
        break;
    case NUCLEO_BASE_SIGNED:
        return "SIGNED";
        break;
    case NUCLEO_BASE_UNSIGNED:
        return "UNSIGNED";
        break;
    case NUCLEO_BASE_BOOL:
        return "BOOL";
        break;
    case NUCLEO_BASE_STRING:
        return "STRING";
        break;
    }
    return "UNDEFINED";
}

const char *Transformer::controlToStr(nucleotide_control_e control) {
    switch (control & 0xf) {
    case NUCLEO_CONTROL_FUNCTION:
        return "FUNCTION";
        break;
    case NUCLEO_CONTROL_IF:
        return "IF";
        break;
    case NUCLEO_CONTROL_ELIF:
        return "ELSE IF";
        break;
    case NUCLEO_CONTROL_ELSE:
        return "ELSE";
        break;
    case NUCLEO_CONTROL_SWITCH:
        return "SWITCH";
        break;
    }
    return "UNDEFINED";
}

const char *Transformer::loopToStr(nucleotide_loop_e loop) {
    switch (loop & 0x7) {
    case NUCLEO_LOOP_DO:
        return "DO WHILE";
        break;
    case NUCLEO_LOOP_WHILE:
        return "WHILE";
        break;
    case NUCLEO_LOOP_FOR:
        return "FOR";
        break;
    }
    return "UNDEFINED";
}

const char *Transformer::jumpToStr(nucleotide_jump_e jump) {
    switch (jump & 0x7) {
    case NUCLEO_JUMP_RETURN:
        return "RETURN";
        break;
    case NUCLEO_JUMP_BREAK:
        return "BREAK";
        break;
    case NUCLEO_JUMP_CONTINUE:
        return "CONTINUE";
        break;
    case NUCLEO_JUMP_GOTO:
        return "GOTO";
        break;
    }
    return "UNDEFINED";
}

const char *Transformer::supportToStr(nucleotide_support_e support) {
    switch (support & 0xf) {
    case NUCLEO_SUPPORT_BLOCK_START:
        return "BLOCK_START";
        break;
    case NUCLEO_SUPPORT_BLOCK_END:
        return "BLOCK_END";
        break;
    case NUCLEO_SUPPORT_FUNC_SRT:
        return "FUNC_SRT";
        break;
    case NUCLEO_SUPPORT_FUNC_END:
        return "FUNC_END";
        break;
    case NUCLEO_SUPPORT_FUNC_NAME:
        return "FUNC_NAME";
        break;
    case NUCLEO_SUPPORT_FUNC_PARAM:
        return "FUNC_PARM";
        break;
    case NUCLEO_SUPPORT_ARGS_START:
        return "ARGS_START";
        break;
    case NUCLEO_SUPPORT_ARGS_END:
        return "ARGS_END";
        break;
    case NUCLEO_SUPPORT_ARGUMENT:
        return "ARGUMENT";
        break;
    case NUCLEO_SUPPORT_FILE_START:
        return "FILE START";
        break;
    case NUCLEO_SUPPORT_FILE_END:
        return "FILE END";
        break;
    }
    return "UNDEFINED";
}

const char *Transformer::assignsToStr(nucleotide_assigns_e assings) {
    switch (assings & 0xf) {
    case NUCLEO_ASSIGNS_IS:
        return "ASSIGNS_IS";
        break;
    case NUCLEO_ASSIGNS_SUM:
        return "ASSIGNS_SUM";
        break;
    case NUCLEO_ASSIGNS_MIN:
        return "ASSIGNS_MIN";
        break;
    case NUCLEO_ASSIGNS_MULTIPLY:
        return "ASSIGNS_MULTIPLY";
        break;
    case NUCLEO_ASSIGNS_DEVIDE:
        return "ASSIGNS_DEVIDE";
        break;
    case NUCLEO_ASSIGNS_MOD:
        return "ASSIGNS_MOD";
        break;
    case NUCLEO_ASSIGNS_PLUS_ONE:
        return "ASSIGNS_PLUS_ONE";
        break;
    case NUCLEO_ASSIGNS_MINUS_ONE:
        return "ASSIGNS_MINUS_ONE";
        break;
    case NUCLEO_ASSIGNS_SHIFT_LEFT:
        return "ASSIGNS_SHIFT_LEFT";
        break;
    case NUCLEO_ASSIGNS_SHIFT_RIGHT:
        return "ASSIGNS_SHIFT_RIGHT";
        break;
    case NUCLEO_ASSIGNS_AND:
        return "ASSIGNS_AND";
        break;
    case NUCLEO_ASSIGNS_OR:
        return "ASSIGNS_OR";
        break;
    case NUCLEO_ASSIGNS_XOR:
        return "ASSIGNS_XOR";
        break;
    }
    return "UNDEFINED";
}

const char *Transformer::compareToStr(nucleotide_compare_e compare) {
    switch (compare & 0xf) {
    case NUCLEO_COMPARE_EQUAL:
        return "COMPARE_EQUAL";
        break;
    case NUCLEO_COMPARE_NOT_EQ:
        return "COMPARE_NOT_EQ";
        break;
    case NUCLEO_COMPARE_LESS:
        return "COMPARE_LESS";
        break;
    case NUCLEO_COMPARE_MORE:
        return "COMPARE_MORE";
        break;
    case NUCLEO_COMPARE_LESS_EQ:
        return "COMPARE_LESS_EQ";
        break;
    case NUCLEO_COMPARE_MORE_EQ:
        return "COMPARE_MORE_EQ";
        break;
    }
    return "UNDEFINED";
}
const char *Transformer::operatorToStr(nucleotide_operator_e oper) {
    switch (oper & 0xf) {
    case NUCLEO_OPERATOR_PLUS:
        return "OPERATOR_PLUS";
        break;
    case NUCLEO_OPERATOR_MINUS:
        return "OPERATOR_MINUS";
        break;
    case NUCLEO_OPERATOR_TIMES:
        return "OPERATOR_TIMES";
        break;
    case NUCLEO_OPERATOR_DEVIDE:
        return "OPERATOR_DEVIDE";
        break;
    case NUCLEO_OPERATOR_MOD:
        return "OPERATOR_MOD";
        break;
    case NUCLEO_OPERATOR_NOT:
        return "OPERATOR_NOT";
        break;
    case NUCLEO_OPERATOR_AND:
        return "OPERATOR_AND";
        break;
    case NUCLEO_OPERATOR_OR:
        return "OPERATOR_OR";
        break;
    case NUCLEO_OPERATOR_INVERT:
        return "OPERATOR_INVERT";
        break;
    case NUCLEO_OPERATOR_PTR:
        return "OPERATOR_PTR";
        break;
    }
    return "UNDEFINED";
}

nucleotide_type_e Transformer::strToType(const char *type, nucleotide_u *subtype) {
    subtype->base = strToBase(type);
    if (subtype->base != NUCLEO_BASE_UNDEFINED) {
        return NUCLEO_TYPE_BASE;
    }
    subtype->control = strToControl(type);
    if (subtype->control != NUCLEO_CONTROL_UNDEFINED) {
        return NUCLEO_TYPE_CONTROL;
    }
    subtype->loop = strToLoop(type);
    if (subtype->loop != NUCLEO_LOOP_UNDEFINED) {
        return NUCLEO_TYPE_LOOP;
    }
    subtype->jump = strToJump(type);
    if (subtype->jump != NUCLEO_JUMP_UNDEFINED) {
        return NUCLEO_TYPE_JUMP;
    }
    subtype->support = strToSupport(type);
    if (subtype->support != NUCLEO_SUPPORT_UNDEFINED) {
        return NUCLEO_TYPE_SUPPORT;
    }
    subtype->assigns = strToAssigns(type);
    if (subtype->assigns != NUCLEO_ASSIGNS_UNDEFINED) {
        return NUCLEO_TYPE_ASSIGNS;
    }
    subtype->compare = strToCompare(type);
    if (subtype->compare != NUCLEO_COMPARE_UNDEFINED) {
        return NUCLEO_TYPE_COMPARE;
    }
    subtype->oper = strToOperator(type);
    if (subtype->oper != NUCLEO_OPERATOR_UNDEFINED) {
        return NUCLEO_TYPE_OPERATOR;
    }
    return NUCLEO_TYPE_UNDEFINED;
}

nucleotide_base_e Transformer::strToBase(const char *base) {
    if (!strcmp(base, "void")) {
        return NUCLEO_BASE_VOID;
    } else if (!strcmp(base, "char")) {
        return NUCLEO_BASE_CHAR;
    } else if (!strcmp(base, "short")) {
        return NUCLEO_BASE_SHORT;
    } else if (!strcmp(base, "int")) {
        return NUCLEO_BASE_INT;
    } else if (!strcmp(base, "long")) {
        return NUCLEO_BASE_LONG;
    } else if (!strcmp(base, "float")) {
        return NUCLEO_BASE_FLOAT;
    } else if (!strcmp(base, "signed")) {
        return NUCLEO_BASE_DOUBLE;
    } else if (!strcmp(base, "double")) {
        return NUCLEO_BASE_SIGNED;
    } else if (!strcmp(base, "unsigned")) {
        return NUCLEO_BASE_UNSIGNED;
    } else if (!strcmp(base, "bool")) {
        return NUCLEO_BASE_BOOL;
    } else if (!strcmp(base, "string")) {
        return NUCLEO_BASE_STRING;
    }
    return NUCLEO_BASE_UNDEFINED;
}

nucleotide_control_e Transformer::strToControl(const char *control) {
    if (!strcmp(control, "func_srt")) {
        return NUCLEO_CONTROL_FUNCTION;
    } else if (!strcmp(control, "if")) {
        return NUCLEO_CONTROL_IF;
    } else if (!strcmp(control, "elif")) {
        return NUCLEO_CONTROL_ELIF;
    } else if (!strcmp(control, "else")) {
        return NUCLEO_CONTROL_ELSE;
    } else if (!strcmp(control, "switch")) {
        return NUCLEO_CONTROL_SWITCH;
    }
    return NUCLEO_CONTROL_UNDEFINED;
}

nucleotide_loop_e Transformer::strToLoop(const char *loop) {
    if (!strcmp(loop, "do_while")) {
        return NUCLEO_LOOP_DO;
    } else if (!strcmp(loop, "while")) {
        return NUCLEO_LOOP_WHILE;
    } else if (!strcmp(loop, "for")) {
        return NUCLEO_LOOP_FOR;
    }
    return NUCLEO_LOOP_UNDEFINED;
}

nucleotide_jump_e Transformer::strToJump(const char *jump) {
    if (!strcmp(jump, "return")) {
        return NUCLEO_JUMP_RETURN;
    } else if (!strcmp(jump, "break")) {
        return NUCLEO_JUMP_BREAK;
    } else if (!strcmp(jump, "continue")) {
        return NUCLEO_JUMP_CONTINUE;
    } else if (!strcmp(jump, "goto")) {
        return NUCLEO_JUMP_GOTO;
    }
    return NUCLEO_JUMP_UNDEFINED;
}

nucleotide_support_e Transformer::strToSupport(const char *support) {
    if (!strcmp(support, "block_st"))
        return NUCLEO_SUPPORT_BLOCK_START;
    else if (!strcmp(support, "block_en"))
        return NUCLEO_SUPPORT_BLOCK_END;
    else if (!strcmp(support, "fnc_srt"))
        return NUCLEO_SUPPORT_FUNC_SRT;
    else if (!strcmp(support, "fnc_end"))
        return NUCLEO_SUPPORT_FUNC_END;
    else if (!strcmp(support, "fnc_name"))
        return NUCLEO_SUPPORT_FUNC_NAME;
    else if (!strcmp(support, "fnc_parm"))
        return NUCLEO_SUPPORT_FUNC_PARAM;
    else if (!strcmp(support, "args_str"))
        return NUCLEO_SUPPORT_ARGS_START;
    else if (!strcmp(support, "args_end"))
        return NUCLEO_SUPPORT_ARGS_END;
    else if (!strcmp(support, "arg"))
        return NUCLEO_SUPPORT_ARGUMENT;
    else if (!strcmp(support, "file_str"))
        return NUCLEO_SUPPORT_FILE_START;
    else if (!strcmp(support, "file_end"))
        return NUCLEO_SUPPORT_FILE_END;
    return NUCLEO_SUPPORT_UNDEFINED;
}

nucleotide_assigns_e Transformer::strToAssigns(const char *assigns) {
    if (!strcmp(assigns, "="))
        return NUCLEO_ASSIGNS_IS;
    else if (!strcmp(assigns, "+="))
        return NUCLEO_ASSIGNS_SUM;
    else if (!strcmp(assigns, "-="))
        return NUCLEO_ASSIGNS_MIN;
    else if (!strcmp(assigns, "*="))
        return NUCLEO_ASSIGNS_MULTIPLY;
    else if (!strcmp(assigns, "/="))
        return NUCLEO_ASSIGNS_DEVIDE;
    else if (!strcmp(assigns, "%="))
        return NUCLEO_ASSIGNS_MOD;
    else if (!strcmp(assigns, "++"))
        return NUCLEO_ASSIGNS_PLUS_ONE;
    else if (!strcmp(assigns, "--"))
        return NUCLEO_ASSIGNS_MINUS_ONE;
    else if (!strcmp(assigns, "<<="))
        return NUCLEO_ASSIGNS_SHIFT_LEFT;
    else if (!strcmp(assigns, ">>="))
        return NUCLEO_ASSIGNS_SHIFT_RIGHT;
    else if (!strcmp(assigns, "&="))
        return NUCLEO_ASSIGNS_AND;
    else if (!strcmp(assigns, "|="))
        return NUCLEO_ASSIGNS_OR;
    else if (!strcmp(assigns, "^="))
        return NUCLEO_ASSIGNS_XOR;
    return NUCLEO_ASSIGNS_UNDEFINED;
}

nucleotide_compare_e Transformer::strToCompare(const char *compare) {
    if (!strcmp(compare, "=="))
        return NUCLEO_COMPARE_EQUAL;
    else if (!strcmp(compare, "!="))
        return NUCLEO_COMPARE_NOT_EQ;
    else if (!strcmp(compare, "<"))
        return NUCLEO_COMPARE_LESS;
    else if (!strcmp(compare, ">"))
        return NUCLEO_COMPARE_MORE;
    else if (!strcmp(compare, "<="))
        return NUCLEO_COMPARE_LESS_EQ;
    else if (!strcmp(compare, ">="))
        return NUCLEO_COMPARE_MORE_EQ;
    return NUCLEO_COMPARE_UNDEFINED;
}

nucleotide_operator_e Transformer::strToOperator(const char *oper) {
    if (!strcmp(oper, "+"))
        return NUCLEO_OPERATOR_PLUS;
    else if (!strcmp(oper, "-"))
        return NUCLEO_OPERATOR_MINUS;
    else if (!strcmp(oper, "*"))
        return NUCLEO_OPERATOR_TIMES;
    else if (!strcmp(oper, "/"))
        return NUCLEO_OPERATOR_DEVIDE;
    else if (!strcmp(oper, "%"))
        return NUCLEO_OPERATOR_MOD;
    else if (!strcmp(oper, "!"))
        return NUCLEO_OPERATOR_NOT;
    else if (!strcmp(oper, "&"))
        return NUCLEO_OPERATOR_AND;
    else if (!strcmp(oper, "|"))
        return NUCLEO_OPERATOR_OR;
    else if (!strcmp(oper, "~"))
        return NUCLEO_OPERATOR_INVERT;
    else if (!strcmp(oper, "ptr"))
        return NUCLEO_OPERATOR_PTR;
    return NUCLEO_OPERATOR_UNDEFINED;
}

void Transformer::printFiles(bool print) {
    nucleotide_t *it = root.subvalues.control.child_fst;
    while (it) {
        nucleotideToStr(it);
        if (print) {
            info("Name\t\tType\t\tSubtype\t\t\t\t");
            printChildren(it, 1);
        }
        it = it->sibling;
    }
}

void Transformer::printChildren(nucleotide_t *parent, size_t depth) {
    depth++;
    if (parent->nucleobase_count > 1) {
        nucleotide_t *it = parent->subvalues.control.child_fst;
        info("%lu", depth);
        while (it) {
            nucleotideToStr(it);
            printChildren(it, depth);
            it = it->sibling;
        }
    }
}

void Transformer::nucleotideToStr(nucleotide_t *nucleotide) {
    info("%s\t\t%s\t\t%s\t\t\t\t", nucleotide->name, typeToStr(nucleotide->nucleobase[0]),
            subtypeToStr(nucleotide->nucleobase[0]));
    printNucleobase(nucleotide);
}
