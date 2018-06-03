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
operator==(const ASTDeductionTarget& lhs, const ASTDeductionTarget& rhs)
{
  if (lhs.is_type<ASTDeductionTargetSingular>() &&
      rhs.is_type<ASTDeductionTargetSingular>())
  {
    const ASTDeductionTargetSingular& lhs_val = lhs.value<ASTDeductionTargetSingular>();
    const ASTDeductionTargetSingular& rhs_val = rhs.value<ASTDeductionTargetSingular>();
    return lhs_val == rhs_val;
  }
  else if (lhs.is_type<ASTDeductionTargetArray>() &&
           rhs.is_type<ASTDeductionTargetArray>())
  {
    const ASTDeductionTargetArray& lhs_val = lhs.value<ASTDeductionTargetArray>();
    const ASTDeductionTargetArray& rhs_val = rhs.value<ASTDeductionTargetArray>();
    return lhs_val == rhs_val;
  }
  else if (lhs.is_type<ASTDeductionTargetComputed>() ||
           rhs.is_type<ASTDeductionTargetComputed>())
  {
    // NOTE: here we consider if either is a computed deduction target,
    // then the two are potentially equivalent.
    return true;
  }

  return false;
}

// -----------------------------------------------------------------------------

bool operator==(const ASTDeductionTargetSingular& lhs,
                const ASTDeductionTargetSingular& rhs)
{
  return lhs.name() == rhs.name();
}

// -----------------------------------------------------------------------------

bool operator==(const ASTDeductionTargetArray& lhs,
                const ASTDeductionTargetArray& rhs)
{
  if (lhs.name() != rhs.name())
  {
    return false;
  }
  else if (lhs.has_size_literal() && rhs.has_size_literal())
  {
    return lhs.size_literal() == rhs.size_literal();
  }

  return true;
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
