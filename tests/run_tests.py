"""
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
"""

import argparse
import datetime
import json
import logging
import os
import subprocess
import sys


## -----------------------------------------------------------------------------

JSON_EXT = '.json'

def immediate_files(a_dir):
    for name in os.listdir(a_dir):
        child = os.path.join(a_dir, name)
        if os.path.isfile(child):
            yield child

def is_file_with_ext(path, ext):
    return os.path.exists(path) and path.endswith(ext)

def immediate_json_files(a_dir):
    for path in immediate_files(a_dir):
        if is_file_with_ext(path, JSON_EXT):
            yield path

## -----------------------------------------------------------------------------

REQUIRED_TESTCASE_FIELD_NAME = 'name'
REQUIRED_TESTCASE_FIELD_INPUT_PATH = 'input_path'
REQUIRED_TESTCASE_FIELD_EXPECTED_EXIT = 'expected_exit'

REQUIRED_TESTCASE_FIELDS = (
    REQUIRED_TESTCASE_FIELD_NAME,
    REQUIRED_TESTCASE_FIELD_INPUT_PATH,
    REQUIRED_TESTCASE_FIELD_EXPECTED_EXIT,
)

class Colors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

## -----------------------------------------------------------------------------

class NoOpLogger:
    def __print(self, msg):
        sys.stdout.write('{}\n'.format(msg))

    def info(self, msg):
        self.__print(msg)

    def error(self, msg):
        self.__print(msg)

    def debug(self, msg):
        self.__print(msg)

## -----------------------------------------------------------------------------

class TestRunner(object):

    class StatusCode:
        SUCCESS = 0x01
        FAILURE = 0x02
        ERROR   = 0x04

        @classmethod
        def tostring(cls, err):
            if err == cls.SUCCESS:
                return 'SUCCESS'
            elif err == cls.FAILURE:
                return 'FAILURE'
            elif err == cls.ERROR:
                return 'ERROR'
            else:
                return '(Unknown)'

    def __init__(self, input_path, opts):
        self.input_path = input_path
        self.opts = opts
        self.__init_logger()

    def __init_logger(self):
        """Initializer logger."""
        self.noop_logger = NoOpLogger()
        self.logger = logging.getLogger(type(self).__name__)
        self.logger.setLevel(logging.INFO)

        # Handler.
        self.log_filepath = os.path.join(os.getcwd(), 'tests_{t}.log'.format(
            t=datetime.datetime.now().strftime('%Y-%m-%d.%H:%M:%S')))
        console_handler = logging.FileHandler(self.log_filepath)
        console_handler.setLevel(logging.INFO)

        # Formatter.
        formatter = logging.Formatter("# %(asctime)s - %(name)s - %(levelname)s - %(message)s")
        console_handler.setFormatter(formatter)

        self.logger.addHandler(console_handler)

    def run(self):
        self.__run()

    def __run(self):
        print
        total_count = 0
        success_count = 0
        failure_count = 0
        error_count = 0
        for json_filepath in immediate_json_files(self.input_path):
            err = self.__run_test_case(json_filepath)
            total_count += 1
            if err == self.StatusCode.SUCCESS:
                success_count += 1
            elif err == self.StatusCode.FAILURE:
                failure_count += 1
            elif err == self.StatusCode.ERROR:
                error_count += 1

        print
        self.__log_msg('Total       : {}'.format(total_count), Colors.OKBLUE)
        self.__log_msg('Passed      : {}'.format(success_count), Colors.OKBLUE)
        self.__log_msg('Failed      : {}'.format(failure_count), Colors.OKBLUE)
        self.__log_msg('Errorred    : {}'.format(error_count), Colors.OKBLUE)

        self.noop_logger.info('Log saved to {}'.format(self.log_filepath))

        if success_count == total_count:
            self.__log_msg('- SUCCESS -', Colors.OKGREEN)
        else:
            self.__log_msg('- FAIL -', Colors.FAIL)

    def __run_test_case(self, json_filepath):
        try:
            with open(json_filepath, 'r') as fd:
                testcase = json.load(fd)
        except Exception as ex:
            self.__log_error('Failed to load {}: {}'.format(json_filepath, str(ex)), colors=self.colors)
            return self.StatusCode.ERROR

        for required_field in REQUIRED_TESTCASE_FIELDS:
            if required_field not in testcase:
                self.__log_error('Test case at {} does not have required fields'.format(json_filepath))
                return self.StatusCode.ERROR

        return self.__invoke_test_case(testcase)

    def __invoke_test_case(self, testcase):
        testcase_name = testcase[REQUIRED_TESTCASE_FIELD_NAME]
        testcase_inputpath = testcase[REQUIRED_TESTCASE_FIELD_INPUT_PATH]
        testcase_expected_exit = testcase[REQUIRED_TESTCASE_FIELD_EXPECTED_EXIT]

        testcase_inputpath = os.path.join(self.input_path, testcase_inputpath)

        if not os.path.exists(testcase_inputpath) or not os.path.isfile(testcase_inputpath):
            self.__log_error('Test case \"{}\" does not have valid input'.format(testcase_name))
            return self.StatusCode.ERROR

        cmd = 'snowlake --silent --output ./ {input_path}'.format(input_path=testcase_inputpath)

        return_code = subprocess.call(cmd, shell=True)

        err = self.StatusCode.SUCCESS if return_code == testcase_expected_exit else self.StatusCode.FAILURE

        if err == self.StatusCode.SUCCESS:
            self.__log_success('[{}] - {}'.format(testcase_name, self.StatusCode.tostring(err)))
        else:
            self.__log_error('[{}] - {}'.format(testcase_name, self.StatusCode.tostring(err)), colors=self.opts.colors)

        if err == self.StatusCode.FAILURE:
            self.__log_error(
                'Test case failed with exit code {return_code}. Expected {testcase_expected_exit}. ({testcase_inputpath})'.format(
                    return_code=return_code,
                    testcase_expected_exit=testcase_expected_exit,
                    testcase_inputpath=testcase_inputpath),
                    colors=self.opts.colors)

        return err

    def __log_msg(self, msg, color=None):
        if color:
            self.noop_logger.info('{}{}{}'.format(color, msg, Colors.ENDC))
        else:
            self.noop_logger.info(msg)
        self.logger.info(msg)

    def __log_success(self, msg):
        self.__log_msg(msg, Colors.OKGREEN)

    def __log_error(self, msg, colors=False):
        if colors:
            self.noop_logger.error('{}{}{}'.format(Colors.FAIL, msg, Colors.ENDC))
        else:
            self.noop_logger.error(msg)
        self.logger.error(msg)

    def __log_verbose(self, msg):
        if self.opts.verbose:
            self.__log_msg(msg)

## -----------------------------------------------------------------------------

def main():
    parser = argparse.ArgumentParser(description='Snowlake integration test suite.')
    parser.add_argument('input', nargs='?', default=os.path.dirname(os.path.realpath(__file__)))
    parser.add_argument('--verbose', dest='verbose', action='store_true', default=False, help='Verbose mode')
    parser.add_argument('--colors', dest='colors', action='store_true', default=True, help='Colored output')

    args = parser.parse_args()

    if args.verbose:
        print 'Arguments:'
        print args

    input_path = args.input

    if args.verbose:
        print 'Input path:'
        print input_path

    if not os.path.exists(input_path) or not os.path.isdir(input_path):
        sys.stderr.write('Please specify a valid input directory.\n')
        sys.exit(-1)

    runner = TestRunner(input_path, args)
    runner.run()


if __name__ == '__main__':
    main()
