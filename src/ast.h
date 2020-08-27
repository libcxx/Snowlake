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

typedef std::string StringType;
typedef uint64_t IntegerType;

enum class EqualityOperator : uint8_t
{
  OPERATOR_EQ,
  OPERATOR_NEQ,
  OPERATOR_LT,
  OPERATOR_LTE
};

class ASTNode
{
};

typedef std::vector<ASTIdentifier> ASTIdentifierList;
typedef std::vector<ASTDeductionTarget> ASTDeductionTargetList;
typedef std::vector<ASTPremiseDefn> ASTPremiseDefnList;
typedef std::vector<ASTInferenceArgument> ASTInferenceArgumentList;
typedef std::vector<ASTGlobalDecl> ASTGlobalDeclList;
typedef std::vector<ASTEnvironmentDefn> ASTEnvironmentDefnList;
typedef std::vector<ASTInferenceDefn> ASTInferenceDefnList;
typedef std::vector<ASTInferenceGroup> ASTInferenceGroupList;

class ASTIdentifier : public ASTNode
{
public:
  ASTIdentifier()
    : m_value()
  {
  }

  ASTIdentifier(StringType&& value)
    : m_value(value)
  {
  }

  const StringType& value() const
  {
    return m_value;
  }

private:
  StringType m_value;
};

class ASTIdentifiable : public ASTNode
{
public:
  ASTIdentifiable()
    : m_identifiers()
  {
  }

  ASTIdentifiable(ASTIdentifierList&& identifiers)
    : m_identifiers(identifiers)
  {
  }

  void add(const ASTIdentifier& element)
  {
    m_identifiers.emplace_back(element);
  }

  const ASTIdentifierList& identifiers() const
  {
    return m_identifiers;
  }

private:
  ASTIdentifierList m_identifiers;
};

class ASTDeductionTargetSingular
{
public:
  ASTDeductionTargetSingular()
    : m_name()
  {
  }

  explicit ASTDeductionTargetSingular(StringType&& name)
    : m_name(name)
  {
  }

  const StringType& name() const
  {
    return m_name;
  }

private:
  StringType m_name;
};

class ASTDeductionTargetArray
{
public:
  ASTDeductionTargetArray()
    : m_name()
    , m_array_size()
  {
  }

  explicit ASTDeductionTargetArray(StringType&& name)
    : m_name(name)
    , m_array_size()
  {
  }

  ASTDeductionTargetArray(StringType&& name, IntegerType array_size)
    : m_name(name)
    , m_array_size(array_size)
  {
  }

  const StringType& name() const
  {
    return m_name;
  }

  bool hasSizeLiteral() const
  {
    return m_array_size.has_value();
  }

  IntegerType sizeLiteral() const
  {
    return m_array_size.value();
  }

private:
  StringType m_name;
  sl::optional<IntegerType> m_array_size;
};

class ASTDeductionTargetComputed
{
public:
  ASTDeductionTargetComputed()
    : m_name()
    , m_arguments()
  {
  }

  ASTDeductionTargetComputed(StringType&& name,
                             ASTDeductionTargetList&& arguments)
    : m_name(name)
    , m_arguments(arguments)
  {
  }

  const StringType& name() const
  {
    return m_name;
  }

  const ASTDeductionTargetList& arguments() const
  {
    return m_arguments;
  }

private:
  StringType m_name;
  ASTDeductionTargetList m_arguments;
};

class ASTDeductionTarget : public ASTNode
{
public:
  ASTDeductionTarget()
    : m_value()
  {
  }

  ASTDeductionTarget(ASTDeductionTargetSingular&& value)
    : m_value(value)
  {
  }

  ASTDeductionTarget(ASTDeductionTargetArray&& value)
    : m_value(value)
  {
  }

  ASTDeductionTarget(ASTDeductionTargetComputed&& value)
    : m_value(value)
  {
  }

  template <typename U>
  bool isType() const
  {
    return m_value.template is<U>();
  }

  template <typename U>
  const U& value() const
  {
    return m_value.template get<U>();
  }

private:
  sl::variant::variant<ASTDeductionTargetSingular, ASTDeductionTargetArray,
                       ASTDeductionTargetComputed>
      m_value;
};

class ASTPropositionDefn
{
public:
  ASTPropositionDefn()
    : m_target()
  {
  }

  ASTPropositionDefn(ASTDeductionTarget&& target)
    : m_target(target)
  {
  }

