/*******************************************************************************
The MIT License (MIT)

Copyright (c) 2018 William Li

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*******************************************************************************/

%skeleton "lalr1.cc" /* -*- C++ -*- */

%require "3.0.4"

%defines
%define parser_class_name {Parser}
%define api.token.constructor
%define api.value.type variant
%define parse.assert
%define parse.error verbose

// Enable location.
%locations

// The parsing context.
%param { ParserDriver& driver }

// Initial code required.
%code requires
{
class ParserDriver;
#include "ast.h"
}

// Initialization.
%initial-action
{
  // Initialize the initial location.
  if (!driver.inputFile().empty())
  {
    @$.begin.filename = @$.end.filename = &driver.inputFile();
  }
};

// In source file (.cc)
%code
{
// HACK: We need to refer to the header under the source directory
// from the build directory.
// TODO: Currently this is the only hack that I can think of.
// Will need to devise a cleaner solution eventually.
#include "../../../src/parser/ParserDriver.h"
#include "lex.yy.hh"
}

// Token definition.
%token <std::string>      KEYWORD_GROUP
%token <std::string>      KEYWORD_INFERENCE
%token <std::string>      KEYWORD_ENVIRONMENT
%token <std::string>      KEYWORD_ARGUMENTS
%token <std::string>      KEYWORD_GLOBALS
%token <std::string>      KEYWORD_WHILE
%token <std::string>      KEYWORD_INRANGE
%token <std::string>      KEYWORD_PREMISES
%token <std::string>      KEYWORD_PROPOSITION

%token <uint64_t>         INTEGER_LITERAL

%token <std::string>      IDENTIFIER
%token <std::string>      DOT
%token <std::string>      COMMA
%token <std::string>      COLON
%token <std::string>      SEMICOLON
%token <std::string>      LBRACKET
%token <std::string>      RBRACKET
%token <std::string>      LBRACE
%token <std::string>      RBRACE
%token <std::string>      LPAREN
%token <std::string>      RPAREN
%token <std::string>      OPERATOR_EQ
%token <std::string>      OPERATOR_NEQ
%token <std::string>      OPERATOR_LT
%token <std::string>      OPERATOR_LTE
%token <std::string>      ELLIPSIS
%token END                0  "end of file"

%type <EqualityOperator> equality_operator;
%type <ASTDeductionTarget> deduction_target;
%type <ASTDeductionTargetList> deduction_target_list;
%type <ASTDeductionTargetSingular> deduction_target_singular;
%type <ASTDeductionTargetArray> deduction_target_array;
%type <ASTDeductionTargetComputed> deduction_target_computed;
%type <ASTIdentifier> identifier;
%type <ASTIdentifiable> identifiable;
%type <ASTPropositionDefn> proposition_defn;
%type <ASTRangeClause> range_clause;
%type <ASTWhileClause> while_clause;
%type <ASTInferencePremiseDefn> premise_type_inference_defn;
%type <ASTInferenceEqualityDefn> premise_type_equality_defn;
%type <ASTPremiseDefn> premise_defn;
%type <ASTPremiseDefnList> premise_defn_list;
%type <ASTPremiseDefnList> premise_set;
%type <ASTInferenceArgument> inference_argument;
%type <ASTInferenceArgumentList> argument_list;
%type <ASTInferenceArgumentList> argument_set;
%type <ASTGlobalDecl> global_decl;
%type <ASTGlobalDeclList> global_decl_list;
%type <ASTGlobalDeclList> global_decl_set;
%type <ASTEnvironmentDefn> environment_defn;
%type <ASTEnvironmentDefnList> environment_defn_list;
%type <ASTInferenceDefn> inference_defn;
%type <ASTInferenceDefnList> inference_defn_list;
%type <ASTInferenceGroup> inference_group;
%type <ASTInferenceGroupList> inference_group_list;
%type <ASTModule> input;

%debug

%%

%start input;

input
    :
        inference_group_list
        {
            ASTModule module(std::move($1));
            driver.setModule(std::move(module));
        }
    ;

inference_group_list
    :
        {
            $$ = ASTInferenceGroupList();
        }
    |
        inference_group_list inference_group
        {
            $1.push_back($2);
            $$ = std::move($1);
        }
    ;

inference_group
    :
        KEYWORD_GROUP IDENTIFIER LBRACE
            environment_defn_list
            inference_defn_list
        RBRACE
        {
            $$ = ASTInferenceGroup(std::move($2), std::move($4), std::move($5));
        }
    ;

environment_defn_list
    :
        {
            $$ = ASTEnvironmentDefnList();
        }
    |
        environment_defn_list environment_defn
        {
            $1.push_back($2);
            $$ = std::move($1);
        }
    ;

environment_defn
    :
        IDENTIFIER COLON IDENTIFIER SEMICOLON
        {
            $$ = ASTEnvironmentDefn(std::move($1), std::move($3));
        }
    ;

inference_defn_list
    :
        {
            $$ = ASTInferenceDefnList();
        }
    |
        inference_defn_list inference_defn
        {
            $1.push_back($2);
            $$ = std::move($1);
        }
    ;

inference_defn
    :
        KEYWORD_INFERENCE IDENTIFIER LBRACE
            global_decl_set
            argument_set
            premise_set
            proposition_defn
        RBRACE
        {
            $$ = ASTInferenceDefn(std::move($2),
                std::move($4),std::move($5),std::move($6), std::move($7));
        }
    ;

global_decl_set
    :
        {
            $$ = ASTGlobalDeclList();
        }
    |
        KEYWORD_GLOBALS COLON LBRACKET global_decl_list RBRACKET
        {
            $$ = std::move($4);
        }
    ;

