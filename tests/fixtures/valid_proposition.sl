group ValidProposition {

  ClassName          : ValidProposition;
  TypeClass          : TypeCls;
  ProofMethod        : proveType;
  TypeCmpMethod      : cmpType;

  inference MethodStaticDispatch {
    arguments: [
      StaticMethodCallStmt : ASTExpr
    ]

    premises: [
      StaticMethodCallStmt.return_type : ReturnType;
    ]

    proposition : ReturnType;
  }
}
