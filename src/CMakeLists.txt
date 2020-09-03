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


### LIBRARY


# Add 'parser' subdirectory.
set(PARSER_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/parser)
include_directories(${PARSER_SRC_DIR})
ADD_SUBDIRECTORY(${PARSER_SRC_DIR})


# Add the necessary source files.
set(sources
    ASTVisitor.cpp
    ASTUtils.cpp
    CompilerError.cpp
    CompilerErrorHandlerRegistrar.cpp
    SemanticAnalyzer.cpp
    Synthesizer.cpp
    ArgumentParser.cpp
    CmdlDriver.cpp
    ProgramDriver.cpp
    )


# Library `snowlake`.
add_library(snowlake STATIC ${sources})


# Additional compiler flags.
set_target_properties(snowlake
    PROPERTIES OUTPUT_NAME "snowlake"
    )


# Link with dependency "Parser".
add_dependencies(snowlake Parser)
target_link_libraries(snowlake Parser)


# Post-build command.
add_custom_command(TARGET snowlake
    POST_BUILD COMMAND ls -al $<TARGET_FILE:snowlake>
    )


### MAIN EXECUTABLE


# Add the necessary source files.
set(exec_sources
    main.cpp
    )


# Main executable `snowlake_compiler_exec`.
add_executable(snowlake_compiler_exec
    ${exec_sources}
    )


# Add the necessary dependencies.
add_dependencies(snowlake_compiler_exec Parser)
add_dependencies(snowlake_compiler_exec snowlake)


# Link with the necessary libraries.
target_link_libraries(snowlake_compiler_exec
    PRIVATE Parser
    PRIVATE snowlake
    )


# Additional compiler flags.
set_target_properties(snowlake_compiler_exec
    PROPERTIES OUTPUT_NAME "snowlakec"
    )


# Post-build command.
add_custom_command(TARGET snowlake_compiler_exec
    POST_BUILD COMMAND ls -al $<TARGET_FILE:snowlake_compiler_exec>
    )


### THE END ###
