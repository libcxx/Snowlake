![Snowlake Logo](/resources/SnowlakeIcon_256.png)


# Snowlake

[![Build Status](https://travis-ci.org/tetrachrome/Snowlake.svg?branch=master)](https://travis-ci.org/tetrachrome/Snowlake)
[![C/C++ CI](https://github.com/tetrachrome/Snowlake/workflows/C/C++%20CI/badge.svg)](https://github.com/tetrachrome/Snowlake/actions)
[![Documentation](https://readthedocs.org/projects/snowlake/badge/?version=latest)](https://snowlake.readthedocs.io/en/latest/)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](https://opensource.org/licenses/MIT)
[![platform-linux](https://img.shields.io/badge/platform-linux-orange)](#)
[![platform-osx](https://img.shields.io/badge/platform-OSX-blue)](#)
[![build-gcc](https://img.shields.io/badge/build-gcc-green)](#)
[![build-clang](https://img.shields.io/badge/build-clang-9cf)](#)


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


## Quick Demo

Imagine we are going on the adventure of designing and building the next great
statically typed system programming language, and name it *MyAwesomeLang*.

As a statically typed language, *MyAwesomeLang* would have similar semantics with
regards to static method dispatch. The compiler we are going to build for the
language needs to perform the necessary semantic analysis and type checking for
each static method callsite to ensure that 1). the binding of the method and
supplied arguments is legal, and 2). the type of the return value from any
execution path within the method call can bind with the variable that's going
to be holding the return value.

Implementing such logic in code directly can be tedious, error prone, and may
also lose the potential to be self-documenting. It can also be difficult for
other people reading the code for the first time to quickly understand the
semantics captured behind it.

We can define the semantic rules of such static method dispatch declaratively
with the *Snowlake* language, in a file called **MyAwesomeLangTypeRules.sl**.

[MyAwesomeLangTypeRules.sl](./demo/MyAwesomeLangTypeRules.sl)

```
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
```

With the rules defined above, we can invoke the *Snowlake* compiler to
synthesize it into actual C++ interface and implementation code:

```
$ snowlakec --errors -o ./output MyAwesomeLangTypeRules.sl
```

The *Snowlake* compiler takes the **MyAwesomeLangTypeRules.sl** file as input
and synthesizes the corresponding output (in this case, C++ .h and .cpp files)
that contain the interface and implementation of executing the static type
checking according to the semantic rules defined above.

Once that's done we can take a look at the synthesized output files:

```
$ ls -al ./output/
total 32
drwxrwxr-x  2 x x 4096 Sep  4 17:18 .
drwxrwxr-x 12 x x 4096 Sep  4 17:18 ..
-rw-rw-r--  1 x x  773 Sep  4 17:18 InferenceErrorDefn.cpp
-rw-rw-r--  1 x x  283 Sep  4 17:18 InferenceErrorDefn.h
-rw-rw-r--  1 x x 2166 Sep  4 17:18 MyAwesomeLangTypeRules.cpp
-rw-rw-r--  1 x x  366 Sep  4 17:18 MyAwesomeLangTypeRules.h
```

In this example it would generate the following .h and .cpp files:

[MyAwesomeLangTypeRules.h](./demo/output/MyAwesomeLangTypeRules.h)

```
/**
 * Auto-generated by Snowlake compiler (version 0.1.1).
 */

/**
 * This file was synthesized from ./demo/MyAwesomeLangTypeRules.sl
 */

#pragma once

#include <cstdlib>
#include <cstddef>
#include <vector>
#include <system_error>

/**
 * This class was synthesized from the "MyAwesomeLangTypeRules" rules group.
 */
class MyAwesomeLangTypeRules
{
public:
    /**
     * This method was synthesized from the "StaticMethodDispatch" inference definition.
     */
    TypeCls StaticMethodDispatch(const ASTExpr& StaticMethodCallStmt, std::error_code*);

};
```

[MyAwesomeLangTypeRules.cpp](./demo/output/MyAwesomeLangTypeRules.cpp)

```
/**
 * Auto-generated by Snowlake compiler (version 0.1.1).
 */

/**
 * This file was synthesized from ./demo/MyAwesomeLangTypeRules.sl
 */

#include "MyAwesomeLangTypeRules.h"
#include "InferenceErrorDefn.h"

/**
 * This method was synthesized from the "StaticMethodDispatch" inference definition.
 */
TypeCls
MyAwesomeLangTypeRules::StaticMethodDispatch(const ASTExpr& StaticMethodCallStmt, std::error_code* err)
{
    // This corresponds to the 1st premise rule in the inference definition.
    std::vector<TypeCls> ArgumentsTypes = proveType(StaticMethodCallStmt.argument_types);

    // This corresponds to the 2nd premise rule in the inference definition.
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
    // This corresponds to the 3rd premise rule in the inference definition.

    // Type annotation setup.
    typeAnnotationSetup(StaticMethodCallStmt.caller_type, CLS_TYPE);

    for (size_t i = 1, size_t j = 1; i < ParameterTypes.size(); ++i, ++j) {
        if (!cmpType(ArgumentsTypes[i], ParameterTypes[j], std::less_equal<TypeCls>())) {
            *err = std::error_code(0, inference_error_category);
            return TypeCls();
        }
    }

    // This corresponds to the 3rd premise rule in the inference definition.
    TypeCls var0 = getBaseType();
    TypeCls var1 = proveType(StaticMethodCallStmt.return_caller_type);
    if (!cmpType(var0, var1, std::equal_to<>())) {
        *err = std::error_code(0, inference_error_category);
        return TypeCls();
    }

    // Type annotation teardown.
    typeAnnotationTeardown(StaticMethodCallStmt.caller_type, CLS_TYPE);

    // This corresponds to the 5th premise rule in the inference definition.
    TypeCls var2 = getBaseType();
    TypeCls var3 = proveType(StaticMethodCallStmt.caller_type);
    if (!cmpType(var2, var3, std::equal_to<>())) {
        *err = std::error_code(0, inference_error_category);
        return TypeCls();
    }

    // This corresponds to the 6th premise rule in the inference definition.
    TypeCls returnType = proveType(StaticMethodCallStmt.return_type);

    return baseType(returnType);
}
```

The synthesized source code files are intended to be integrated with the
rest of the code for the compiler of our language.

All the source for this demo can be found at [./demo/](./demo/).


## Build and Test

To build all targets (i.e. static libraries and executables), plus the unit test suite and run it,
do the following:

```
mkdir build && cd build && cmake .. && make
```


## Documentation

Please refer to the [Snowlake Documentation](https://snowlake.readthedocs.io/en/latest/)
to learn more.


## License

*Snowlake* is licensed under [The MIT License](http://opensource.org/licenses/MIT).
