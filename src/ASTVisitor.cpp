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
#include "macros.h"

// -----------------------------------------------------------------------------

#define DEFAULT_RETURN() return true
#define VERIFY(x)                                                              \
  if (!(x))                                                                    \
  return false
#define PREVISIT_AND_VERIFY(x) VERIFY(previsit((x)))
#define POSTVISIT_AND_VERIFY(x) VERIFY(postvisit((x)))
#define VISIT_AND_VERIFY(x) VERIFY(visit((x)))

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

bool
ASTVisitor::visit(const ASTModule& module)
{
  PREVISIT_AND_VERIFY(module);

  for (const auto& inference_group : module.inferenceGroups()) {
    VISIT_AND_VERIFY(inference_group);
  }

  POSTVISIT_AND_VERIFY(module);

  DEFAULT_RETURN();
}

// -----------------------------------------------------------------------------

bool
ASTVisitor::visit(const ASTInferenceGroup& inference_group)
{
  PREVISIT_AND_VERIFY(inference_group);

  for (const auto& environment_defn : inference_group.environmentDefns()) {
    VISIT_AND_VERIFY(environment_defn);
  }

  for (const auto& inference_defn : inference_group.inferenceDefns()) {
    VISIT_AND_VERIFY(inference_defn);
  }

  POSTVISIT_AND_VERIFY(inference_group);

  DEFAULT_RETURN();
}

// -----------------------------------------------------------------------------

bool
ASTVisitor::visit(const ASTEnvironmentDefn& environment_defn)
{
  PREVISIT_AND_VERIFY(environment_defn);
  POSTVISIT_AND_VERIFY(environment_defn);
  DEFAULT_RETURN();
}

// -----------------------------------------------------------------------------

bool
ASTVisitor::visit(const ASTInferenceDefn& inference_defn)
{
  PREVISIT_AND_VERIFY(inference_defn);

  // Global decls.
  for (const auto& decl : inference_defn.globalDecls()) {
    VISIT_AND_VERIFY(decl);
  }

  // Arguments.
  for (const auto& argument : inference_defn.arguments()) {
    VISIT_AND_VERIFY(argument);
  }

  // Premise defns.
  for (const auto& premise_defn : inference_defn.premiseDefns()) {
    VISIT_AND_VERIFY(premise_defn);
  }

  // Proposition defn.
  VISIT_AND_VERIFY(inference_defn.propositionDefn());

  POSTVISIT_AND_VERIFY(inference_defn);

  DEFAULT_RETURN();
}

// -----------------------------------------------------------------------------

bool
ASTVisitor::visit(const ASTPremiseDefn& premise_defn)
{
  if (premise_defn.is_type<ASTInferencePremiseDefn>()) {
    const ASTInferencePremiseDefn& defn =
        premise_defn.value<ASTInferencePremiseDefn>();
    VISIT_AND_VERIFY(defn);
  } else if (premise_defn.is_type<ASTInferenceEqualityDefn>()) {
    const ASTInferenceEqualityDefn& defn =
        premise_defn.value<ASTInferenceEqualityDefn>();
    VISIT_AND_VERIFY(defn);
  } else {
    ASSERT(0);
  }

  DEFAULT_RETURN();
}

// -----------------------------------------------------------------------------

bool
ASTVisitor::visit(const ASTInferencePremiseDefn& defn)
{
  PREVISIT_AND_VERIFY(defn);
  POSTVISIT_AND_VERIFY(defn);
  DEFAULT_RETURN();
}

// -----------------------------------------------------------------------------

bool
ASTVisitor::visit(const ASTInferenceEqualityDefn& defn)
{
  PREVISIT_AND_VERIFY(defn);

  VISIT_AND_VERIFY(defn.lhs());
  VISIT_AND_VERIFY(defn.rhs());

  POSTVISIT_AND_VERIFY(defn);

  DEFAULT_RETURN();
}

// -----------------------------------------------------------------------------

