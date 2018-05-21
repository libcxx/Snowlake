#!/bin/bash
#
# The MIT License (MIT)
#
# Copyright (c) 2018 William Li
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of
# this software and associated documentation files (the "Software"), to deal in
# the Software without restriction, including without limitation the rights to
# use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
# the Software, and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
# COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
# IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


## Lexer

set(lexer_source
    ${PARSER_SRC_DIR}/lexer.ll
    )

add_custom_command(OUTPUT lex.yy.hh lex.yy.cc
    COMMAND flex -o lex.yy.cc --header-file=lex.yy.hh ${lexer_source})


## Parser

set(parser_source
    ${PARSER_SRC_DIR}/parser.yy
    )

add_custom_command(
    OUTPUT
        parser.tab.hh
        parser.tab.cc
        location.hh
        position.hh
        stack.hh
    COMMAND
        bison --language=c++ --verbose --warnings=error ${parser_source})


add_custom_command(
    OUTPUT
        fake_output DEPENDS lex.yy.hh parser.tab.hh
    COMMAND
        echo "Copying generated .hh files ..." &&
        cp ${CMAKE_CURRENT_BINARY_DIR}/*.hh ${PARSER_SRC_DIR}/)


## Composite parser static library.

set(parser_sources
    lex.yy.cc
    parser.tab.cc
    ParserDriver.cpp
    fake_output
    )

add_library(Parser ${parser_sources})

# Auto-generated code from Flex and Bison have a lot of warnings.
# Silence them for now.
set_target_properties(Parser PROPERTIES COMPILE_FLAGS "-Wno-everything")


### THE END ###
