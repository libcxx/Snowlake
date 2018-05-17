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


# Build executable `run_tests`.
add_executable(run_tests
    SemanticAnalyzerTests.cpp
    main.cpp
    )


set_target_properties(run_tests PROPERTIES COMPILE_FLAGS "-D__DEBUG__=1 -g -Wno-weak-vtables -Wno-missing-variable-declarations")


target_link_libraries(run_tests
    libsnowlake
    ${LIBGTEST_LIBRARY}
    ${DEPENDENCY_LINK_LIBRARIES}
    )


# Run tests.
add_custom_command(TARGET run_tests POST_BUILD COMMAND
    $<TARGET_FILE:run_tests> --gtest_color=yes --gtest_shuffle --gtest_repeat=3)
