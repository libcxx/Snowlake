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
%code requires
{
# include <cstdint>
# include <string>
# include <utility>
}
// The parsing context.
%param { void* yyscanner }
%locations
%define parse.error verbose
%initial-action
{
};
%code
{
}
%token <std::string>      KEYWORD_GROUP
%token <std::string>      KEYWORD_INFERENCE
%token <std::string>      KEYWORD_ENVIRONMENT
%token <std::string>      KEYWORD_ARGUMENTS
%token <std::string>      KEYWORD_PREMISES
%token <std::string>      KEYWORD_PROPOSITION

%token <std::string>      IDENTIFIER
%token <std::string>      COMMA
%token <std::string>      DOT
%token <std::string>      COLON
%token <std::string>      SEMICOLON
%token <std::string>      PERCENT
%token <std::string>      STAR
%token <std::string>      POND
%token <std::string>      EQUAL
%token <std::string>      LPAREN
%token <std::string>      RPAREN
%token <std::string>      LBRACKET
%token <std::string>      RBRACKET
%token <std::string>      LBRACE
%token <std::string>      RBRACE
%token END                0  "end of file"

%debug

%%

%start input;

input
    :   inference_group_set
        {
        }
    ;

inference_group_set
    :
        {
        }
    |
        inference_group_set inference_group
        {
        }
    ;

inference_group
    :   KEYWORD_INFERENCE LBRACE
            environment_defn_set
            inference_defn_list
        RBRACE
        {

        }
    ;

environment_defn_set
    :
        {
        }
    |   environment_defn_set environment_defn
        {
        }
    ;

environment_defn
    :   IDENTIFIER COLON IDENTIFIER
        {
        }
    ;

inference_defn_list
    :
        {
        }
    | inference_defn_list inference_defn
        {
        }
    ;

inference_defn
    :   KEYWORD_INFERENCE LBRACE
            inference_argument_set
            inference_premises_set
            inference_proposition
        RBRACE
        {
        }
    ;

inference_argument_set
    :   KEYWORD_ARGUMENTS COLON LBRACKET inference_argument_list RBRACKET
        {
        }
    ;

inference_argument_list
    :
        {
        }
    | inference_argument_list inference_argument
        {
        }
    ;

inference_argument
    :   IDENTIFIER
        {
        }
    ;

inference_premises_set
    : KEYWORD_PREMISES COLON LBRACKET inference_premises_list RBRACKET
        {
        }
    ;

inference_premises_list
    :
        {
        }
    |   inference_premises_list inference_premise_stmt
        {
        }
    ;

inference_premise_stmt
    :   inference_premise_type_inference_stmt
        {
        }
    |   inference_premise_type_equality_stmt
        {
        }
    ;

inference_premise_type_inference_stmt
    :   identifiable COLON identifier
    ;

inference_premise_type_equality_stmt
    :   identifiable equality_operator identifiable
    ;

inference_proposition
    :   KEYWORD_PROPOSITION COLON identifiable
    ;

identifiable
    :   identifier
    |   identifiable_attribute
    ;

identifier
    :   IDENTIFIER
    ;

identifiable_attribute
    :   identifiable DOT identifier
    ;

equality_operator
    :   EQUAL
    ;

%%

void
yy::ir_parser::error(const location_type& l, const std::string& m)
{
}
