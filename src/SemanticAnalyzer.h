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

#include <cstdio>
#include <string>
#include <vector>
#include <unordered_set>

class SemanticAnalyzer : public ASTVisitor
{
public:
  enum class ErrorCode
  {
    NoError = 0x00,
    Warning = 0x01,
    Error = 0x10
  };

  struct Error
  {
    ErrorCode code;
    std::string msg;
  };

public:
  typedef std::vector<Error> ErrorList;

  struct Options
  {
    bool bailOnFirstError;
    bool warningsAsErrors;
    bool verbose;
  };

  SemanticAnalyzer();

  explicit SemanticAnalyzer(const Options&);

  bool run(const ASTModule&);

  const ErrorList& errors() const;

  const Options& options() const;

private:
  virtual bool previsit(const ASTModule&) override;
  virtual bool previsit(const ASTInferenceGroup&) override;
  virtual bool previsit(const ASTInferenceDefn&) override;

  typedef std::unordered_set<std::string> SymbolSet;

  struct InferenceDefnContext
  {
    const std::string& name;
    SymbolSet symbolSet;
    TargetTable targetTbl;
  };

  bool checkRequiredEnvDefns(const SymbolSet&);

  bool recursivePremiseDefnCheck(const ASTPremiseDefn&, InferenceDefnContext*);

  template <typename T>
  bool recursivePremiseDefnCheck(const T&, InferenceDefnContext*);

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
      _errors.emplace_back(Error{ErrorCode::Warning, buffer});
    }
  }

  template <typename U, typename... Args>
  void addError(const U& msg, Args... args)
  {
    char buffer[MAX_MSG_LEN] = {0};
    snprintf(buffer, sizeof(buffer), msg, args...);
    _errors.emplace_back(Error{ErrorCode::Error, buffer});
  }

  std::vector<Error> _errors;
  Options _opts;
};
