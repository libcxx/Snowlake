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


## Integration test run.


set(RUN_TESTS_PY ${PROJECT_SOURCE_DIR}/tests/run_tests.py)
set(RUN_TESTS_PY_INPUT_PATH ${PROJECT_SOURCE_DIR}/tests/)
set(RUN_TESTS_PY_OPTIONS --verbose --colors --logs)
set(RUN_TESTS_COMMAND
    python ${RUN_TESTS_PY} ${RUN_TESTS_PY_INPUT_PATH} ${RUN_TESTS_PY_OPTIONS})


# Integration test run command.
add_custom_command(OUTPUT integration_tests_run
    COMMAND ${RUN_TESTS_COMMAND}
    )


# Integration test dummy target.
add_custom_target(integration_tests ALL
    DEPENDS integration_tests_run
    )
