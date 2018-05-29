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
    visit(inference_group, module);
  }

  postvisit(module);
}

// -----------------------------------------------------------------------------

void
ASTVisitor::visit(const ASTInferenceGroup& inference_group,
                  const ASTModule& parent)
{
  previsit(inference_group, parent);

  for (const auto& environment_defn : inference_group.environment_defns())
  {
    visit(environment_defn, inference_group);
  }

  for (const auto& inference_defn : inference_group.inference_defns())
  {
    visit(inference_defn, inference_group);
  }

  postvisit(inference_group, parent);
}

// -----------------------------------------------------------------------------

void
ASTVisitor::visit(const ASTEnvironmentDefn& environment_defn,
                  const ASTInferenceGroup& parent)
{
  previsit(environment_defn, parent);
  postvisit(environment_defn, parent);
}

// -----------------------------------------------------------------------------

void
ASTVisitor::visit(const ASTInferenceDefn& inference_defn,
                  const ASTInferenceGroup& parent)
{
  previsit(inference_defn, parent);

  // Global decls.
  for (const auto& decl : inference_defn.global_decls())
  {
    visit(decl, inference_defn);
  }

  // Arguments.
  for (const auto& argument : inference_defn.arguments())
  {
    visit(argument, inference_defn);
  }

  // Premise defns.
  for (const auto& premise_defn : inference_defn.premise_defns())
  {
    visit(premise_defn, inference_defn);
  }

  // Proposition defn.
  visit(inference_defn.proposition_defn(), inference_defn);

  postvisit(inference_defn, parent);
}

// -----------------------------------------------------------------------------

void
ASTVisitor::visit(const ASTPremiseDefn& premise_defn,
                  const ASTInferenceDefn& parent)
{
  previsit(premise_defn, parent);
  postvisit(premise_defn, parent);
}

// -----------------------------------------------------------------------------

void
ASTVisitor::visit(const ASTGlobalDecl& global_decl,
                  const ASTInferenceDefn& parent)
{
  previsit(global_decl, parent);
  postvisit(global_decl, parent);
}

// -----------------------------------------------------------------------------

void
ASTVisitor::visit(const ASTInferenceArgument& argument,
                  const ASTInferenceDefn& parent)
{
  previsit(argument, parent);
  postvisit(argument, parent);
}

// -----------------------------------------------------------------------------

void
ASTVisitor::visit(const ASTPropositionDefn& proposition_defn,
                  const ASTInferenceDefn& parent)
{
  previsit(proposition_defn, parent);

  visit(proposition_defn.target(), proposition_defn);

  postvisit(proposition_defn, parent);
}

// -----------------------------------------------------------------------------

void
ASTVisitor::visit(const ASTDeductionTarget& deduction_target,
                  const ASTPropositionDefn& parent)
{
  previsit(deduction_target, parent);
  postvisit(deduction_target, parent);
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
void
ASTVisitor::previsit(const ASTInferenceGroup&, const ASTModule&)
{
  // Do nothing here.
}

// -----------------------------------------------------------------------------

/* virtual */
void
ASTVisitor::postvisit(const ASTInferenceGroup&, const ASTModule&)
{
  // Do nothing here.
}

// -----------------------------------------------------------------------------

/* virtual */
void
ASTVisitor::previsit(const ASTEnvironmentDefn&, const ASTInferenceGroup&)
{
  // Do nothing here.
}

// -----------------------------------------------------------------------------

/* virtual */
void
ASTVisitor::postvisit(const ASTEnvironmentDefn&, const ASTInferenceGroup&)
{
  // Do nothing here.
}

// -----------------------------------------------------------------------------

/* virtual */
void
ASTVisitor::previsit(const ASTInferenceDefn&, const ASTInferenceGroup&)
{
  // Do nothing here.
}

// -----------------------------------------------------------------------------

/* virtual */
void
ASTVisitor::postvisit(const ASTInferenceDefn&, const ASTInferenceGroup&)
{
  // Do nothing here.
}

// -----------------------------------------------------------------------------

/* virtual */
void
ASTVisitor::previsit(const ASTGlobalDecl&, const ASTInferenceDefn&)
{
  // Do nothing here.
}

// -----------------------------------------------------------------------------

/* virtual */
void
ASTVisitor::postvisit(const ASTGlobalDecl&, const ASTInferenceDefn&)
{
  // Do nothing here.
}

// -----------------------------------------------------------------------------

/* virtual */
void
ASTVisitor::previsit(const ASTInferenceArgument&, const ASTInferenceDefn&)
{
  // Do nothing here.
}

// -----------------------------------------------------------------------------

/* virtual */
void
ASTVisitor::postvisit(const ASTInferenceArgument&, const ASTInferenceDefn&)
{
  // Do nothing here.
}

// -----------------------------------------------------------------------------

/* virtual */
void
ASTVisitor::previsit(const ASTPremiseDefn&, const ASTInferenceDefn&)
{
  // Do nothing here.
}

// -----------------------------------------------------------------------------

/* virtual */
void
ASTVisitor::postvisit(const ASTPremiseDefn&, const ASTInferenceDefn&)
{
  // Do nothing here.
}

// -----------------------------------------------------------------------------

/* virtual */
void
ASTVisitor::previsit(const ASTPropositionDefn&, const ASTInferenceDefn&)
{
  // Do nothing here.
}

// -----------------------------------------------------------------------------

/* virtual */
void
ASTVisitor::postvisit(const ASTPropositionDefn&, const ASTInferenceDefn&)
{
  // Do nothing here.
}

// -----------------------------------------------------------------------------

/* virtual */
void
ASTVisitor::previsit(const ASTDeductionTarget&, const ASTPropositionDefn&)
{
  // Do nothing here.
}

// -----------------------------------------------------------------------------

/* virtual */
void
ASTVisitor::postvisit(const ASTDeductionTarget&, const ASTPropositionDefn&)
{
  // Do nothing here.
}

// -----------------------------------------------------------------------------
