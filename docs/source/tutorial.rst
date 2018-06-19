.. Copyright William Li. All rights reserved.

********
Tutorial
********

Snowlake joins
`Flex <https://en.wikipedia.org/wiki/Flex_(lexical_analyser_generator)>`_
and
`Bison <https://en.wikipedia.org/wiki/GNU_bison>`_
(as well as their siblings
`Lex <https://en.wikipedia.org/wiki/Lex_(software)>`_
and
`Yacc <https://en.wikipedia.org/wiki/Yacc>`_)
as a new member in the family of compiler-compilers in empowering
programming language designers, authors, and engineers to be more
efficient and innovative. As Flex and Bison focuses on enabling the
authoring of language lexers and parsers, Snowlake focuses on the
next major step in language compiler construction:
*static type checking as part of semantic analysis*.

Snowlake is a declarative language for expressing the static type inference
rules expressed in programming language semantics. The Snowlake declarative
language is designed to be language agnostic, and is based on syntax and
semantics derived from existing languages in order to reduce the learning
curve in mastering it.

Here we'd like to demonstrate the features, syntax and semantics of the
Snowlake language by going through a brief example of defining the
semantics and static type inference rules of a trivial reference language.
We chose to intentionally omit the introduction of the syntax of this
reference language, and solely focus on its semantics and static type
inference rules, because the language syntax is irrelevant in this context.
The various features of the Snowlake language are illustrated in detail as
we progress in defining and expressing the inference rules of our example
language.


Inference Groups
################

The top-level abstraction in the Snowlake language is **inference group**,
which enables logical grouping of inference rules.
Inference group definitions start with the keyword `group` followed by the
name of the group. Each file can contain multiple inference group definitions,
but their names must be unique.

Let us define one inference group to encapsulate all inference rules used
for this exercise, and name it `SampleProject`. The definition will
look like::

  group SampleProject {
  ...
  }

Each inference group definition translates directly into a corresponding
C++ class with the same name. With this group definition above, the
synthesized C++ class definition will resemble the following form::

  class SampleProject {
  ...
  };


Environment definitions
***********************

Each inference group definition contains a set of attributes that affect
all the inference rules within the group. These attributes are denoted as
key-value pairs and are specified within the group definition.
Some keys are required, while others are optional.
Below are descriptions of all supported environment definitions.

ClassName
^^^^^^^^^

The **ClassName** field is a field that specifies the name of
the synthesized C++ class, if it needs to be different from the name
of the encapsulating inference group definition.

The syntax for this field is:

`ClassName : <value>;`


TypeClass
^^^^^^^^^

The **TypeClass** field is a required field that specifies the  class type
in the synthesized C++ code for representing types in the target language.

It is critical to understand that in Snowlake language, all types in the
target language are universally represented by a single type class in C++.
One additional requirement for this type class is that it needs to be
`default constructible <http://www.cplusplus.com/reference/type_traits/is_default_constructible/>`_.

The syntax for this field is:

`TypeClass : <value>;`


ProofMethod
^^^^^^^^^^^

The **ProofMethod** field is a required field that specifies the name of the
C++ member function that can be used to infer types on identifiable entities.
This is a user supplied function that needs to be a member of the synthesized
C++ class in order for all the C++ to compile successfully.

The requirement on the signature of such function is that the return value
is of type specified by the `TypeClass <#typeclass>`_ field, and the parameters
can be a single arbitrary type that fits of the context of the synthesized code.

The syntax for this field is:

`ProofMethod : <value>;`


TypeCmpMethod
^^^^^^^^^^^^^

Similar to the "ProofMethod" field described above, the **TypeCmpMethod**
field is a required field that specifies the name of the C++ member function
that can be used compare and evaluate equality among type instances.
This is also a user supplied function that needs to be a member of the
synthesized C++ class.

The requirement on the signature of such function is that the return value
is of type `bool`, and the parameters are two instances of the type specified
by the `TypeClass <#typeclass>`_ field above, and an overloaded comparator
functor in the `std` namespace. See `Equality premise <#equality-premise>`_
below for the supported comparison functor types.

The syntax for this field is:

`TypeCmpMethod : <value>;`


TypeAnnotationSetupMethod
^^^^^^^^^^^^^^^^^^^^^^^^^

The **TypeAnnotationSetupMethod** field is an optional field that specifies
the name of the C++ member function that can be used to perform temporary
type registration setup.

The signature of such function is that the return type be `void`, and takes
a single parameter of type class specified by the `TypeClass <#typeclass>`_
field above.

This field is used in conjunction with the
`TypeAnnotationTeardownMethod <#typeannotationteardownmethod>`_ field to
perform setup and teardown.

The syntax for this field is:

`TypeAnnotationSetupMethod : <value>;`


TypeAnnotationTeardownMethod
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The **TypeAnnotationTeardownMethod** field is an optional field that specifies
the name of the C++ member function that can be used to perform temporary
type registration teardown.

The signature of such function is that the return type be `void`, and takes
a single parameter of type class specified by the `TypeClass <#typeclass>`_
field above.

This field is used in conjunction with the
`TypeAnnotationSetupMethod <#typeannotationsetupmethod>`_ field to
perform setup and teardown.

The syntax for this field is:

`TypeAnnotationTeardownMethod : <value>;`

------

With the environment definition fields described, let us specify the required
field for our inference group definition.

