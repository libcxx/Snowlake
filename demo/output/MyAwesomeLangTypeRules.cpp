/**
 * Auto-generated by Snowlake compiler (version 0.1.1).
 */

/**
 * This file was synthesized from MyAwesomeLangTypeRules.sl
 */

#include "MyAwesomeLangTypeRules.h"
#include "InferenceErrorDefn.h"

/**
 * This method was synthesized from the "StaticMethodDispatch" inference definition.
 */
TypeCls
MyAwesomeLangTypeRules::StaticMethodDispatch(const ASTExpr& StaticMethodCallStmt, std::error_code* err)
{
    std::vector<TypeCls> ArgumentsTypes = proveType(StaticMethodCallStmt.argument_types);
    std::vector<TypeCls> ParameterTypes = proveType(StaticMethodCallStmt.callee.parameter_types);
    for (size_t i = 0, size_t j = 1; i < ParameterTypes.size(); ++i, ++j) {
        if (!cmpType(ArgumentsTypes[i], ParameterTypes[j], std::less_equal<TypeCls>())) {
            *err = std::error_code(0, inference_error_category);
            return TypeCls();
        }
    }

    if (!cmpType(ArgumentsTypes, SELF_TYPE, std::not_equal_to<TypeCls>())) {
        *err = std::error_code(0, inference_error_category);
        return TypeCls();
    }

    // Type annotation setup.
    typeAnnotationSetup(StaticMethodCallStmt.caller_type, CLS_TYPE);

    for (size_t i = 1, size_t j = 1; i < ParameterTypes.size(); ++i, ++j) {
        if (!cmpType(ArgumentsTypes[i], ParameterTypes[j], std::less_equal<TypeCls>())) {
            *err = std::error_code(0, inference_error_category);
            return TypeCls();
        }
    }

    TypeCls var0 = getBaseType();
    TypeCls var1 = proveType(StaticMethodCallStmt.return_caller_type);
    if (!cmpType(var0, var1, std::equal_to<>())) {
        *err = std::error_code(0, inference_error_category);
        return TypeCls();
    }

    // Type annotation teardown.
    typeAnnotationTeardown(StaticMethodCallStmt.caller_type, CLS_TYPE);

    TypeCls var2 = getBaseType();
    TypeCls var3 = proveType(StaticMethodCallStmt.caller_type);
    if (!cmpType(var2, var3, std::equal_to<>())) {
        *err = std::error_code(0, inference_error_category);
        return TypeCls();
    }

    TypeCls returnType = proveType(StaticMethodCallStmt.return_type);
    return baseType(returnType);
}