bool
ASTVisitor::visit(const ASTWhileClause& whileClause)
{
  for (const auto& premise_defn : whileClause.premiseDefns()) {
    VISIT_AND_VERIFY(premise_defn);
  }

  DEFAULT_RETURN();
}

// -----------------------------------------------------------------------------

bool
ASTVisitor::visit(const ASTGlobalDecl& global_decl)
{
  PREVISIT_AND_VERIFY(global_decl);
  POSTVISIT_AND_VERIFY(global_decl);
  DEFAULT_RETURN();
}

// -----------------------------------------------------------------------------

bool
ASTVisitor::visit(const ASTInferenceArgument& argument)
{
  PREVISIT_AND_VERIFY(argument);
  POSTVISIT_AND_VERIFY(argument);
  DEFAULT_RETURN();
}

// -----------------------------------------------------------------------------

bool
ASTVisitor::visit(const ASTPropositionDefn& propositionDefn)
{
  PREVISIT_AND_VERIFY(propositionDefn);

  VISIT_AND_VERIFY(propositionDefn.target());

  POSTVISIT_AND_VERIFY(propositionDefn);

  DEFAULT_RETURN();
}

// -----------------------------------------------------------------------------

bool
ASTVisitor::visit(const ASTDeductionTarget& deductionTarget)
{
  PREVISIT_AND_VERIFY(deductionTarget);

  if (deductionTarget.is_type<ASTDeductionTargetSingular>()) {
    const ASTDeductionTargetSingular& target =
        deductionTarget.value<ASTDeductionTargetSingular>();
    VISIT_AND_VERIFY(target);
  } else if (deductionTarget.is_type<ASTDeductionTargetArray>()) {
    const ASTDeductionTargetArray& target =
        deductionTarget.value<ASTDeductionTargetArray>();
    VISIT_AND_VERIFY(target);
  } else if (deductionTarget.is_type<ASTDeductionTargetComputed>()) {
    const ASTDeductionTargetComputed& target =
        deductionTarget.value<ASTDeductionTargetComputed>();
    VISIT_AND_VERIFY(target);
  } else {
    ASSERT(0);
  }

  POSTVISIT_AND_VERIFY(deductionTarget);

  DEFAULT_RETURN();
}

// -----------------------------------------------------------------------------

bool
ASTVisitor::visit(const ASTDeductionTargetSingular& deductionTarget)
{
  PREVISIT_AND_VERIFY(deductionTarget);
  POSTVISIT_AND_VERIFY(deductionTarget);
  DEFAULT_RETURN();
}

// -----------------------------------------------------------------------------

bool
ASTVisitor::visit(const ASTDeductionTargetArray& deductionTarget)
{
  PREVISIT_AND_VERIFY(deductionTarget);
  POSTVISIT_AND_VERIFY(deductionTarget);
  DEFAULT_RETURN();
}

// -----------------------------------------------------------------------------

bool
ASTVisitor::visit(const ASTDeductionTargetComputed& deductionTarget)
{
  PREVISIT_AND_VERIFY(deductionTarget);
  POSTVISIT_AND_VERIFY(deductionTarget);
  DEFAULT_RETURN();
}

// -----------------------------------------------------------------------------

/* virtual */
bool
ASTVisitor::previsit(const ASTModule&)
{
  DEFAULT_RETURN();
}

// -----------------------------------------------------------------------------

/* virtual */
bool
ASTVisitor::postvisit(const ASTModule&)
{
  DEFAULT_RETURN();
}

// -----------------------------------------------------------------------------

/* virtual */
bool
ASTVisitor::previsit(const ASTInferenceGroup&)
{
  DEFAULT_RETURN();
}

// -----------------------------------------------------------------------------

/* virtual */
bool
ASTVisitor::postvisit(const ASTInferenceGroup&)
{
  DEFAULT_RETURN();
}

// -----------------------------------------------------------------------------

