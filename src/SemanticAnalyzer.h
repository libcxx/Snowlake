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

#include "ASTVisitor.h"

class SemanticAnalyzer : public ASTVisitor
{
public:
  enum class StatusCode
  {
    NoError = 0x00,
    Warning = 0x01,
    Error   = 0x10
  };

public:
  SemanticAnalyzer();

  StatusCode visit(const ASTModule&);

private:
  StatusCode status() const;
  void set_status(StatusCode);

  template <typename U>
  void set_warning_msg(const U msg) {
    set_msg(msg);
    set_status(StatusCode::Warning);
  }

  template <typename U>
  void set_error_msg(const U msg) {
    set_msg(msg);
    set_status(StatusCode::Error);
  }

  virtual bool previsit(const ASTModule&) override;
  virtual bool postvisit(const ASTModule&) override;

  StatusCode m_status;
};
