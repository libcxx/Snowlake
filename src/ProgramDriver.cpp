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

#include "ProgramDriver.h"

#include "CmdlDriver.h"
#include "ErrorPrinter.h"
#include "SemanticAnalyzer.h"
#include "Synthesizer.h"
#include "ast_fwd.h"
#include "parser/ParserDriver.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>

// -----------------------------------------------------------------------------

ProgramDriver::ProgramDriver()
{
}

// -----------------------------------------------------------------------------

int
ProgramDriver::run(int argc, char** argv)
{
  bool res = true;

  // Cmdl driver.
  CmdlDriver cmdlDriver;
  res = cmdlDriver.run(argc, argv);
  if (!res) {
    return EXIT_FAILURE;
  }

  const auto& cmdlOpts = cmdlDriver.options();

  if (cmdlOpts.verbose && !cmdlOpts.silent) {
    std::cout << "Input:" << std::endl;
    std::cout << cmdlOpts.inputPath << std::endl;
    std::cout << std::endl;
    std::cout << "Output path:" << std::endl;
    std::cout << cmdlOpts.outputPath << std::endl;
    std::cout << std::endl;
  }

  ErrorPrinter errorPrinter(cmdlOpts.inputPath, std::cout);

  // Parsing.
  ParserDriver::Options parserOpts{.traceLexer = cmdlOpts.debugMode,
                                   .traceParser = cmdlOpts.debugMode,
                                   .suppressErrorMessages = false};

  Error parserError;
  parserError.code = Error::ErrorCode::NoError;

  ParserDriver parser(parserOpts);
  res = parser.parseFromFile(cmdlOpts.inputPath, &parserError);
  if (res != 0) {
    errorPrinter.printError(parserError);
    return EXIT_FAILURE;
  }

  const auto& module = parser.module();

  // Semantic analysis.
  SemanticAnalyzer::Options semaOpts{
      .bailOnFirstError = cmdlOpts.bailOnFirstError,
      .warningsAsErrors = cmdlOpts.warningsAsErrors,
      .verbose = cmdlOpts.debugMode};
  SemanticAnalyzer semaAnalyzer(semaOpts);
  res = semaAnalyzer.run(module);
  if (!res) {
    if (!cmdlOpts.silent) {
      auto& semaErrors = semaAnalyzer.errors();
      errorPrinter.printErrors(std::begin(semaErrors), std::end(semaErrors));
    }
    return EXIT_FAILURE;
  }

  // Synthesis.
  Synthesizer::Options synthesisOpts{.useException = false,
                                     .outputPath = cmdlOpts.outputPath};
  Synthesizer synthesizer(synthesisOpts);
  res = synthesizer.run(module);
  if (!res) {
    if (!cmdlOpts.silent) {
      char buf[2048] = {0};
      snprintf(buf, sizeof(buf), "Error: Failed to synthesize output to: %s",
               cmdlOpts.outputPath.c_str());
      errorPrinter.printError(
          Error{.code = Error::ErrorCode::Error, .msg = buf});
    }
    return EXIT_FAILURE;
  }

  // SUCCESS.
  return EXIT_SUCCESS;
}

// -----------------------------------------------------------------------------