global_decl_list
    :
        global_decl
        {
            ASTGlobalDeclList decls;
            decls.push_back($1);
            $$ = std::move(decls);
        }
    |
        global_decl_list COMMA global_decl
        {
            $1.push_back($3);
            $$ = std::move($1);
        }
    ;

global_decl
    :
        IDENTIFIER
        {
            $$ = ASTGlobalDecl(std::move($1));
        }
    ;

argument_set
    :
        KEYWORD_ARGUMENTS COLON LBRACKET RBRACKET
        {
            $$ = ASTInferenceArgumentList();
        }
    |
        KEYWORD_ARGUMENTS COLON LBRACKET argument_list RBRACKET
        {
            $$ = std::move($4);
        }
    ;

argument_list
    :
        inference_argument
        {
            ASTInferenceArgumentList arguments;
            arguments.push_back($1);
            $$ = std::move(arguments);
        }
    |
        argument_list COMMA inference_argument
        {
            $1.push_back($3);
            $$ = std::move($1);
        }
    ;

inference_argument
    :
        IDENTIFIER COLON IDENTIFIER
        {
            $$ = ASTInferenceArgument(std::move($1), std::move($3));
        }
    ;

premise_set
    :
        KEYWORD_PREMISES COLON LBRACKET premise_defn_list RBRACKET
        {
            $$ = $4;
        }
    ;

premise_defn_list
    :
        {
            $$ = ASTPremiseDefnList();
        }
    |
        premise_defn_list premise_defn
        {
            $1.push_back($2);
            $$ = std::move($1);
        }
    ;

premise_defn
    :
        premise_type_inference_defn
        {
            $$ = ASTPremiseDefn(std::move($1));
        }
    |
        premise_type_equality_defn
        {
            $$ = ASTPremiseDefn(std::move($1));
        }
    ;

premise_type_inference_defn
    :
        identifiable COLON deduction_target SEMICOLON
        {
            $$ = ASTInferencePremiseDefn(std::move($1), std::move($3));
        }
    |
        identifiable COLON deduction_target while_clause SEMICOLON
        {
            $$ = ASTInferencePremiseDefn(std::move($1), std::move($3), std::move($4));
        }
    ;

while_clause
    :
        KEYWORD_WHILE LBRACE premise_defn_list RBRACE
        {
            $$ = ASTWhileClause(std::move($3));
        }
    ;

premise_type_equality_defn
    :
        deduction_target equality_operator deduction_target SEMICOLON
        {
            $$ = ASTInferenceEqualityDefn(std::move($1), std::move($3), $2);
        }
    |
        deduction_target equality_operator deduction_target range_clause SEMICOLON
        {
            $$ = ASTInferenceEqualityDefn(std::move($1), std::move($3), $2, std::move($4));
        }
    ;

range_clause
    :
        KEYWORD_INRANGE INTEGER_LITERAL ELLIPSIS INTEGER_LITERAL ELLIPSIS deduction_target
        {
            $$ = ASTRangeClause($2, $4, std::move($6));
        }
    ;

proposition_defn
    :
        KEYWORD_PROPOSITION COLON deduction_target SEMICOLON
        {
            $$ = ASTPropositionDefn(std::move($3));
        }
    ;

identifiable
    :
        identifier
        {
            ASTIdentifiable res;
            res.add($1);
            $$ = std::move(res);
        }
    |
        identifiable DOT identifier
        {
            $1.add($3);
            $$ = $1;
        }
    ;

identifier
    :
        IDENTIFIER
        {
            $$ = ASTIdentifier(std::move($1));
        }
    ;

deduction_target_list
    :
        deduction_target
        {
            ASTDeductionTargetList list;
            list.push_back($1);
            $$ = std::move(list);
        }
    |
        deduction_target_list COMMA deduction_target
        {
            $1.push_back($3);
            $$ = std::move($1);
        }
    ;

deduction_target
    :
        deduction_target_singular
        {
            $$ = ASTDeductionTarget(std::move($1));
        }
    |
        deduction_target_array
        {
            $$ = ASTDeductionTarget(std::move($1));
        }
    |
        deduction_target_computed
        {
            $$ = ASTDeductionTarget(std::move($1));
        }
    ;

deduction_target_singular
    :
        IDENTIFIER
        {
            $$ = ASTDeductionTargetSingular(std::move($1));
        }
    ;

deduction_target_array
    :
        IDENTIFIER LBRACKET RBRACKET
        {
            $$ = ASTDeductionTargetArray(std::move($1));
        }
    |
        IDENTIFIER LBRACKET INTEGER_LITERAL RBRACKET
        {
            $$ = ASTDeductionTargetArray(std::move($1), std::move($3));
        }
    ;

deduction_target_computed
    :
        IDENTIFIER LPAREN RPAREN
        {
            ASTDeductionTargetList arguments;
            $$ = ASTDeductionTargetComputed(std::move($1), std::move(arguments));
        }
    |
        IDENTIFIER LPAREN deduction_target_list RPAREN
        {
            $$ = ASTDeductionTargetComputed(std::move($1), std::move($3));
        }
    ;

equality_operator
    :
        OPERATOR_EQ
        {
            $$ = EqualityOperator::OPERATOR_EQ;
        }
    |
        OPERATOR_NEQ
        {
            $$ = EqualityOperator::OPERATOR_NEQ;
        }
    |
        OPERATOR_LT
        {
            $$ = EqualityOperator::OPERATOR_LT;
        }
    |
        OPERATOR_LTE
        {
            $$ = EqualityOperator::OPERATOR_LTE;
        }
    ;

%%

void
yy::Parser::error(const location_type& l, const std::string& m)
{
  driver.error(l, m);
}