  const ASTDeductionTarget& target() const
  {
    return m_target;
  }

private:
  ASTDeductionTarget m_target;
};

class ASTRangeClause : public ASTNode
{
public:
  ASTRangeClause()
    : m_lhs_idx(0)
    , m_rhs_idx(0)
    , m_deduction_target()
  {
  }

  ASTRangeClause(IntegerType lhsIdx, IntegerType rhsIdx,
                 ASTDeductionTarget&& deductionTarget)
    : m_lhs_idx(lhsIdx)
    , m_rhs_idx(rhsIdx)
    , m_deduction_target(deductionTarget)
  {
  }

  IntegerType lhsIdx() const
  {
    return m_lhs_idx;
  }

  IntegerType rhsIdx() const
  {
    return m_rhs_idx;
  }

  const ASTDeductionTarget& deductionTarget() const
  {
    return m_deduction_target;
  }

private:
  IntegerType m_lhs_idx;
  IntegerType m_rhs_idx;
  ASTDeductionTarget m_deduction_target;
};

class ASTInferenceEqualityDefn
{
public:
  ASTInferenceEqualityDefn()
    : m_lhs()
    , m_rhs()
    , m_range_clause()
  {
  }

  ASTInferenceEqualityDefn(ASTDeductionTarget&& lhs, ASTDeductionTarget&& rhs,
                           EqualityOperator oprt)
    : m_lhs(lhs)
    , m_rhs(rhs)
    , m_oprt(oprt)
  {
  }

  ASTInferenceEqualityDefn(ASTDeductionTarget&& lhs, ASTDeductionTarget&& rhs,
                           EqualityOperator oprt, ASTRangeClause&& rangeClause)
    : m_lhs(lhs)
    , m_rhs(rhs)
    , m_oprt(oprt)
    , m_range_clause(rangeClause)
  {
  }

  const ASTDeductionTarget& lhs() const
  {
    return m_lhs;
  }

  const ASTDeductionTarget& rhs() const
  {
    return m_rhs;
  }

  EqualityOperator oprt() const
  {
    return m_oprt;
  }

  bool hasRangeClause() const
  {
    return m_range_clause.has_value();
  }

  const ASTRangeClause& rangeClause() const
  {
    return m_range_clause.value();
  }

private:
  ASTDeductionTarget m_lhs;
  ASTDeductionTarget m_rhs;
  EqualityOperator m_oprt;
  sl::optional<ASTRangeClause> m_range_clause;
};

class ASTWhileClause : public ASTNode
{
public:
  ASTWhileClause()
    : m_premise_defns()
  {
  }

  explicit ASTWhileClause(ASTPremiseDefnList&& premiseDefns)
    : m_premise_defns(premiseDefns)
  {
  }

  const ASTPremiseDefnList& premiseDefns() const
  {
    return m_premise_defns;
  }

private:
  ASTPremiseDefnList m_premise_defns;
};

class ASTInferencePremiseDefn
{
public:
  ASTInferencePremiseDefn()
    : m_source()
    , m_deduction_target()
    , m_while_clause()
  {
  }

  ASTInferencePremiseDefn(ASTIdentifiable&& source,
                          ASTDeductionTarget&& deductionTarget)
    : m_source(source)
    , m_deduction_target(deductionTarget)
    , m_while_clause()
  {
  }

  ASTInferencePremiseDefn(ASTIdentifiable&& source,
                          ASTDeductionTarget&& deductionTarget,
                          ASTWhileClause&& whileClause)
    : m_source(source)
    , m_deduction_target(deductionTarget)
    , m_while_clause(whileClause)
  {
  }

  const ASTIdentifiable& source() const
  {
    return m_source;
  }

  const ASTDeductionTarget& deductionTarget() const
  {
    return m_deduction_target;
  }

  bool hasWhileClause() const
  {
    return m_while_clause.has_value();
  }

  const ASTWhileClause& whileClause() const
  {
    return m_while_clause.value();
  }

private:
  ASTIdentifiable m_source;
  ASTDeductionTarget m_deduction_target;
  sl::optional<ASTWhileClause> m_while_clause;
};

class ASTPremiseDefn : public ASTNode
{
public:
  ASTPremiseDefn()
    : m_value()
  {
  }

  ASTPremiseDefn(ASTInferencePremiseDefn&& defn)
    : m_value(defn)
  {
  }

