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

#include "CmdlDriver.h"
#include "ArgumentParser.h"
#include "version.h"

// -----------------------------------------------------------------------------

CmdlDriver::CmdlDriver()
  : m_opts()
{
}

// -----------------------------------------------------------------------------

const CmdlDriver::Options&
CmdlDriver::options() const
{
  return m_opts;
}

// -----------------------------------------------------------------------------

bool
CmdlDriver::run(int argc, char** argv)
{
  ArgumentParser argparser(SNOWLAKE_PROG_NAME, SNOWLAKE_VERSION_STRING, SNOWLAKE_PROG_DESC);

  argparser.add_string_parameter("output", "Output path", true,
                                 &m_opts.output_path);
  argparser.add_boolean_parameter("errors", "Treat warnings as errors", false,
                                  &m_opts.warningsAsErrors, false);
  argparser.add_boolean_parameter("bail", "Bail on first error", false,
                                  &m_opts.bailOnFirstError, false);
  argparser.add_boolean_parameter("debug", "Debug mode", false,
                                  &m_opts.debugMode, false);
  argparser.set_minimum_positional_args_required(1);

  const bool res = argparser.parse_args(argc, argv);
  if (!res) {
    return false;
  }

  m_opts.input_path = argparser.positional_args().front();

  return true;
}

// -----------------------------------------------------------------------------
