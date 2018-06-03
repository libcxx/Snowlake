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

#include "ast_util.h"
#include "ast.h"
#include <sstream>


// -----------------------------------------------------------------------------

std::string
canonicalize_ASTIdentifiable(const ASTIdentifiable& identifiable)
{
  std::stringstream stream;

  const auto& identifiers = identifiable.identifiers();

  if (!identifiers.empty())
  {
    stream << identifiers.front().value();
  }

  for (size_t i = 1; i < identifiers.size(); ++i)
  {
    stream << '.' << identifiers[i].value();
  }

  return stream.str();
}

// -----------------------------------------------------------------------------

bool
are_targets_compatible(const ASTDeductionTarget& lhs,
                       const ASTDeductionTarget& rhs)
{
  if (lhs.is_type<ASTDeductionTargetSingular>() &&
      rhs.is_type<ASTDeductionTargetSingular>())
  {
    const ASTDeductionTargetSingular& lhs_val = lhs.value<ASTDeductionTargetSingular>();
    const ASTDeductionTargetSingular& rhs_val = rhs.value<ASTDeductionTargetSingular>();
    return are_targets_compatible(lhs_val, rhs_val);
  }
  else if (lhs.is_type<ASTDeductionTargetArray>() &&
           rhs.is_type<ASTDeductionTargetArray>())
  {
    const ASTDeductionTargetArray& lhs_val = lhs.value<ASTDeductionTargetArray>();
    const ASTDeductionTargetArray& rhs_val = rhs.value<ASTDeductionTargetArray>();
    return are_targets_compatible(lhs_val, rhs_val);
  }
  else if (lhs.is_type<ASTDeductionTargetComputed>() ||
           rhs.is_type<ASTDeductionTargetComputed>())
  {
    // NOTE: here we consider if either is a computed deduction target,
    // then the two are potentially equivalent.
    return true;
  }
  else
  {
    // One of them is a target array, and the other one is a singular.
    // If the array target has a size literal, meaning that it
    // references a singular element in it, then they are considered
    // compatible.
    if (lhs.is_type<ASTDeductionTargetArray>())
    {
      const auto& lhs_val = lhs.value<ASTDeductionTargetArray>();
      return lhs_val.has_size_literal();
    }
    else
    {
      ASSERT(rhs.is_type<ASTDeductionTargetArray>());
      const auto& rhs_val = rhs.value<ASTDeductionTargetArray>();
      return rhs_val.has_size_literal();
    }
  }
}

// -----------------------------------------------------------------------------

bool
are_targets_compatible(const ASTDeductionTargetSingular& /* lhs */,
                       const ASTDeductionTargetSingular& /* rhs */)
{
  // Singular targets should be compatible.
  return true;
}

// -----------------------------------------------------------------------------

bool
are_targets_compatible(const ASTDeductionTargetArray& lhs,
                       const ASTDeductionTargetArray& rhs)
{
  if (lhs.has_size_literal() && rhs.has_size_literal())
  {
    return true;
  }
  else if (!lhs.has_size_literal() && !rhs.has_size_literal())
  {
    // In range clause.
    // E.g. "ArgumentsTypes[] <= ParameterTypes[] inrange 0..1..ParameterTypes;"
    return true;
  }

  return false;
}

// -----------------------------------------------------------------------------

const std::string&
get_root_of_ASTIdentifiable(const ASTIdentifiable& identifiable)
{
  static const std::string nullvalue;

  const auto& identifiers = identifiable.identifiers();
  if (identifiers.empty())
  {
    return nullvalue;
  }

  return identifiers.front().value();
}

// -----------------------------------------------------------------------------

void
add_target_to_table(const ASTDeductionTarget& target, TargetTable* tbl)
{
  if (target.is_type<ASTDeductionTargetSingular>())
  {
    const auto& value = target.value<ASTDeductionTargetSingular>();
    tbl->operator[](value.name()) = &target;
  }
  else if (target.is_type<ASTDeductionTargetArray>())
  {
    const auto& value = target.value<ASTDeductionTargetArray>();
    tbl->operator[](value.name()) = &target;
  }
}

// -----------------------------------------------------------------------------

bool
has_compatible_target_in_table(const ASTDeductionTarget& target,
                               const TargetTable& tbl)
{
  if (target.is_type<ASTDeductionTargetSingular>())
  {
    const auto& value = target.value<ASTDeductionTargetSingular>();
    if (tbl.count(value.name()) > 0)
    {
      const ASTDeductionTarget* existing_value = tbl.at(value.name());
      return existing_value->is_type<ASTDeductionTargetSingular>();
    }
    else
    {
      return false;
    }
  }
  else if (target.is_type<ASTDeductionTargetArray>())
  {
    const auto& value = target.value<ASTDeductionTargetArray>();
    if (tbl.count(value.name()) > 0)
    {
      const ASTDeductionTarget* existing_value = tbl.at(value.name());
      return existing_value->is_type<ASTDeductionTargetArray>();
    }
    else
    {
      return false;
    }
  }
  else if (target.is_type<ASTDeductionTargetComputed>())
  {
    // We do not know the type of the target if it's computed.
    // So have to return true here.
    return true;
  }
  else
  {
    ASSERT(0); // Should not be reachable.
  }
}

// -----------------------------------------------------------------------------

bool
has_incompatible_target_in_table(const ASTDeductionTarget& target,
                                 const TargetTable& tbl)
{
  if (target.is_type<ASTDeductionTargetSingular>())
  {
    const auto& value = target.value<ASTDeductionTargetSingular>();
    if (tbl.count(value.name()) > 0)
    {
      const ASTDeductionTarget* existing_value = tbl.at(value.name());
      return !existing_value->is_type<ASTDeductionTargetSingular>();
    }
  }
  else if (target.is_type<ASTDeductionTargetArray>())
  {
    const auto& value = target.value<ASTDeductionTargetArray>();
    if (tbl.count(value.name()) > 0)
    {
      const ASTDeductionTarget* existing_value = tbl.at(value.name());
      return !existing_value->is_type<ASTDeductionTargetArray>();
    }
  }
  else if (target.is_type<ASTDeductionTargetComputed>())
  {
    // We do not know the type of the target if it's computed.
    // So have to return false here.
    return false;
  }
  return false;
}

// -----------------------------------------------------------------------------
