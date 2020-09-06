group ComputedTargetType {

  ClassName          : ComputedTargetType;
  TypeClass          : TypeCls;
  ProofMethod        : proveType;
  TypeCmpMethod      : cmpType;

  inference MethodStaticDispatch {

    globals: [
      SELF_TYPE,
      CLS_TYPE
    ]

    arguments: [
      StaticMethodCallStmt : ASTExpr
    ]

    premises: [
      StaticMethodCallStmt.argument_types : getArgumentsTypes();
      StaticMethodCallStmt.parameter_types : getParametersTypes();
      StaticMethodCallStmt.return_type: getReturnType();
    ]

    proposition : getReturnType();
  }
}
