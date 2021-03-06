/*******************************************************************************
The MIT License (MIT)

Copyright (c) 2020 Tomiko

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

#pragma once

#include "CompilerErrorCategory.h"
#include "SemanticAnalysisErrorCodes.h"

#include <cassert>

struct SemanticAnalysisErrorCategory
  : public CompilerErrorCategory<SemanticAnalysisErrorCategory>
{
  static const char* CategoryName()
  {
    return "semantic analysis error";
  }

  static const char* CategoryMessageByCode(CompilerError::Code code)
  {
    switch (code) {
      case kSemanticAnalysisDuplicateInferenceGroupIdentifierError:
        return "duplicate infernece group identifier";
      case kSemanticAnalysisDuplicateInferenceDefnIdentifierError:
        return "duplicate inference definition identifier";
      case kSemanticAnalysisDuplicateEnvironmentDefnFieldError:
        return "duplicate environment definition field";
      case kSemanticAnalysisDuplicateGlobalDefinitionError:
        return "duplicate global symbol definition";
      case kSemanticAnalysisDuplicateArgumentIdentifierError:
        return "duplicate argument identifier";
      case kSemanticAnalysisInvalidTargetTypeError:
        return "invalid target type";
      case kSemanticAnalysisMissingRequiredEnvironmentDefnFieldError:
        return "missing required environment definition field";
      case kSemanticAnalysisUnknownSymbolError:
        return "unknown symbol";
      case kSemanticAnalysisIncompatibleTargetTypeError:
        return "incompatible target type";
      case kSemanticAnalysisUnknownPremiseDefnError:
        return "unknown premise definition";
      default:
        assert(0 && "Unrecognized error code");
        return "unrecognized error code";
    }
  }
};
