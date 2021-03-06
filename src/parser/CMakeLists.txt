#!/bin/bash
#
# The MIT License (MIT)
#
# Copyright (c) 2020 Tomiko
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


## Pre-compiled parser source dir.
set(PARSER_PRECOMPILED_SRC_DIR ${PARSER_SRC_DIR}/precompiled)

set(USE_PRECOMPILED_PARSER true)


## Lexer

if(NOT USE_PRECOMPILED_PARSER)
    set(lexer_source
        ${PARSER_SRC_DIR}/lexer.ll
        )

    add_custom_command(OUTPUT lex.yy.hh lex.yy.cc
        COMMAND flex -o lex.yy.cc --header-file=lex.yy.hh ${lexer_source})
endif()


## Parser

if(NOT USE_PRECOMPILED_PARSER)
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
endif()


## Copy compiled parser files.

if(USE_PRECOMPILED_PARSER)
    add_custom_command(
        OUTPUT
            copy_precompiled_parser
        COMMAND
            echo "Copying precompiled .hh files ..." &&
            cp ${PARSER_PRECOMPILED_SRC_DIR}/*.hh ${PARSER_SRC_DIR}/)
else()
    add_custom_command(
        OUTPUT
            copy_freshly_compiled_parser DEPENDS lex.yy.hh parser.tab.hh
        COMMAND
            echo "Copying generated .hh files ..." &&
            cp ${CMAKE_CURRENT_BINARY_DIR}/*.hh ${PARSER_SRC_DIR}/)
endif()


## Composite parser static library.

if(USE_PRECOMPILED_PARSER)
    set(parser_sources
        ${PARSER_PRECOMPILED_SRC_DIR}/lex.yy.cc
        ${PARSER_PRECOMPILED_SRC_DIR}/parser.tab.cc
        ParserDriver.cpp
        copy_precompiled_parser
        )
else()
    set(parser_sources
        lex.yy.cc
        parser.tab.cc
        ParserDriver.cpp
        copy_freshly_compiled_parser
        )
endif()


add_library(Parser STATIC ${parser_sources})


# Auto-generated code from Flex and Bison have a lot of warnings.
# Silence them for now.
set_target_properties(Parser PROPERTIES COMPILE_FLAGS "-Wno-everything")


# Post-build command.
add_custom_command(TARGET Parser
    POST_BUILD COMMAND ls -al $<TARGET_FILE:Parser>
    )


### THE END ###
