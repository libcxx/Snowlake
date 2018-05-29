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

#include "ASTVisitor.h"
#include "ast.h"

// -----------------------------------------------------------------------------

ASTVisitor::ASTVisitor()
{
}

// -----------------------------------------------------------------------------

/* virtual */
ASTVisitor::~ASTVisitor()
{
}

// -----------------------------------------------------------------------------

void
ASTVisitor::visit(const ASTModule& module)
{
  previsit(module);

  for (const auto& inference_group : module.inference_groups())
  {
    visit(inference_group);
  }

  postvisit(module);
}

// -----------------------------------------------------------------------------

void
ASTVisitor::visit(const ASTInferenceGroup& inference_group)
{
  previsit(inference_group);

  for (const auto& environment_defn : inference_group.environment_defns())
  {
    visit(environment_defn);
  }

  for (const auto& inference_defn : inference_group.inference_defns())
  {
    visit(inference_defn);
  }

  postvisit(inference_group);
}

// -----------------------------------------------------------------------------

void
ASTVisitor::visit(const ASTEnvironmentDefn& environment_defn)
{
  previsit(environment_defn);
  postvisit(environment_defn);
}

// -----------------------------------------------------------------------------

void
ASTVisitor::visit(const ASTInferenceDefn& inference_defn)
{
  previsit(inference_defn);

  // Global decls.
  for (const auto& decl : inference_defn.global_decls())
  {
    visit(decl);
  }

  // Arguments.
  for (const auto& argument : inference_defn.arguments())
  {
    visit(argument);
  }

  // Premise defns.
  for (const auto& premise_defn : inference_defn.premise_defns())
  {
    visit(premise_defn);
  }

  // Proposition defn.
  visit(inference_defn.proposition_defn());

  postvisit(inference_defn);
}

// -----------------------------------------------------------------------------

void
ASTVisitor::visit(const ASTPremiseDefn& premise_defn)
{
  previsit(premise_defn);
  postvisit(premise_defn);
}

// -----------------------------------------------------------------------------

void
ASTVisitor::visit(const ASTGlobalDecl& global_decl)
{
  previsit(global_decl);
  postvisit(global_decl);
}

// -----------------------------------------------------------------------------

void
ASTVisitor::visit(const ASTInferenceArgument& argument)
{
  previsit(argument);
  postvisit(argument);
}

// -----------------------------------------------------------------------------

void
ASTVisitor::visit(const ASTPropositionDefn& proposition_defn)
{
  previsit(proposition_defn);

  visit(proposition_defn.target());

  postvisit(proposition_defn);
}

// -----------------------------------------------------------------------------

void
ASTVisitor::visit(const ASTDeductionTarget& deduction_target)
{
  previsit(deduction_target);
  postvisit(deduction_target);
}

// -----------------------------------------------------------------------------

/* virtual */
void
ASTVisitor::previsit(const ASTModule&)
{
  // Do nothing here.
}

// -----------------------------------------------------------------------------

/* virtual */
void
ASTVisitor::postvisit(const ASTModule&)
{
  // Do nothing here.
}

// -----------------------------------------------------------------------------

/* virtual */
void ASTVisitor::previsit(const ASTInferenceGroup&)
{
  // Do nothing here.
}

// -----------------------------------------------------------------------------

/* virtual */
void ASTVisitor::postvisit(const ASTInferenceGroup&)
{
  // Do nothing here.
}

// -----------------------------------------------------------------------------

/* virtual */
void
ASTVisitor::previsit(const ASTEnvironmentDefn&)
{
  // Do nothing here.
}

// -----------------------------------------------------------------------------

/* virtual */
void
ASTVisitor::postvisit(const ASTEnvironmentDefn&)
{
  // Do nothing here.
}

// -----------------------------------------------------------------------------

/* virtual */
void
ASTVisitor::previsit(const ASTInferenceDefn&)
{
  // Do nothing here.
}

// -----------------------------------------------------------------------------

/* virtual */
void
ASTVisitor::postvisit(const ASTInferenceDefn&)
{
  // Do nothing here.
}

// -----------------------------------------------------------------------------

/* virtual */
void ASTVisitor::previsit(const ASTGlobalDecl&)
{
  // Do nothing here.
}

// -----------------------------------------------------------------------------

/* virtual */
void ASTVisitor::postvisit(const ASTGlobalDecl&)
{
  // Do nothing here.
}

// -----------------------------------------------------------------------------

/* virtual */
void ASTVisitor::previsit(const ASTInferenceArgument&)
{
  // Do nothing here.
}

// -----------------------------------------------------------------------------

/* virtual */
void ASTVisitor::postvisit(const ASTInferenceArgument&)
{
  // Do nothing here.
}

// -----------------------------------------------------------------------------

/* virtual */
void ASTVisitor::previsit(const ASTPremiseDefn&)
{
  // Do nothing here.
}

// -----------------------------------------------------------------------------

/* virtual */
void ASTVisitor::postvisit(const ASTPremiseDefn&)
{
  // Do nothing here.
}

// -----------------------------------------------------------------------------

/* virtual */
void ASTVisitor::previsit(const ASTPropositionDefn&)
{
  // Do nothing here.
}

// -----------------------------------------------------------------------------

/* virtual */
void ASTVisitor::postvisit(const ASTPropositionDefn&)
{
  // Do nothing here.
}

// -----------------------------------------------------------------------------

/* virtual */
void ASTVisitor::previsit(const ASTDeductionTarget&)
{
  // Do nothing here.
}

// -----------------------------------------------------------------------------

/* virtual */
void ASTVisitor::postvisit(const ASTDeductionTarget&)
{
  // Do nothing here.
}

// -----------------------------------------------------------------------------
