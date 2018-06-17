group MyGroup {
  ClassName          : MyGroup;
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
      StaticMethodCallStmt.argument_types : ArgumentsTypes;
      StaticMethodCallStmt.parameter_types : ParameterTypes;
      StaticMethodCallStmt.return_type: ReturnType while {
      ReturnType <= ParameterTypes[] inrange 0..1..ParameterTypes[];
      };
    ]

    proposition : ReturnType;
  }
}
