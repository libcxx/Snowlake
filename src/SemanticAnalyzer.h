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

#pragma once

#include "ASTUtils.h"
#include "ASTVisitor.h"
#include "CompilerError.h"
#include "CompilerErrorPrinter.h"
#include "SemanticAnalysisErrorCategory.h"

#include <cstdio>
#include <string>

struct InferenceDefnContext;

typedef struct InferenceDefnContext* InferenceDefnContextRef;

class SemanticAnalyzer : public ASTVisitor
{
public:
  struct Options
  {
    bool bailOnFirstError;
    bool warningsAsErrors;
    bool verbose;
  };

  SemanticAnalyzer();

  explicit SemanticAnalyzer(const Options&);

  bool run(const ASTModule&);

  const Options& options() const;

  void setCompilerErrorPrinter(CompilerErrorPrinter*);

private:
  bool previsit(const ASTModule&) override;
  bool previsit(const ASTInferenceGroup&) override;
  bool previsit(const ASTInferenceDefn&) override;

  bool checkRequiredEnvDefns(const SymbolSet&);

  bool recursivePremiseDefnCheck(const ASTPremiseDefn&,
                                 InferenceDefnContextRef);

  template <typename T>
  bool recursivePremiseDefnCheck(const T&, InferenceDefnContextRef);

private:
  enum
  {
    MAX_MSG_LEN = 1024
  };

  template <typename U, typename... Args>
  void addWarning(const U& msg, Args... args)
  {
    if (_opts.warningsAsErrors) {
      addError(msg, args...);
    } else {
      char buffer[MAX_MSG_LEN] = {0};
      snprintf(buffer, sizeof(buffer), msg, args...);
      if (_errorPrinter) {
        _errorPrinter->printError(
            SemanticAnalysisErrorCategory::
                CreateCompilerErrorWithTypeAndMessage(
                    CompilerError::Type::Warning, 0 /** code **/, buffer));
      }
    }
  }

  template <typename U, typename... Args>
  void addError(const U& msg, Args... args)
  {
    char buffer[MAX_MSG_LEN] = {0};
    snprintf(buffer, sizeof(buffer), msg, args...);
    if (_errorPrinter) {
      _errorPrinter->printError(
          SemanticAnalysisErrorCategory::CreateCompilerErrorWithTypeAndMessage(
              CompilerError::Type::Error, 0 /** code **/, buffer));
    }
  }

  Options _opts;
  CompilerErrorPrinter* _errorPrinter;
};
