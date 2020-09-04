![Snowlake Logo](/resources/SnowlakeIcon_256.png)


# Snowlake

[![Build Status](https://travis-ci.org/tetrachrome/Snowlake.svg?branch=master)](https://travis-ci.org/tetrachrome/Snowlake)
[![C/C++ CI](https://github.com/tetrachrome/Snowlake/workflows/C/C++%20CI/badge.svg)](https://github.com/tetrachrome/Snowlake/actions)
[![Documentation](https://readthedocs.org/projects/snowlake/badge/?version=latest)](https://snowlake.readthedocs.io/en/latest/)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](https://opensource.org/licenses/MIT)


## Overview

*Snowlake* is both a declarative language of regular rules of inference
and propositional logic for defining static type inference rules of
programming languages, as well as a *compiler-compiler* that can
synthesize such inference rule definitions into code used for static type
checking, typically used for semantic analysis in language compilers.

The goals of *Snowlake* are to:

  1. Provide a flexible declarative language that is able to define the static
     type inference rules of most programming languages.
  2. Facilitate language developers in defining, documenting and sharing
     the set of type inference rules of any particular language.
  3. Alleviate the burden on language developers from implementing type
     checking logic that are usually extremely complex, tedious and error-prone.

### Demo

Imagine we are going on the adventure of designing and building the next great
statically typed system programming language, and name it *MyAwesomeLang*.

As a statically typed language, *MyAwesomeLang* would have a similar semantics with
regards to static method dispatch. The compiler we are going to build for the
language needs to perform the necessary semantic analysis and type checking for
each static method callsite to ensure that 1). the binding of such call and
supplied arguments is legal, and 2). the type of the return value from any
execution path within the method call can bind with the variable that's going
to be holding the return value.

Implementing such logic in code directly can be tedious, error prone, and may
also lose the potential to be self-documenting. It can also be difficult for
other people reading the code for the first time to quickly understand the
semantics captured behind it.

We can define the semantic rules of such static method dispatch declaratively
with the *Snowlake* language:

```
group MyAwesomeLang {

    ClassName     : MyAwesomeLangStaticTypeAnalyzer;
    TypeClass     : TypeCls;
    ProofMethod   : proveType;
    TypeCmpMethod : cmpType;

    inference StaticMethodStaticDispatch {

        globals: [
            SELF_TYPE
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
            };

            StaticMethodCallStmt.caller_type               : getBaseType();
            StaticMethodCallStmt.return_type               : returnType;
        ]

        proposition : baseType(returnType);
    }
}
```

The *Snowlake* compiler would take that as input and synthesize the corresponding
code (in this case, C++ .h and .cpp files) that contain the interface and implementation
of executing the static type checking according to the semantic rules defined above.

**MyAwesomeLangStaticTypeAnalyzer.h**

```
#pragma once

#include <cstdlib>
#include <cstddef>
#include <vector>
#include <system_error>

class MyAwesomeLangStaticTypeAnalyzer
{
public:
    TypeCls MethodStaticDispatch(const ASTExpr& StaticMethodCallStmt, std::error_code*);
};
```

**MyAwesomeLangStaticTypeAnalyzer.cpp**

```
#include "SampleProjectTypeChecker.h"
#include "InferenceErrorDefn.h"

TypeCls
MyAwesomeLangStaticTypeChecker::MethodStaticDispatch(const ASTExpr& StaticMethodCallStmt, std::error_code* err)
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

    // Type annotation teardown.
    typeAnnotationTeardown(StaticMethodCallStmt.caller_type, CLS_TYPE);

    TypeCls var0 = getBaseType();
    TypeCls var1 = proveType(StaticMethodCallStmt.caller_type);
    if (!cmpType(var0, var1, std::equal_to<>())) {
        *err = std::error_code(0, inference_error_category);
        return TypeCls();
    }

    TypeCls returnType = proveType(StaticMethodCallStmt.return_type);
    return baseType(returnType);
}
```

## Build and Test

To build all targets (i.e. static libraries and executables), plus the unit test suite and run it,
do the following:

```
mkdir build && cd build && cmake .. && make
```


## Development Environment and Dependencies

Please refer to the [Development Environment and Dependencies](https://github.com/tetrachrome/Snowlake/wiki/Development-Environment-and-Dependencies) wiki page for detailed up-to-date
information.


## Documentation

Please refer to the [Snowlake Documentation](https://snowlake.readthedocs.io/en/latest/)
to learn more.


## License

*Snowlake* is licensed under [The MIT License](http://opensource.org/licenses/MIT).
