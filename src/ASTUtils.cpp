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

#include "ASTUtils.h"

#include "ast.h"

#include <sstream>

// -----------------------------------------------------------------------------

std::string
ASTUtils::canonicalizeASTIdentifiable(const ASTIdentifiable& identifiable)
{
  std::stringstream stream;

  const auto& identifiers = identifiable.identifiers();

  if (!identifiers.empty()) {
    stream << identifiers.front().value();
  }

  for (size_t i = 1; i < identifiers.size(); ++i) {
    stream << '.' << identifiers[i].value();
  }

  return stream.str();
}

// -----------------------------------------------------------------------------

bool
ASTUtils::areTargetsCompatible(const ASTDeductionTarget& lhs,
                               const ASTDeductionTarget& rhs)
{
  if (lhs.isType<ASTDeductionTargetSingular>() &&
      rhs.isType<ASTDeductionTargetSingular>()) {
    const ASTDeductionTargetSingular& lhs_val =
        lhs.value<ASTDeductionTargetSingular>();
    const ASTDeductionTargetSingular& rhs_val =
        rhs.value<ASTDeductionTargetSingular>();
    return areTargetsCompatible(lhs_val, rhs_val);
  } else if (lhs.isType<ASTDeductionTargetArray>() &&
             rhs.isType<ASTDeductionTargetArray>()) {
    const ASTDeductionTargetArray& lhs_val =
        lhs.value<ASTDeductionTargetArray>();
    const ASTDeductionTargetArray& rhs_val =
        rhs.value<ASTDeductionTargetArray>();
    return areTargetsCompatible(lhs_val, rhs_val);
  } else if (lhs.isType<ASTDeductionTargetComputed>() ||
             rhs.isType<ASTDeductionTargetComputed>()) {
    // NOTE: here we consider if either is a computed deduction target,
    // then the two are potentially equivalent.
    return true;
  } else {
    // One of them is a target array, and the other one is a singular.
    // If the array target has a size literal, meaning that it
    // references a singular element in it, then they are considered
    // compatible.
    if (lhs.isType<ASTDeductionTargetArray>()) {
      const auto& lhs_val = lhs.value<ASTDeductionTargetArray>();
      return lhs_val.hasSizeLiteral();
    } else {
      ASSERT(rhs.isType<ASTDeductionTargetArray>());
      const auto& rhs_val = rhs.value<ASTDeductionTargetArray>();
      return rhs_val.hasSizeLiteral();
    }
  }
}

// -----------------------------------------------------------------------------

bool
ASTUtils::areTargetsCompatible(const ASTDeductionTargetSingular& /* lhs */,
                               const ASTDeductionTargetSingular& /* rhs */)
{
  // Singular targets should be compatible.
  return true;
}

// -----------------------------------------------------------------------------

bool
ASTUtils::areTargetsCompatible(const ASTDeductionTargetArray& lhs,
                               const ASTDeductionTargetArray& rhs)
{
  if (lhs.hasSizeLiteral() && rhs.hasSizeLiteral()) {
    return true;
  } else if (!lhs.hasSizeLiteral() && !rhs.hasSizeLiteral()) {
    // In range clause.
    // E.g. "ArgumentsTypes[] <= ParameterTypes[] inrange 0..1..ParameterTypes;"
    return true;
  }

  return false;
}

// -----------------------------------------------------------------------------

const std::string&
ASTUtils::getRootOfASTIdentifiable(const ASTIdentifiable& identifiable)
{
  static const std::string nullvalue;

  const auto& identifiers = identifiable.identifiers();
  if (identifiers.empty()) {
    return nullvalue;
  }

  return identifiers.front().value();
}

// -----------------------------------------------------------------------------

void
ASTUtils::addTargetToTable(const ASTDeductionTarget& target, TargetTable* tbl)
{
  if (target.isType<ASTDeductionTargetSingular>()) {
    const auto& value = target.value<ASTDeductionTargetSingular>();
    tbl->operator[](value.name()) = &target;
  } else if (target.isType<ASTDeductionTargetArray>()) {
    const auto& value = target.value<ASTDeductionTargetArray>();
    tbl->operator[](value.name()) = &target;
  }
}

// -----------------------------------------------------------------------------

bool
ASTUtils::hasCompatibleTargetInTable(const ASTDeductionTarget& target,
                                     const TargetTable& tbl)
{
  if (target.isType<ASTDeductionTargetSingular>()) {
    const auto& value = target.value<ASTDeductionTargetSingular>();
    if (tbl.count(value.name()) > 0) {
      const ASTDeductionTarget* existingValue = tbl.at(value.name());
      return existingValue->isType<ASTDeductionTargetSingular>();
    } else {
      return false;
    }
  } else if (target.isType<ASTDeductionTargetArray>()) {
    const auto& value = target.value<ASTDeductionTargetArray>();
    if (tbl.count(value.name()) > 0) {
      const ASTDeductionTarget* existingValue = tbl.at(value.name());
      if (existingValue->isType<ASTDeductionTargetArray>()) {
        const auto& existingTarget =
            existingValue->value<ASTDeductionTargetArray>();
        if (value.hasSizeLiteral() && !existingTarget.hasSizeLiteral()) {
          return false;
        } else if (!value.hasSizeLiteral() && existingTarget.hasSizeLiteral()) {
          return false;
        } else if (value.hasSizeLiteral() && existingTarget.hasSizeLiteral()) {
          return value.sizeLiteral() == existingTarget.sizeLiteral();
        } else // Neither has size literal.
        {
          return true;
        }
      } else {
        return false;
      }
    } else {
      return false;
    }
  } else if (target.isType<ASTDeductionTargetComputed>()) {
    // We do not know the type of the target if it's computed.
    // So have to return true here.
    return true;
  } else {
    ASSERT(0); // Should not be reachable.
  }
}

// -----------------------------------------------------------------------------

bool
ASTUtils::hasIncompatibleTargetInTable(const ASTDeductionTarget& target,
                                       const TargetTable& tbl)
{
  if (target.isType<ASTDeductionTargetSingular>()) {
    const auto& value = target.value<ASTDeductionTargetSingular>();
    if (tbl.count(value.name()) > 0) {
      const ASTDeductionTarget* existingValue = tbl.at(value.name());
      return !existingValue->isType<ASTDeductionTargetSingular>();
    }
  } else if (target.isType<ASTDeductionTargetArray>()) {
    const auto& value = target.value<ASTDeductionTargetArray>();
    if (tbl.count(value.name()) > 0) {
      const ASTDeductionTarget* existingValue = tbl.at(value.name());
      if (existingValue->isType<ASTDeductionTargetArray>()) {
        const auto& existingTarget =
            existingValue->value<ASTDeductionTargetArray>();
        if (value.hasSizeLiteral() && !existingTarget.hasSizeLiteral()) {
          return true;
        } else if (!value.hasSizeLiteral() && existingTarget.hasSizeLiteral()) {
          return true;
        } else if (value.hasSizeLiteral() && existingTarget.hasSizeLiteral()) {
          return value.sizeLiteral() != existingTarget.sizeLiteral();
        } else // Neither has size literal.
        {
          return false;
        }
      } else {
        return true;
      }
    }
  } else if (target.isType<ASTDeductionTargetComputed>()) {
    // We do not know the type of the target if it's computed.
    // So have to return false here.
    return false;
  }
  return false;
}

// -----------------------------------------------------------------------------
