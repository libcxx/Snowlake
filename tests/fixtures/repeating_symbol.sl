group MyGroup {
  ClassName          : MyGroup;
  TypeClass          : TypeCls;
  ProofMethod        : proveType;
  TypeCmpMethod      : cmpType;
  
  inference MyInference {
    globals: [
      Arg1
    ]
    arguments: [Arg1 : ArgType]
    premises: []
    proposition: HelloWorld;
  }
}
