.. Copyright William Li. All rights reserved.

Tutorial
========

Snowlake joins Flex and Bison (formerly known as Lex and Yacc) as a new
member in the family of compiler-compilers in empowering programming language
designers, authors, and engineers to be more efficient and productive.
As Flex and Bison focuses on enabling the authoring of language lexers and
parsers, Snowlake focuses on the next major step in language compiler
construction: static type inference as part of semantic analysis.

Snowlake is a declarative language for expressing the static type inference
rules expressed in programming language semantics. The Snowlake declarative
language is designed to be language agnostic, and is based on syntax and
semantics derived from existing languages in order to reduce the learning
curve in mastering it.

We demonstrate the features, syntax and semantics of the Snowlake language
by going through a brief example of defining the semantics and static type
inference rules of a trivial reference language. Here we intentionally omit
the introduction of the syntax of this reference language, and solely focus
on its semantics and static type inference rules, because the language syntax
is irrelevant in this context. The various features of the Snowlake language
are illustrated in detail as we progress in defining and expressing the
inference rules of our example language.