  ASTPremiseDefn(ASTInferenceEqualityDefn&& defn)
    : m_value(defn)
  {
  }

  template <class U>
  bool isType() const
  {
    return m_value.template is<U>();
  }

  template <class U>
  const U& value() const
  {
    return m_value.template get<U>();
  }

private:
  sl::variant::variant<ASTInferencePremiseDefn, ASTInferenceEqualityDefn>
      m_value;
};

class ASTInferenceArgument : public ASTNode
{
public:
  ASTInferenceArgument()
    : m_name()
    , m_type_name()
  {
  }

  ASTInferenceArgument(StringType&& name, StringType&& typeName)
    : m_name(name)
    , m_type_name(typeName)
  {
  }

  const StringType& name() const
  {
    return m_name;
  }

  const StringType& typeName() const
  {
    return m_type_name;
  }

private:
  StringType m_name;
  StringType m_type_name;
};

class ASTGlobalDecl : public ASTNode
{
public:
  ASTGlobalDecl()
    : m_name()
  {
  }

  ASTGlobalDecl(StringType&& name)
    : m_name(name)
  {
  }

  const StringType& name() const
  {
    return m_name;
  }

private:
  StringType m_name;
};

class ASTInferenceDefn : public ASTNode
{
public:
  ASTInferenceDefn()
    : m_name()
    , m_global_decls()
    , m_arguments()
    , m_premise_defns()
    , m_proposition_defn()
  {
  }

  ASTInferenceDefn(StringType&& name, ASTGlobalDeclList&& globalDecls,
                   ASTInferenceArgumentList&& arguments,
                   ASTPremiseDefnList&& premiseDefns,
                   ASTPropositionDefn&& propositionDefn)
    : m_name(name)
    , m_global_decls(globalDecls)
    , m_arguments(arguments)
    , m_premise_defns(premiseDefns)
    , m_proposition_defn(propositionDefn)
  {
  }

  const StringType& name() const
  {
    return m_name;
  }

  const ASTGlobalDeclList& globalDecls() const
  {
    return m_global_decls;
  }

  const ASTInferenceArgumentList& arguments() const
  {
    return m_arguments;
  }

  const ASTPremiseDefnList& premiseDefns() const
  {
    return m_premise_defns;
  }

  const ASTPropositionDefn& propositionDefn() const
  {
    return m_proposition_defn;
  }

private:
  StringType m_name;
  ASTGlobalDeclList m_global_decls;
  ASTInferenceArgumentList m_arguments;
  ASTPremiseDefnList m_premise_defns;
  ASTPropositionDefn m_proposition_defn;
};

class ASTEnvironmentDefn : public ASTNode
{
public:
  ASTEnvironmentDefn()
    : m_field()
    , m_value()
  {
  }

  ASTEnvironmentDefn(StringType&& field, StringType&& value)
    : m_field(field)
    , m_value(value)
  {
  }

  const StringType& field() const
  {
    return m_field;
  }

  const StringType& value() const
  {
    return m_value;
  }

private:
  StringType m_field;
  StringType m_value;
};

class ASTInferenceGroup : public ASTNode
{
public:
  ASTInferenceGroup()
    : m_name()
    , m_environment_defns()
    , m_inference_defns()
  {
  }

  ASTInferenceGroup(StringType&& name,
                    ASTEnvironmentDefnList&& environmentDefns,
                    ASTInferenceDefnList&& inferenceDefns)
    : m_name(name)
    , m_environment_defns(environmentDefns)
    , m_inference_defns(inferenceDefns)
  {
  }

  const StringType& name() const
  {
    return m_name;
  }

  const ASTEnvironmentDefnList& environmentDefns() const
  {
    return m_environment_defns;
  }

  const ASTInferenceDefnList& inferenceDefns() const
  {
    return m_inference_defns;
  }

private:
  StringType m_name;
  ASTEnvironmentDefnList m_environment_defns;
  ASTInferenceDefnList m_inference_defns;
};

class ASTModule : public ASTNode
{
public:
  ASTModule()
    : m_inference_groups()
  {
  }

  explicit ASTModule(ASTInferenceGroupList&& inferenceGroups)
    : m_inference_groups(inferenceGroups)
  {
  }

  const ASTInferenceGroupList& inferenceGroups() const
  {
    return m_inference_groups;
  }

  ASTInferenceGroupList& inferenceGroups()
  {
    return m_inference_groups;
  }

private:
  ASTInferenceGroupList m_inference_groups;
};
