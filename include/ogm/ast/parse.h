

#ifndef LIBOGMLAST_PARSE_H
#define LIBOGMLAST_PARSE_H

typedef enum ogm_ast_type
{
    //! expression
    ogm_ast_t_exp, 
    //! imperative
    ogm_ast_t_imp
} ogm_ast_type_t;

typedef enum ogm_ast_subtype
{
    //// expressions ////
    
    // literal                                      x = <4>;, s = <"hello">;
    ogm_ast_st_exp_literal_primitive,
    // identifier                                   <x> = 4;
    ogm_ast_st_exp_identifier,
    // accessor expression                          <arr[3]> = 4;, <map[? 2]> = x;
    ogm_ast_st_exp_accessor,
    // parentheses                                  b = <3 * (a + 2)>
    ogm_ast_st_exp_paren,
    // arithmetic                                   x = <3 + y>;, <z != 2>
    ogm_ast_st_exp_arithmetic,
    // function                                     y = <fn()>;
    ogm_ast_st_exp_fn,
    // read_possessive                              <id.x> = 0;
    ogm_ast_st_exp_possessive,
    // assignment as an expression                  z = <i++>;
    ogm_ast_st_exp_assignment,
    
    //// statements ////
    
    // empty statement                              <;>
    ogm_ast_st_imp_empty,
    // body                                         <{;;;}>
    ogm_ast_st_imp_body,
    // assignment                                   <x = 4;>, <++i;>
    ogm_ast_st_imp_assignment,
    // function call                                <trace(x);>
    ogm_ast_st_imp_fn,
    // (local) var statement  .                     <var x, y = 4;>
    ogm_ast_st_imp_var,
    // if statement                                 <if (x) { } else { }>
    ogm_ast_st_imp_if,
    // for loop                                     <for (;;) { }>
    ogm_ast_st_imp_for,
    // all other loops                              <with (x) { }>, <do { ...} until ()>
    ogm_ast_st_imp_loop,
    // switch                                       <switch (x) { case 1: default: }>
    ogm_ast_st_imp_switch,
    // control                                      <break;>, <return;>
    ogm_ast_st_imp_control
    
} ogm_ast_subtype_t;

char* 

// specification of the above type (optional)
typedef enum ogm_ast_spec
{
    // not all subtypes have specifications
    ogm_ast_spec_none,
    
    //// loop types ////
    ogm_ast_spec_loop_with,
    ogm_ast_spec_loop_while,
    ogm_ast_spec_loop_do_until,
    ogm_ast_spec_loop_repeat,
    
    //// control flow keywords ////
    ogm_ast_spec_control_continue,
    ogm_ast_spec_control_break,
    ogm_ast_spec_control_exit,
    ogm_ast_spec_control_return,
    
    //// accessor types ////
    
    // basic array usage                            a[0]
    ogm_ast_spec_acc_none,
    // in-place array access                        a[@ 0]
    ogm_ast_spec_acc_array,
    //                                              a[? 0]
    ogm_ast_spec_acc_map,
    //                                              a[# 0]
    ogm_ast_spec_acc_grid,
    //                                              a[| 0]
    ogm_ast_spec_acc_list,
    
    // arithmetic operators
    ogm_ast_spec_op_plus,
    ogm_ast_spec_op_minus,
    ogm_ast_spec_op_times,
    ogm_ast_spec_op_divide,
    ogm_ast_spec_op_integer_division_kw,
    ogm_ast_spec_op_mod_kw,
    
    ogm_ast_spec_op_leftshift,
    ogm_ast_spec_op_rightshift,
    
    // comparisons
    ogm_ast_spec_op_eqeq,
    ogm_ast_spec_op_ne,
    ogm_ast_spec_op_lt,
    ogm_ast_spec_op_lte,
    ogm_ast_spec_op_gt,
    ogm_ast_spec_op_gte,
    ogm_ast_spec_op_ltgt,
    
    // (binary) boolean operators
    ogm_ast_spec_op_boolean_and,
    ogm_ast_spec_op_boolean_and_kw,
    ogm_ast_spec_op_boolean_or,
    ogm_ast_spec_op_boolean_or_kw,
    ogm_ast_spec_op_boolean_xor,
    ogm_ast_spec_op_boolean_xor_kw,
    
    // bitwise operators
    ogm_ast_spec_op_bitwise_and,
    ogm_ast_spec_op_bitwise_or,
    ogm_ast_spec_op_bitwise_xor,
    
    // unary operators
    ogm_ast_spec_opun_boolean_not,
    ogm_ast_spec_opun_boolean_not_kw,
    ogm_ast_spec_opun_bitwise_not
    
    // assignments
    // x = 4
    ogm_ast_spec_op_eq,
    ogm_ast_spec_op_pluseq,
    ogm_ast_spec_op_minuseq,
    ogm_ast_spec_op_timeseq,
    ogm_ast_spec_op_divisioneq,
    ogm_ast_spec_op_andeq,
    ogm_ast_spec_op_oreq,
    ogm_ast_spec_op_xoreq,
    ogm_ast_spec_op_unary_pre_plusplus,
    ogm_ast_spec_op_unary_post_plusplus,
    ogm_ast_spec_op_unary_pre_minusminus,
    ogm_ast_spec_op_unary_post_minusminus,
    
    //// body types ////
    // this is a brace-surrounded body
    ogm_ast_spec_body_braced,
    // this body has no braces (just a list of statements)
    ogm_ast_spec_body_braceless,
    ogm_ast_spec_count
} ogm_ast_spec_t;

typedef struct ogm_ast
{
    ogm_ast_type_t m_type;
    ogm_ast_subtype_t m_subtype;
    
    union
    {
        void* m_payload;
        ogm_ast_spec_t m_spec;
    }
    
    // subtrees
    size_t m_sub_c;
    ogm_ast_t* m_sub; 
} ogm_ast_t;

// var or globvar declaration
typedef struct ogm_ast_declaration
{
    char* type;
    char** m_identifier;
    size_t m_identifier_count;
} ogm_ast_declaration_t;


typedef enum ogm_ast_literal_primitive_type
{
    ogm_ast_literal_t_dec,
    ogm_ast_literal_t_hex,
    ogm_ast_literal_t_string
} ogm_ast_literal_primitive_type_t;

// payload value of a literal primitive
typedef struct ogm_ast_literal_primitive
{
    ogm_ast_literal_primitive_type_t m_type;
    
    char* value;
} ogm_ast_literal_primitive_t;

//// decor ////

typedef enum ogm_ast_decor_type
{
    ogm_ast_decor_t_whitespace,
    ogm_ast_decor_t_comment_sl,
    ogm_ast_decor_t_comment_ml,
    ogm_ast_decor_t_list,
} ogm_ast_decor_type_t;

typedef struct ogm_ast_decor
{
    ogm_ast_decor_type_t type;
    void* m_payload;
} ogm_ast_decor_t;

typedef struct ogm_ast_decor_list
{
    
} ogm_ast_decor_list_t;

extern "C"
{
    // string associated with spec
    const char* ogm_ast_spec_string[];
    
    // parse AST.
    bool ogm_ast_parse(
        char* code,
        ogm_ast_t* outTree,
        ogm_ast_decor_t* outDecorTree
    );
    
    // free AST
    void ogm_ast_free(
        ogm_ast_t*
    );
    
    // free decor tree
    void ogm_ast_decor_free(
        ogm_ast_decor_t*
    );
}

#endif