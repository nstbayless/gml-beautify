#include "ogm/ast/parse.h"

#include "Lexer.hpp"
#include "Parser.hpp"
#include "Production.hpp"

#include <string>
#include <cstring>

#define handle(x, T, base) if (T x = dynamic_cast<T>(base))
#define make_ast(n) (ogm_ast_t*) malloc( sizeof(ogm_ast_t) * n);

const char* ogm_ast_spec_string[] = 
{
    "",
    "with",
    "while",
    "do",
    "repeat",
    
    "continue",
    "break",
    "exit",
    "return",
    
    // accessors
    "",
    "@",
    "?",
    "#",
    "|",
    
    // operations
    "+",
    "-",
    "*",
    "/",
    "div",
    "mod",
    "<<",
    ">>",
    
    // comparisons
    "==",
    "!=",
    "<",
    "<=",
    ">",
    ">=",
    "<>",
    
    "&&",
    "and",
    "||",
    "or",
    "^^",
    "xor",
    "!",
    "not",
    "~",
    
    // assignments
    "=",
    "+=",
    "-=",
    "*=",
    "/=",
    "&=",
    "|=",
    "^=",
    "++",
    "++",
    "--",
    "--",
}

namespace
{    
    char* buffer(std::string s)
    {
        size_t l = s.length();
        char* c = (char*)malloc(l);
        memcpy(c, s.c_str(), l);
        return c;
    }
    
    void
    initialize_decor_from_production(
        ogm_ast_decor_t& outDecor,
        Production* production
    )
    {
        
    }
    
    ogm_ast_spec_t op_to_spec(std::string op)
    {
        ogm_ast_spec_t to_return = ogm_ast_spec_none;
        while (to_return < ogm_ast_spec_count)
        {
            if (op == ogm_ast_spec_string[to_return])
            {
                return to_return;
            }
            ++to_return;
        }
        return ogm_ast_spec_none;
    }
    
