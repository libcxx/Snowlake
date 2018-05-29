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

  virtual void previsit(const ASTInferenceGroup&, const ASTModule&);
  virtual void postvisit(const ASTInferenceGroup&, const ASTModule&);

  virtual void previsit(const ASTEnvironmentDefn&, const ASTInferenceGroup&);
  virtual void postvisit(const ASTEnvironmentDefn&, const ASTInferenceGroup&);

  virtual void previsit(const ASTInferenceDefn&, const ASTInferenceGroup&);
  virtual void postvisit(const ASTInferenceDefn&, const ASTInferenceGroup&);

  virtual void previsit(const ASTGlobalDecl&, const ASTInferenceDefn&);
  virtual void postvisit(const ASTGlobalDecl&, const ASTInferenceDefn&);

  virtual void previsit(const ASTInferenceArgument&, const ASTInferenceDefn&);
  virtual void postvisit(const ASTInferenceArgument&, const ASTInferenceDefn&);

  virtual void previsit(const ASTPremiseDefn&, const ASTInferenceDefn&);
  virtual void postvisit(const ASTPremiseDefn&, const ASTInferenceDefn&);

  virtual void previsit(const ASTPropositionDefn&, const ASTInferenceDefn&);
  virtual void postvisit(const ASTPropositionDefn&, const ASTInferenceDefn&);

  virtual void previsit(const ASTDeductionTarget&, const ASTPropositionDefn&);
  virtual void postvisit(const ASTDeductionTarget&, const ASTPropositionDefn&);

private:
  void visit(const ASTInferenceGroup&, const ASTModule&);
  void visit(const ASTEnvironmentDefn&, const ASTInferenceGroup&);
  void visit(const ASTInferenceDefn&, const ASTInferenceGroup&);
  void visit(const ASTGlobalDecl&, const ASTInferenceDefn&);
  void visit(const ASTInferenceArgument&, const ASTInferenceDefn&);
  void visit(const ASTPremiseDefn&, const ASTInferenceDefn&);
  void visit(const ASTPropositionDefn&, const ASTInferenceDefn&);
  void visit(const ASTDeductionTarget&, const ASTPropositionDefn&);
};
