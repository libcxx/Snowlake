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
#include "optional.h"
#include "variant.h"

#include <cstdlib>
#include <string>
#include <vector>

// -----------------------------------------------------------------------------

typedef std::string StringType;
typedef uint64_t IntegerType;

// -----------------------------------------------------------------------------

typedef std::vector<ASTIdentifier> ASTIdentifierList;
typedef std::vector<ASTDeductionTarget> ASTDeductionTargetList;
typedef std::vector<ASTPremiseDefn> ASTPremiseDefnList;
typedef std::vector<ASTInferenceArgument> ASTInferenceArgumentList;
typedef std::vector<ASTGlobalDecl> ASTGlobalDeclList;
typedef std::vector<ASTEnvironmentDefn> ASTEnvironmentDefnList;
typedef std::vector<ASTInferenceDefn> ASTInferenceDefnList;
typedef std::vector<ASTInferenceGroup> ASTInferenceGroupList;

// -----------------------------------------------------------------------------

enum class EqualityOperator : uint8_t
{
  OPERATOR_EQ,
  OPERATOR_NEQ,
  OPERATOR_LT,
  OPERATOR_LTE
};

// -----------------------------------------------------------------------------

class ASTNode
{
};

// -----------------------------------------------------------------------------

class ASTIdentifier : public ASTNode
{
public:
  ASTIdentifier()
    : _value()
  {
  }

  ASTIdentifier(StringType&& value)
    : _value(value)
  {
  }

  const StringType& value() const
  {
    return _value;
  }

private:
  StringType _value;
};

// -----------------------------------------------------------------------------

class ASTIdentifiable : public ASTNode
{
public:
  ASTIdentifiable()
    : _identifiers()
  {
  }

  ASTIdentifiable(ASTIdentifierList&& identifiers)
    : _identifiers(identifiers)
  {
  }

  void add(const ASTIdentifier& element)
  {
    _identifiers.emplace_back(element);
  }

  const ASTIdentifierList& identifiers() const
  {
    return _identifiers;
  }

private:
  ASTIdentifierList _identifiers;
};

// -----------------------------------------------------------------------------

class ASTDeductionTargetSingular
{
public:
  ASTDeductionTargetSingular()
    : _name()
  {
  }

  explicit ASTDeductionTargetSingular(StringType&& name)
    : _name(name)
  {
  }

  const StringType& name() const
  {
    return _name;
  }

private:
  StringType _name;
};

// -----------------------------------------------------------------------------

class ASTDeductionTargetArray
{
public:
  ASTDeductionTargetArray()
    : _name()
    , _arraySize()
  {
  }

  explicit ASTDeductionTargetArray(StringType&& name)
    : _name(name)
    , _arraySize()
  {
  }

  ASTDeductionTargetArray(StringType&& name, IntegerType arraySize)
    : _name(name)
    , _arraySize(arraySize)
  {
  }

  const StringType& name() const
  {
    return _name;
  }

  bool hasSizeLiteral() const
  {
    return _arraySize.has_value();
  }

  IntegerType sizeLiteral() const
  {
    return _arraySize.value();
  }

private:
  StringType _name;
  sl::optional<IntegerType> _arraySize;
};

// -----------------------------------------------------------------------------

class ASTDeductionTargetComputed
{
public:
  ASTDeductionTargetComputed()
    : _name()
    , _arguments()
  {
  }

  ASTDeductionTargetComputed(StringType&& name,
                             ASTDeductionTargetList&& arguments)
    : _name(name)
    , _arguments(arguments)
  {
  }

  const StringType& name() const
  {
    return _name;
  }

  const ASTDeductionTargetList& arguments() const
  {
    return _arguments;
  }

private:
  StringType _name;
  ASTDeductionTargetList _arguments;
};

// -----------------------------------------------------------------------------

class ASTDeductionTarget : public ASTNode
{
public:
  ASTDeductionTarget()
    : _value()
  {
  }

  ASTDeductionTarget(ASTDeductionTargetSingular&& value)
    : _value(value)
  {
  }

  ASTDeductionTarget(ASTDeductionTargetArray&& value)
    : _value(value)
  {
  }

  ASTDeductionTarget(ASTDeductionTargetComputed&& value)
    : _value(value)
  {
  }

  template <typename U>
  bool isType() const
  {
    return _value.template is<U>();
  }

  template <typename U>
  const U& value() const
  {
    return _value.template get<U>();
  }

private:
  sl::variant::variant<ASTDeductionTargetSingular, ASTDeductionTargetArray,
                       ASTDeductionTargetComputed>
      _value;
};

// -----------------------------------------------------------------------------

