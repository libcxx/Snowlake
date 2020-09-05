group MyAwesomeLang {

    ClassName                      : MyAwesomeLangTypeRules;
    TypeClass                      : TypeCls;
    ProofMethod                    : proveType;
    TypeCmpMethod                  : cmpType;
    TypeAnnotationSetupMethod      : typeAnnotationSetup;
    TypeAnnotationTeardownMethod   : typeAnnotationTeardown;

    inference StaticMethodDispatch {

        globals: [
            SELF_TYPE,
            CLS_TYPE
        ]

        arguments: [
            StaticMethodCallStmt : ASTExpr
        ]

        premises: [
            StaticMethodCallStmt.argument_types            : ArgumentsTypes[];

            StaticMethodCallStmt.callee.parameter_types    : ParameterTypes[];

            ArgumentsTypes[] <= ParameterTypes[] inrange 0..1..ParameterTypes[];

            ArgumentsTypes[0] != SELF_TYPE;

            StaticMethodCallStmt.caller_type : CLS_TYPE while {
                ArgumentsTypes[] <= ParameterTypes[] inrange 1..1..ParameterTypes[];

                StaticMethodCallStmt.return_caller_type      : getBaseType();
            };

            StaticMethodCallStmt.caller_type               : getBaseType();

            StaticMethodCallStmt.return_type               : returnType;
        ]

        proposition : baseType(returnType);
    }
}