Since we want to have the synthesized C++ class be named
`SampleProjectTypeChecker`, and have the code live under
`SampleProjectTypeChecker.h` and `SampleProjectTypeChecker.cpp`,
let us define the following::

  ClassName : SampleProjectTypeChecker;

We also know that we are going to be using a C++ type class called `TypeCls`
for working with all type instances through our type inference logic,
so we can define the following::

  TypeClass : TypeCls;

Let's further assume that we are going to supply our own implementation
of the type proof and type comparison member functions, and they are
named `proveType` and `cmpType` respectively, we can then specify the
following two key-value pairs::

  ProofMethod   : proveType;
  TypeCmpMethod : cmpType;

With that, our inference group definition now will look like the following::

  group SampleProject {
    ClassName     : SampleProjectTypeChecker;
    TypeClass     : TypeCls;
    ProofMethod   : proveType;
    TypeCmpMethod : cmpType;
  }


Inference rule definitions
##########################

**Inference rule definitions** are at the heart of the Snowlake language.
Each inference rule definition uniquely captures the static type inference
logic associated with one language construct. The Snowlake compiler
synthesizes each inference definition into a corresponding C++ member
function, which is a member of the C++ class that is synthesized from the
corresponding parent inference group.

Each inference rule definition is made up of four parts:
**global definitions**, **parameters**, **premises**, and **proposition**,
as well as two entities that make up premise and proposition definitions:
**identifiables** and **deduced targets**.
Global definitions and parameters are input that the inference rule uses
for deriving its inferences. Premises are the logical rules that make up
the assumptions of a particular inference. Finally, each inference
definition consists one proposition definition that makes up the final
inferred type of the rule.

Inference rule definitions start with the keyword `inference` followed
by the name of the inference rule. For the purpose of this exercise,
let us define a single inference rule used for our reference language
for inferring the return type of a static function call.

Let us call the inference rule `StaticMethodStaticDispatch`. Our
inference rule definition will then look like the following::

  inference StaticMethodStaticDispatch {
  ...
  }


Global definitions
##################

**Global definitions** refer to named entities in the synthesized C++ code
that reference objects or other constructs at the global level.
Global definitions are simply declared names that tell the Snowlake compiler
that such definitions can be used throughout the inference rules in a
semantically correct manner.


Identifiables
#############

**Identifiables** in Snowlake are identifiers that simply refer to entities
or attributes of entities in the synthesized C++ code. Identifiables can be
chained with the dot (i.e. `'.'`) character to represent members on
existing identifiables.

For example, we can have an identifiable named `"binaryExpr"` that refer
to a variable named `"binaryExpr"` in C++, and `"binaryExpr.type"` that refer
to the type of the binary expression.


Deduced targets
###############

**Deduced targets** are declarations of the deduced types within an
inference rule. Deduced targets are synthesized into C++ variable
declarations and definitions, and thus can be used in subsequent
premise definitions.

There are three form of deduced targets: **singular form**,
**array form** (with and without size literal), and **computed form**.


Singular form
*************

Deduced targets in singular form represent individual named types
deduced in the inference rule.


Array form
**********

Deduced targets in array form represent a collection of types deduced
in the inference rule, and are synthesized into array/vector types in
C++ depending on if a fixed size literal is used.


Computed form
*************

Deduced targets in computed form represent types deduced through calling
a function. This form of deduced targets are used when the type deduction
result is not bound at compile time, but rather at run time. This is
important for many language constructs, such as class inheritance.


Parameters
##########

As mentioned above, each inference rule definition is synthesized into
a corresponding C++ member function, thus it is a required step to define
the parameters that get passed to the function, which in turn make up
the missing part of the function signature. Each parameter is made up
of a name and its type, much like in C++. However, the difference lie
in the syntax for expressing parameters in Snowlake.

Parameters are defined under the `parameters` key within an inference
rule definition. Each parameter is defined with its name, followed by
colon (i.e. `:`), and followed by its type in the final C++ code.
Note that just like in C++, parameters for each inference rule definition
must not contain duplicate names.


Premises
########

**Premises** are the building block of inference rule definitions that
capture the logic of the inference, and are synthesized to actual C++
code within the body of the corresponding synthesized C++ function.
Premises are categorized into two types: **inference premises**
and **equality premises**.


Inference premise
*****************

**Inference premises** are logical rules that establishes the assumption
that an identifiable entity can be proven to be a specified type.
This type of premise is essential and are used in the majority of inference
rules. Inference premises following the following syntax:

*<identifiable> : <deduced target>*


Equality premise
****************

Equality premises are logical rules that establish the expected equality
relations between inferred types. They are binary expressions that evaluate
on two deduced types, along with an equality operator that represents the
equality relation. There are four types of equality relations:

+-------------------+----------+------------------------------------+
| Equality relation | Operator | Synthesized C++ comparison functor |
+===================+==========+====================================+
|   Equal           |    =     |           std::equal_to<>          |
+-------------------+----------+------------------------------------+
|   Not equal       |    !=    |           std::not_equal_to<>      |
+-------------------+----------+------------------------------------+
|   Less than       |    <     |           std::less<>              |
+-------------------+----------+------------------------------------+
|   Less or equal   |    <=    |           std::less_equal<>        |
+-------------------+----------+------------------------------------+


Proposition
###########

Each inference rule definition ends with a proposition definition that
declares the inferred type of the inference. The syntax of propositions is as:

`proposition: <deduced target>;`