class ASTPropositionDefn
{
public:
  ASTPropositionDefn()
    : _target()
  {
  }

  ASTPropositionDefn(ASTDeductionTarget&& target)
    : _target(target)
  {
  }

  const ASTDeductionTarget& target() const
  {
    return _target;
  }

private:
  ASTDeductionTarget _target;
};

// -----------------------------------------------------------------------------

class ASTRangeClause : public ASTNode
{
public:
  ASTRangeClause()
    : _lhsIdx(0)
    , _rhsIdx(0)
    , _deductionTarget()
  {
  }

  ASTRangeClause(IntegerType lhsIdx, IntegerType rhsIdx,
                 ASTDeductionTarget&& deductionTarget)
    : _lhsIdx(lhsIdx)
    , _rhsIdx(rhsIdx)
    , _deductionTarget(deductionTarget)
  {
  }

  IntegerType lhsIdx() const
  {
    return _lhsIdx;
  }

  IntegerType rhsIdx() const
  {
    return _rhsIdx;
  }

  const ASTDeductionTarget& deductionTarget() const
  {
    return _deductionTarget;
  }

private:
  IntegerType _lhsIdx;
  IntegerType _rhsIdx;
  ASTDeductionTarget _deductionTarget;
};

// -----------------------------------------------------------------------------

class ASTInferenceEqualityDefn
{
public:
  ASTInferenceEqualityDefn()
    : _lhs()
    , _rhs()
    , _rangeClause()
  {
  }

  ASTInferenceEqualityDefn(ASTDeductionTarget&& lhs, ASTDeductionTarget&& rhs,
                           EqualityOperator oprt)
    : _lhs(lhs)
    , _rhs(rhs)
    , _oprt(oprt)
  {
  }

  ASTInferenceEqualityDefn(ASTDeductionTarget&& lhs, ASTDeductionTarget&& rhs,
                           EqualityOperator oprt, ASTRangeClause&& rangeClause)
    : _lhs(lhs)
    , _rhs(rhs)
    , _oprt(oprt)
    , _rangeClause(rangeClause)
  {
  }

  const ASTDeductionTarget& lhs() const
  {
    return _lhs;
  }

  const ASTDeductionTarget& rhs() const
  {
    return _rhs;
  }

  EqualityOperator oprt() const
  {
    return _oprt;
  }

  bool hasRangeClause() const
  {
    return _rangeClause.has_value();
  }

  const ASTRangeClause& rangeClause() const
  {
    return _rangeClause.value();
  }

private:
  ASTDeductionTarget _lhs;
  ASTDeductionTarget _rhs;
  EqualityOperator _oprt;
  sl::optional<ASTRangeClause> _rangeClause;
};

// -----------------------------------------------------------------------------

class ASTWhileClause : public ASTNode
{
public:
  ASTWhileClause()
    : _premiseDefns()
  {
  }

  explicit ASTWhileClause(ASTPremiseDefnList&& premiseDefns)
    : _premiseDefns(premiseDefns)
  {
  }

  const ASTPremiseDefnList& premiseDefns() const
  {
    return _premiseDefns;
  }

private:
  ASTPremiseDefnList _premiseDefns;
};

// -----------------------------------------------------------------------------

class ASTInferencePremiseDefn
{
public:
  ASTInferencePremiseDefn()
    : _source()
    , _deductionTarget()
    , _whileClause()
  {
  }

  ASTInferencePremiseDefn(ASTIdentifiable&& source,
                          ASTDeductionTarget&& deductionTarget)
    : _source(source)
    , _deductionTarget(deductionTarget)
    , _whileClause()
  {
  }

  ASTInferencePremiseDefn(ASTIdentifiable&& source,
                          ASTDeductionTarget&& deductionTarget,
                          ASTWhileClause&& whileClause)
    : _source(source)
    , _deductionTarget(deductionTarget)
    , _whileClause(whileClause)
  {
  }

  const ASTIdentifiable& source() const
  {
    return _source;
  }

  const ASTDeductionTarget& deductionTarget() const
  {
    return _deductionTarget;
  }

  bool hasWhileClause() const
  {
    return _whileClause.has_value();
  }

  const ASTWhileClause& whileClause() const
  {
    return _whileClause.value();
  }

private:
  ASTIdentifiable _source;
  ASTDeductionTarget _deductionTarget;
  sl::optional<ASTWhileClause> _whileClause;
};

// -----------------------------------------------------------------------------

class ASTPremiseDefn : public ASTNode
{
public:
  ASTPremiseDefn()
    : _value()
  {
  }

  ASTPremiseDefn(ASTInferencePremiseDefn&& defn)
    : _value(defn)
  {
  }

