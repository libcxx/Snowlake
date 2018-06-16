group MyGroup {
  ClassName                      : MyInference;
  TypeClass                      : TypeCls;
  ProofMethod                    : proveType;
  TypeCmpMethod                  : cmpType;

  inference MethodStaticDispatch {
    arguments: [
    ]

    premises: [
      StaticMethodCallStmt.function_arguments  : ArgumentsTypes[] if {
        StaticMethodCallStmt.callee            : CalleeType;
      };
      StaticMethodCallStmt.callee.return_type  : ReturnType;
    ]

    proposition : ReturnType;
  }
}
