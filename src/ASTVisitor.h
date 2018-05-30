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

  bool visit(const ASTModule&);

  virtual bool previsit(const ASTModule&);
  virtual bool postvisit(const ASTModule&);

  virtual bool previsit(const ASTInferenceGroup&);
  virtual bool postvisit(const ASTInferenceGroup&);

  virtual bool previsit(const ASTEnvironmentDefn&);
  virtual bool postvisit(const ASTEnvironmentDefn&);

  virtual bool previsit(const ASTInferenceDefn&);
  virtual bool postvisit(const ASTInferenceDefn&);

  virtual bool previsit(const ASTGlobalDecl&);
  virtual bool postvisit(const ASTGlobalDecl&);

  virtual bool previsit(const ASTInferenceArgument&);
  virtual bool postvisit(const ASTInferenceArgument&);

  virtual bool previsit(const ASTInferencePremiseDefn&);
  virtual bool postvisit(const ASTInferencePremiseDefn&);

  virtual bool previsit(const ASTInferenceEqualityDefn&);
  virtual bool postvisit(const ASTInferenceEqualityDefn&);

  virtual bool previsit(const ASTPropositionDefn&);
  virtual bool postvisit(const ASTPropositionDefn&);

  virtual bool previsit(const ASTDeductionTarget&);
  virtual bool postvisit(const ASTDeductionTarget&);

  virtual bool previsit(const ASTDeductionTargetSingular&);
  virtual bool postvisit(const ASTDeductionTargetSingular&);

  virtual bool previsit(const ASTDeductionTargetArray&);
  virtual bool postvisit(const ASTDeductionTargetArray&);

  virtual bool previsit(const ASTDeductionTargetComputed&);
  virtual bool postvisit(const ASTDeductionTargetComputed&);

private:
  bool visit(const ASTInferenceGroup&);
  bool visit(const ASTEnvironmentDefn&);
  bool visit(const ASTInferenceDefn&);
  bool visit(const ASTGlobalDecl&);
  bool visit(const ASTInferenceArgument&);
  bool visit(const ASTPremiseDefn&);
  bool visit(const ASTInferencePremiseDefn&);
  bool visit(const ASTInferenceEqualityDefn&);
  bool visit(const ASTWhileClause&);
  bool visit(const ASTPropositionDefn&);
  bool visit(const ASTDeductionTarget&);
  bool visit(const ASTDeductionTargetSingular&);
  bool visit(const ASTDeductionTargetArray&);
  bool visit(const ASTDeductionTargetComputed&);
};
