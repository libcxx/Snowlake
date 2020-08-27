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
#include "SemanticAnalyzer.h"
#include "Synthesizer.h"
#include "ast_fwd.h"
#include "parser/ParserDriver.h"

#include <cstdlib>
#include <iostream>

// -----------------------------------------------------------------------------

static void print_semantic_analyzer_errors(const std::string&,
                                           const SemanticAnalyzer&,
                                           std::ostream&);

// -----------------------------------------------------------------------------

#define PRINT_VERBOSE_MSG(msg)                                                 \
  do {                                                                         \
    if (cmdl_opts.verbose && !cmdl_opts.silent) {                              \
      std::cerr << (msg) << std::endl;                                         \
    }                                                                          \
  } while (0)

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
  CmdlDriver cmdl_driver;
  res = cmdl_driver.run(argc, argv);
  if (!res) {
    return EXIT_FAILURE;
  }

  const auto& cmdl_opts = cmdl_driver.options();

  if (cmdl_opts.verbose && !cmdl_opts.silent) {
    std::cout << "Input:" << std::endl;
    std::cout << cmdl_opts.input_path << std::endl;
    std::cout << std::endl;
    std::cout << "Output path:" << std::endl;
    std::cout << cmdl_opts.output_path << std::endl;
    std::cout << std::endl;
  }

  // Parsing.
  ParserDriver::Options parser_opts{
      .trace_lexer = cmdl_opts.debugMode,
      .trace_parser = cmdl_opts.debugMode,
      .suppress_error_messages = !(cmdl_opts.debugMode),
  };

  ParserDriver parser(parser_opts);
  res = parser.parseFromFile(cmdl_opts.input_path);
  if (res != 0) {
    PRINT_VERBOSE_MSG("Failed: parsing error");
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
    if (!cmdl_opts.silent) {
      print_semantic_analyzer_errors(cmdl_opts.input_path, sema_analyzer,
                                     std::cout);
    }
    return EXIT_FAILURE;
  }

  // Synthesis.
  Synthesizer::Options synthesis_opts{
    .use_exception = false,
    .output_path = cmdl_opts.output_path
  };
  Synthesizer synthesizer(synthesis_opts);
  res = synthesizer.run(module);
  if (!res) {
    if (!cmdl_opts.silent) {
      std::cerr << "Error: Failed to synthesize output to: "
                << cmdl_opts.output_path;
    }
    return EXIT_FAILURE;
  }

  PRINT_VERBOSE_MSG("- SUCCESS -");

  // SUCCESS.
  return EXIT_SUCCESS;
}

// -----------------------------------------------------------------------------

static void
print_semantic_analyzer_errors(const std::string& input_path,
                               const SemanticAnalyzer& sema_analyzer,
                               std::ostream& out)
{
  size_t errors_count = 0;
  size_t warnings_count = 0;
  for (const auto& error : sema_analyzer.errors()) {
    if (error.code == SemanticAnalyzer::ErrorCode::Error) {
      ++errors_count;
      out << input_path << ": error: ";
    } else if (error.code == SemanticAnalyzer::ErrorCode::Warning) {
      ++warnings_count;
      out << input_path << ": warning: ";
    }
    out << error.msg << std::endl;
  }

  out << std::endl;
  if (warnings_count) {
    out << warnings_count;
    out << " warning";
    if (warnings_count > 1) {
      out << 's';
    }
    out << " generated." << std::endl;
  }
  if (errors_count) {
    out << errors_count;
    out << " error";
    if (errors_count > 1) {
      out << 's';
    }
    out << " generated." << std::endl;
  }
}

// -----------------------------------------------------------------------------
