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
      StaticMethodCallStmt.argument_types : ArgumentsTypes[];
      ArgumentsTypes[0] != SELF_TYPE;
    ]

    proposition : lub(T1, T2);
  }
}