  ASTPremiseDefn(ASTInferenceEqualityDefn&& defn)
    : _value(defn)
  {
  }

  template <class U>
  bool isType() const
  {
    return _value.template is<U>();
  }

  template <class U>
  const U& value() const
  {
    return _value.template get<U>();
  }

private:
  sl::variant::variant<ASTInferencePremiseDefn, ASTInferenceEqualityDefn>
      _value;
};

// -----------------------------------------------------------------------------

class ASTInferenceArgument : public ASTNode
{
public:
  ASTInferenceArgument()
    : _name()
    , _typeName()
  {
  }

  ASTInferenceArgument(StringType&& name, StringType&& typeName)
    : _name(name)
    , _typeName(typeName)
  {
  }

  const StringType& name() const
  {
    return _name;
  }

  const StringType& typeName() const
  {
    return _typeName;
  }

private:
  StringType _name;
  StringType _typeName;
};

// -----------------------------------------------------------------------------

class ASTGlobalDecl : public ASTNode
{
public:
  ASTGlobalDecl()
    : _name()
  {
  }

  ASTGlobalDecl(StringType&& name)
    : _name(name)
  {
  }

  const StringType& name() const
  {
    return _name;
  }

private:
  StringType _name;
};

// -----------------------------------------------------------------------------

class ASTInferenceDefn : public ASTNode
{
public:
  ASTInferenceDefn()
    : _name()
    , _globalDecls()
    , _arguments()
    , _premiseDefns()
    , _propositionDefn()
  {
  }

  ASTInferenceDefn(StringType&& name, ASTGlobalDeclList&& globalDecls,
                   ASTInferenceArgumentList&& arguments,
                   ASTPremiseDefnList&& premiseDefns,
                   ASTPropositionDefn&& propositionDefn)
    : _name(name)
    , _globalDecls(globalDecls)
    , _arguments(arguments)
    , _premiseDefns(premiseDefns)
    , _propositionDefn(propositionDefn)
  {
  }

  const StringType& name() const
  {
    return _name;
  }

  const ASTGlobalDeclList& globalDecls() const
  {
    return _globalDecls;
  }

  const ASTInferenceArgumentList& arguments() const
  {
    return _arguments;
  }

  const ASTPremiseDefnList& premiseDefns() const
  {
    return _premiseDefns;
  }

  const ASTPropositionDefn& propositionDefn() const
  {
    return _propositionDefn;
  }

private:
  StringType _name;
  ASTGlobalDeclList _globalDecls;
  ASTInferenceArgumentList _arguments;
  ASTPremiseDefnList _premiseDefns;
  ASTPropositionDefn _propositionDefn;
};

// -----------------------------------------------------------------------------

class ASTEnvironmentDefn : public ASTNode
{
public:
  ASTEnvironmentDefn()
    : _field()
    , _value()
  {
  }

  ASTEnvironmentDefn(StringType&& field, StringType&& value)
    : _field(field)
    , _value(value)
  {
  }

  const StringType& field() const
  {
    return _field;
  }

  const StringType& value() const
  {
    return _value;
  }

private:
  StringType _field;
  StringType _value;
};

// -----------------------------------------------------------------------------

class ASTInferenceGroup : public ASTNode
{
public:
  ASTInferenceGroup()
    : _name()
    , _environmentDefns()
    , _inferenceDefns()
  {
  }

  ASTInferenceGroup(StringType&& name,
                    ASTEnvironmentDefnList&& environmentDefns,
                    ASTInferenceDefnList&& inferenceDefns)
    : _name(name)
    , _environmentDefns(environmentDefns)
    , _inferenceDefns(inferenceDefns)
  {
  }

  const StringType& name() const
  {
    return _name;
  }

  const ASTEnvironmentDefnList& environmentDefns() const
  {
    return _environmentDefns;
  }

  const ASTInferenceDefnList& inferenceDefns() const
  {
    return _inferenceDefns;
  }

private:
  StringType _name;
  ASTEnvironmentDefnList _environmentDefns;
  ASTInferenceDefnList _inferenceDefns;
};

// -----------------------------------------------------------------------------

class ASTModule : public ASTNode
{
public:
  ASTModule()
    : _inferenceGroups()
  {
  }

  explicit ASTModule(ASTInferenceGroupList&& inferenceGroups)
    : _inferenceGroups(inferenceGroups)
  {
  }

  const ASTInferenceGroupList& inferenceGroups() const
  {
    return _inferenceGroups;
  }

  ASTInferenceGroupList& inferenceGroups()
  {
    return _inferenceGroups;
  }

private:
  ASTInferenceGroupList _inferenceGroups;
};

// -----------------------------------------------------------------------------
