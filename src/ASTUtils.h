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

#include <string>
#include <unordered_map>
#include <unordered_set>

typedef std::unordered_map<std::string, const ASTDeductionTarget*> TargetTable;

typedef std::unordered_set<std::string> SymbolSet;

class ASTUtils
{
public:
  static std::string CanonicalizeASTIdentifiable(const ASTIdentifiable&);

  static bool AreTargetsCompatible(const ASTDeductionTarget&,
                                   const ASTDeductionTarget&);

  static bool AreTargetsCompatible(const ASTDeductionTargetSingular&,
                                   const ASTDeductionTargetSingular&);

  static bool AreTargetsCompatible(const ASTDeductionTargetArray&,
                                   const ASTDeductionTargetArray&);

  static const std::string& GetRootOfASTIdentifiable(const ASTIdentifiable&);

  static void AddTargetToTable(const ASTDeductionTarget&, TargetTable*);

  static bool HasCompatibleTargetInTable(const ASTDeductionTarget&,
                                         const TargetTable&);

  static bool HasIncompatibleTargetInTable(const ASTDeductionTarget&,
                                           const TargetTable&);
};