    void
    initialize_ast_from_production(
        ogm_ast_t& out,
        Production* production
    )
    {
        handle(p, PrExprParen*, production)
        {
            out.m_type = ogm_ast_t_exp;
            out.m_subtype = ogm_ast_t_exp;
            out.m_sub = make_ast(1);
            out.m_sub_c = 1;
            initialize_ast_from_production(*out.m_sub, p->content);
        }
        else handle(p, PrExpressionFn*, production)
        {
            out.m_type = ogm_ast_t_exp;
            out.m_subtype = ogm_ast_st_exp_fn;
            out.m_payload = buffer(p->identifier.value)
            size_t arg_count = p->args.size();
            out.m_sub = make_ast(arg_count);
            for (size_t i = 0; i < arg_count; i++)
            {
                initialize_ast_from_production(out.m_sub[i], p->args[i]);
            }
        }
        else handle(p, PrExprArithmetic*, production)
        {
            out.m_type = ogm.ogm_ast_t_exp;
            out.m_subtype = ogm.ogm_ast_st_exp_arithmetic;
            out.m_spec = op_to_spec(p->op.value);
            
            size_t i;
            if (p->rhs)
            {
                
                if (out.m_spec == ogm_ast_spec_op_unary_pre_plusplus ||
                    out.m_spec == ogm_ast_spec_op_unary_pre_minusminus)
                {
                    // set to be post_plusplus / post_minusminus
                    out.m_spec++;
                    
                    out.m_sub = make_ast(1);
                    out.m_sub_count = 1;
                    initialize_ast_from_production(*out.m_sub, p->rhs);
                    return;
                }
                else
                {
                    i = 2;
                }
            }
            
            out.m_sub = make_ast(i);
            out.m_sub_count = i;
            if (i == 1)
            {
                initialize_ast_from_production(*out.m_sub, p->lhs)
            }
            else
            {
                initialize_ast_from_production(out.m_sub[0], p->lhs)
                initialize_ast_from_production(out.m_sub[1], p->rhs)
            }
        }
        else handle(p, PrFinal*, production)
        {
            out.m_type = ogm_ast_t_exp;
            out.m_subtype = ogm_ast_st_exp_literal_primitive;
            auto* payload = (ogm_ast_literal_primitive_t*) malloc( sizeof(ogm_ast_literal_primitive_t) );
            switch (p->final->type)
            {
            case NUM:
                // hex number
                if (p->final->string.find("$") == 0)
                {
                    payload->type = ogm_ast_literal_t_hex;
                }
                else
                {
                    payload->type = ogm_ast_literal_t_dec;
                }
                break;
            case STR:
                payload->type = ogm_ast_literal_t_dec;
                break;
            }
            payload->value = buffer(p->final.value);
            out.m_payload = payload;
            out.m_sub_count = 0;
        }
        else handle(p, PrIdentifier*, production)
        {
            out.m_type = ogm_ast_t_exp;
            out.m_subtype = ogm_ast_st_exp_identifier;
            out.payload = ogm_ast_st_exp_identifier;
            out.m_sub_count = 0;
        }
        else handle(p, PrAccessorExpression*, production)
        {
            out.m_type = ogm_ast_t_exp;
            out.m_subtype = ogm_ast_st_exp_accessor;
            out.m_spec = op_to_spec(p->acc);
            out.m_sub_count = 1 + p->indices;
            out.m_sub = make_ast(out.m_sub_count);
            initialize_ast_from_production(out.m_sub[0], p->ds);
            for (size_t i = 1; i < out.m_sub_count; i++)
            {
                initialize_ast_from_production(out.m_sub[i], p->indices[i - 1]);
            }
        }
        else handle(p, PrEmptyStatement*, production)
        {
            out.m_type = ogm_ast_t_imp;
            out.m_subtype = ogm_ast_st_imp_empty;
            out.m_sub_count = 0;
        }
        else handle(p, PrStatementFn*, production)
        {
            // we allow expressions to be a type of statement,
            // so there is no need to distinguish between
            // functions as a statement and functions as an expression.
            initialize_ast_from_production(*out, p->fn);
        }
        else handle(p, PrVarDeclaration*, production)
        {
            out.m_type = ogm_ast_t_imp;
            out.m_subtype = ogm_ast_st_imp_var;
            
            auto* declaration = (ogm_ast_declaration_t*) malloc( sizeof(ogm_ast_declaration_t) );
            declaration.m_identifier_count = p->declarations.size();
            declaration.m_type = buffer(p->type);
            out.m_sub_count = declaration.m_identifier_count;
            out.m_sub = make_ast(declaration.m_identifier_count);
            for (size_t i = 0; i < declaration.m_identifier_count; i++)
            {
                PrVarDeclaration* subDeclaration = p->declarations[i];
                declaration.m_identifier[i] = subDeclaration->identifier.value;
                if (subDeclaration.definition)
                {
                    // empty definition
                    out.m_sub[i].m_type = ogm_ast_t_imp;
                    out.m_sub[i].m_subtype = ogm_ast_st_imp_empty;
                    out.m_sub[i].m_sub_count = 0;
                }
                else
                {
                    // proper definition
                    initialize_ast_from_production(out.m_sub[i], subDeclaration->definition);
                }
            }
        }
        else handle(p, PrBody*, production)
        {
            out.m_type = ogm_ast_t_imp;
            out.m_subtype = ogm_ast_st_imp_body;
            if (p->is_root)
            {
                out.m_spec = ogm_ast_spec_body_braceless;
            }
            else
            {
                out.m_spec = ogm_ast_spec_body_braced;
            }
            size_t count = productions.size();
            out.m_sub_count = count;
            out.m_sub = make_ast(count);
            for (size_t i = 0; i < count; i++)
            {
                initialize_ast_from_production(out.m_sub[i], p->productions[i]);
            }
        }
        else handle(p, PrStatementIf*, production)
        {
            out.m_type = ogm_ast_t_imp;
            out.m_subtype = ogm_ast_st_imp_if;
            size_t i;
            if (p->otherwise)
            {
                i = 3;
            }
            else
            {
                i = 2;
            }
            out.m_sub_count = i;
            out.m_sub = make_ast(i);
            initialize_ast_from_production(out.m_sub[0], p->condition);
            if (i == 2)
            {
                initialize_ast_from_production(out.m_sub[1], p->result);
            }
            else
            {
                initialize_ast_from_production(out.m_sub[1], p->result);
                initialize_ast_from_production(out.m_sub[2], p->otherwise);
            }
        }
        else handle(p, PrFor*, production)
        {
            out.m_type = ogm_ast_t_imp;
            out.m_subtype = ogm_ast_st_imp_for;
            out.m_sub_count = 4;
            out.m_sub = make_ast(4);
            initialize_ast_from_production(out.m_sub[0], p->init);
            initialize_ast_from_production(out.m_sub[1], p->condition);
            initialize_ast_from_production(out.m_sub[2], p->second);
            initialize_ast_from_production(out.m_sub[3], p->first);
        }
        else handle(p, PrWhile*, production)
        {
            out.m_type = ogm_ast_t_imp;
            out.m_subtype = ogm_ast_st_imp_loop;
            out.m_spec = ogm_ast_spec_loop_while;
            out.m_sub_count = 2;
            out.m_sub = make_ast(2);
            initialize_ast_from_production(out.m_sub[0], p->condition);
            initialize_ast_from_production(out.m_sub[1], p->event);
        }
        else handle(p, PrRepeat*, production)
        {
            out.m_type = ogm_ast_t_imp;
            out.m_subtype = ogm_ast_st_imp_loop;
            out.m_spec = ogm_ast_spec_loop_repeat;
            out.m_sub_count = 2;
            out.m_sub = make_ast(2);
            initialize_ast_from_production(out.m_sub[0], p->count);
            initialize_ast_from_production(out.m_sub[1], p->event);
        }
        else handle(p, PrDo*, production)
        {
            out.m_type = ogm_ast_t_imp;
            out.m_subtype = ogm_ast_st_imp_loop;
            out.m_spec = ogm_ast_spec_loop_do;
            out.m_sub_count = 2;
            out.m_sub = make_ast(2);
            initialize_ast_from_production(out.m_sub[0], p->condition);
            initialize_ast_from_production(out.m_sub[1], p->event);
        }
        else handle(p, PrWith*, production)
        {
            out.m_type = ogm_ast_t_imp;
            out.m_subtype = ogm_ast_st_imp_loop;
            out.m_spec = ogm_ast_spec_loop_with;
            out.m_sub_count = 2;
            out.m_sub = make_ast(2);
            initialize_ast_from_production(out.m_sub[0], p->objid);
            initialize_ast_from_production(out.m_sub[1], p->event);
        }
        else handle(p, PrSwitch*, production)
        {
            out.m_type = ogm_ast_t_imp;
            out.m_subtype = ogm_ast_st_imp_switch;
            out.m_sub_count = 1 + (p->cases.size() << 1);
            out.m_sub = make_ast(out.m_sub_count);
            initialize_ast_from_production(out.m_sub[0], p->condition);
            for (size_t i = 0; i < p->cases.size(); i++)
            {
                PrCase* pCase = p->cases[i];
                initialize_ast_from_production(out.m_sub[1 + (i << 1)], pCase->value);
                size_t body_index = (1 + i) << 1;
                out.m_sub[body_index].m_type = ogm_ast_t_imp;
                out.m_sub[body_index].m_subtype = ogm_ast_st_body;
                out.m_sub[body_index].m_spec = ogm_ast_spec_body_braceless;
                size_t sub_count = p->productions.size();
                out.m_sub[body_index].m_sub_count = sub_count;
                for (size_t j = 0; j < sub_count; j++)
                {
                    initialize_ast_from_production(out.m_sub[body_index].m_sub[j], pCase->productions[j]);
                }
            }
        }
        else handle(p, PrControl*, production)
        {
            out.m_type = ogm_ast_t_imp;
            out.m_subtype = ogm_ast_st_imp_control;
            out.m_spec = op_to_spec(p->kw);
            if (p->val)
            {
                out.m_sub_count = 1;
                out.m_sub = make_ast(1);
                initialize_ast_from_production(out.m_sub, p->val);
            }
            else
            {
                out.m_sub_count = 0;
            }
        }
    }
}

bool ogm_ast_parse(
    char* code,
    ogm_ast_t* outTree,
    ogm_ast_decor_t* outDecorTree
)
{
    Parser p(code);
    PrBody* body = p.parse();
    
    initialize_ast_from_production(*body, outTree, body);
    if (outDecorTree)
    {
        initialize_ast_from_production(*outDecorTree, body)
    }
    
    delete body;
}

// free AST
void ogm_ast_free(
    ogm_ast_t* tree
)
{
    free tree;
}

// free decor tree
void ogm_ast_decor_free(
    ogm_ast_decor_t* decor
)
{
    free decor;
}