/* virtual */
bool
ASTVisitor::previsit(const ASTEnvironmentDefn&)
{
  DEFAULT_RETURN();
}

// -----------------------------------------------------------------------------

/* virtual */
bool
ASTVisitor::postvisit(const ASTEnvironmentDefn&)
{
  DEFAULT_RETURN();
}

// -----------------------------------------------------------------------------

/* virtual */
bool
ASTVisitor::previsit(const ASTInferenceDefn&)
{
  DEFAULT_RETURN();
}

// -----------------------------------------------------------------------------

/* virtual */
bool
ASTVisitor::postvisit(const ASTInferenceDefn&)
{
  DEFAULT_RETURN();
}

// -----------------------------------------------------------------------------

/* virtual */
bool
ASTVisitor::previsit(const ASTGlobalDecl&)
{
  DEFAULT_RETURN();
}

// -----------------------------------------------------------------------------

/* virtual */
bool
ASTVisitor::postvisit(const ASTGlobalDecl&)
{
  DEFAULT_RETURN();
}

// -----------------------------------------------------------------------------

/* virtual */
bool
ASTVisitor::previsit(const ASTInferenceArgument&)
{
  DEFAULT_RETURN();
}

// -----------------------------------------------------------------------------

/* virtual */
bool
ASTVisitor::postvisit(const ASTInferenceArgument&)
{
  DEFAULT_RETURN();
}

// -----------------------------------------------------------------------------

/* virtual */
bool
ASTVisitor::previsit(const ASTInferencePremiseDefn&)
{
  DEFAULT_RETURN();
}

// -----------------------------------------------------------------------------

/* virtual */
bool
ASTVisitor::postvisit(const ASTInferencePremiseDefn&)
{
  DEFAULT_RETURN();
}

// -----------------------------------------------------------------------------

/* virtual */
bool
ASTVisitor::previsit(const ASTInferenceEqualityDefn&)
{
  DEFAULT_RETURN();
}

// -----------------------------------------------------------------------------

/* virtual */
bool
ASTVisitor::postvisit(const ASTInferenceEqualityDefn&)
{
  DEFAULT_RETURN();
}

// -----------------------------------------------------------------------------

/* virtual */
bool
ASTVisitor::previsit(const ASTPropositionDefn&)
{
  DEFAULT_RETURN();
}

// -----------------------------------------------------------------------------

/* virtual */
bool
ASTVisitor::postvisit(const ASTPropositionDefn&)
{
  DEFAULT_RETURN();
}

// -----------------------------------------------------------------------------

/* virtual */
bool
ASTVisitor::previsit(const ASTDeductionTarget&)
{
  DEFAULT_RETURN();
}

// -----------------------------------------------------------------------------

/* virtual */
bool
ASTVisitor::postvisit(const ASTDeductionTarget&)
{
  DEFAULT_RETURN();
}

// -----------------------------------------------------------------------------

/* virtual */
bool
ASTVisitor::previsit(const ASTDeductionTargetSingular&)
{
  DEFAULT_RETURN();
}

// -----------------------------------------------------------------------------

/* virtual */
bool
ASTVisitor::postvisit(const ASTDeductionTargetSingular&)
{
  DEFAULT_RETURN();
}

// -----------------------------------------------------------------------------

/* virtual */
bool
ASTVisitor::previsit(const ASTDeductionTargetArray&)
{
  DEFAULT_RETURN();
}

// -----------------------------------------------------------------------------

/* virtual */
bool
ASTVisitor::postvisit(const ASTDeductionTargetArray&)
{
  DEFAULT_RETURN();
}

// -----------------------------------------------------------------------------

/* virtual */
bool
ASTVisitor::previsit(const ASTDeductionTargetComputed&)
{
  DEFAULT_RETURN();
}

// -----------------------------------------------------------------------------

/* virtual */
bool
ASTVisitor::postvisit(const ASTDeductionTargetComputed&)
{
  DEFAULT_RETURN();
}

// -----------------------------------------------------------------------------
