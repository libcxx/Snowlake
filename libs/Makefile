#!/bin/bash

# The MIT License (MIT)

# Copyright (c) 2018 William Li

# Permission is hereby granted, free of charge, to any person obtaining a copy of
# this software and associated documentation files (the "Software"), to deal in
# the Software without restriction, including without limitation the rights to
# use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
# the Software, and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions:

# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
# COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
# IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


## Makefile for building all dependencies.


INCLUDE=./include
SRC=./src
BUILD=./build
ROOT_DIR=$(shell pwd)

GTEST=gtest
GTEST_SHA=c99458533a9b4c743ed51537e25989ea55944908 # googletest 1.7.0
GTEST_DIR=./googletest
GTEST_BUILD=./$(BUILD)/$(GTEST)
GTEST_INCLUDE_DIR=$(GTEST_DIR)/include/gtest
GTEST_INCLUDE_TARGET_DIR=/usr/local/include/
GTEST_STATIC_LIB=libgtest.a
GTEST_STATIC_LIB_PATH=$(GTEST_BUILD)/$(GTEST_STATIC_LIB)
GTEST_STATIC_LIB_TARGET_DIR=/usr/local/lib

CMAKE=`which cmake`

LIB_INSTALL_DIR=/usr/local/lib


.PHONY: all
all: gtest


.PHONY: gtest
gtest:
	git submodule sync
	git submodule update --init
	mkdir -p $(GTEST_BUILD)
	#cd $(ROOT_DIR)/$(GTEST_DIR) && git fetch origin && git checkout $(GTEST_SHA)
	cd $(ROOT_DIR)/$(GTEST_BUILD) && $(CMAKE) $(ROOT_DIR)/$(GTEST_DIR)
	cd $(ROOT_DIR)/$(GTEST_BUILD) && $(MAKE)
	sudo cp -rf $(GTEST_INCLUDE_DIR) $(GTEST_INCLUDE_TARGET_DIR)
	sudo cp $(GTEST_STATIC_LIB_PATH) $(GTEST_STATIC_LIB_TARGET_DIR)
