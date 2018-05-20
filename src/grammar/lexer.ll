/*******************************************************************************
The MIT License (MIT)

Copyright (c) 2018 William Li

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*******************************************************************************/

%{ /* -*- C++ -*- */
# include <cstdio>
# include <cerrno>
# include <cstdlib>


// The location of the current token.
static yy::location loc;
%}
%option noyywrap nounput batch debug noinput

integer ([0]|[0-9]*)
blank [ \t]

%{
  // Code run each time a pattern is matched.
  # define YY_USER_ACTION loc.columns(yyleng);
%}

%%

%{
  // Code run each time yylex is called.
  loc.step();
%}

#keywords
"group"
"inference"
"environment"
"arguments"
"while"
"premises"
"proposition"

#identifiers
[_a-zA-Z][_a-zA-Z0-9]*

#integer
{integer} {
  long int n = strtol(yytext, NULL, 10);
  if (errno == ERANGE)
  {
    n = 0;
  }
  return n;
}

#colon
":"

#equal
"="

#less_than
"<"

#no_more_than
"<="

#dot
"."

#sqaure_brackets
"["
"]"

#curly_braces
"{"
"}"

{blank}+                                  loc.step();
[\n]+                                     loc.lines(yyleng); loc.step();

.                                         {
                                            /* ignore bad characters */
                                          }

<<EOF>>

%%
