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

#pragma once

/**
 * Definitions of constants used in Snowlake language format.
 */

/**
 * Key name for inference group environment definition "class name" field.
 */
#define SNOWLAKE_ENVN_DEFN_KEY_NAME_FOR_CLASS "ClassName"

/**
 * Key name for inference group environment definition "type class" field.
 */
#define SNOWLAKE_ENVN_DEFN_KEY_NAME_FOR_TYPE_CLASS "TypeClass"

/**
 * Key name for inference group environment definition "proof method" field.
 */
#define SNOWLAKE_ENVN_DEFN_KEY_NAME_FOR_PROOF_METHOD "ProofMethod"

/**
 * Key name for inference group environment definition
 * "type comparison method" field.
 */
#define SNOWLAKE_ENVN_DEFN_KEY_NAME_FOR_TYPE_CMP_METHOD "TypeCmpMethod"

/**
 * Key name for inference group environment "definition header" field.
 */
#define SNOWLAKE_ENVN_DEFN_KEY_NAME_FOR_HEADER "Header"

/**
 * Key name for inference group environment definition
 * "type annotation setup method" field.
 */
#define SNOWLAKE_ENVN_DEFN_KEY_NAME_FOR_TYPE_ANNOTATION_SETUP_METHOD           \
  "TypeAnnotationSetupMethod"

/**
 * Key name for inference group environment definition
 * "type annotation teardown method" field.
 */
#define SNOWLAKE_ENVN_DEFN_KEY_NAME_FOR_TYPE_ANNOTATION_TEARDOWN_METHOD        \
  "TypeAnnotationTeardownMethod"
