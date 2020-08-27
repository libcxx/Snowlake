![Snowlake Logo](/resources/SnowlakeIcon_256.png)


# Snowlake

[![Build Status](https://travis-ci.org/tetrachrome/Snowlake.svg?branch=master)](https://travis-ci.org/tetrachrome/Snowlake)
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
