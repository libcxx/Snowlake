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
#include "SemanticAnalyzer.h"
#include "Synthesizer.h"
#include "ast_fwd.h"
#include "parser/ParserDriver.h"

#include <cstdlib>
#include <fstream>

// -----------------------------------------------------------------------------

ProgramDriver::ProgramDriver()
  : m_cmdl_driver()
{
}

// -----------------------------------------------------------------------------

int
ProgramDriver::run(int argc, char** argv)
{
  bool res = true;
  res = m_cmdl_driver.run(argc, argv);
  if (!res) {
    return EXIT_FAILURE;
  }

  const auto& cmdl_opts = m_cmdl_driver.options();

  // Parsing.
  ParserDriver::Options parser_opts{
      .trace_lexer = cmdl_opts.debugMode,
      .trace_parser = cmdl_opts.debugMode,
      .suppress_error_messages = !(cmdl_opts.debugMode),
  };

  ParserDriver parser(parser_opts);
  res = parser.parse_from_file(cmdl_opts.input_path);
  if (res != 0) {
    return EXIT_FAILURE;
  }

  const auto& module = parser.module();

  // Semantic analysis.
  SemanticAnalyzer::Options sema_opts{
      .bailOnFirstError = cmdl_opts.bailOnFirstError,
      .warningsAsErrors = cmdl_opts.warningsAsErrors,
      .verbose = cmdl_opts.debugMode};
  SemanticAnalyzer sema_analyzer(sema_opts);
  res = sema_analyzer.run(module);
  if (!res) {
    return EXIT_FAILURE;
  }

  // Synthesis.
  std::ofstream file_stream(cmdl_opts.output_path.c_str(), std::ofstream::out);
  if (!file_stream.good()) {
    return EXIT_FAILURE;
  }
  Synthesizer::Options synthesis_opts{};
  Synthesizer synthesizer(synthesis_opts);
  res = synthesizer.run(module, file_stream);
  if (!res) {
    return EXIT_FAILURE;
  }

  // SUCCESS.
  return EXIT_SUCCESS;
}

// -----------------------------------------------------------------------------
