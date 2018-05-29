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

#include "ast_fwd.h"

class ASTVisitor
{
protected:
  ASTVisitor();

  virtual ~ASTVisitor();

  void visit(const ASTModule&);

  virtual void previsit(const ASTModule&);
  virtual void postvisit(const ASTModule&);

  virtual void previsit(const ASTInferenceGroup&);
  virtual void postvisit(const ASTInferenceGroup&);

  virtual void previsit(const ASTEnvironmentDefn&);
  virtual void postvisit(const ASTEnvironmentDefn&);

  virtual void previsit(const ASTInferenceDefn&);
  virtual void postvisit(const ASTInferenceDefn&);

  virtual void previsit(const ASTGlobalDecl&);
  virtual void postvisit(const ASTGlobalDecl&);

  virtual void previsit(const ASTInferenceArgument&);
  virtual void postvisit(const ASTInferenceArgument&);

  virtual void previsit(const ASTPremiseDefn&);
  virtual void postvisit(const ASTPremiseDefn&);

  virtual void previsit(const ASTPropositionDefn&);
  virtual void postvisit(const ASTPropositionDefn&);

  virtual void previsit(const ASTDeductionTarget&);
  virtual void postvisit(const ASTDeductionTarget&);

private:
  void visit(const ASTInferenceGroup&);
  void visit(const ASTEnvironmentDefn&);
  void visit(const ASTInferenceDefn&);
  void visit(const ASTGlobalDecl&);
  void visit(const ASTInferenceArgument&);
  void visit(const ASTPremiseDefn&);
  void visit(const ASTPropositionDefn&);
  void visit(const ASTDeductionTarget&);
};
