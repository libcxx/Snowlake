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


## Add subdirectories.

# Add 'grammar' subdirectory.
set(SRC_GRAMMAR_DIR ${CMAKE_CURRENT_SOURCE_DIR}/grammar)
include_directories(${SRC_GRAMMAR_DIR})
ADD_SUBDIRECTORY(${SRC_GRAMMAR_DIR})
file(GLOB_RECURSE grammar_sources
    ${CMAKE_CURRENT_BINARY_DIR}/lex.yy.cc
    ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.cc)


# Add the necessary source files.
set(sources
    SemanticAnalyzer.cpp
    )


# Library `libsnowlake`.
add_library(libsnowlake ${sources} ${grammar_sources})


# Additional compiler flags.
set_target_properties(libsnowlake PROPERTIES COMPILE_FLAGS "-O3" OUTPUT_NAME "snowlake")


# Add dependency on target `Parser`.
add_dependencies(libsnowlake Parser)


# Post-build command.
add_custom_command(TARGET libsnowlake POST_BUILD COMMAND ls -al $<TARGET_FILE:libsnowlake>)


### THE END ###
