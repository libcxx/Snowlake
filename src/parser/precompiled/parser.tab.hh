// A Bison parser, made by GNU Bison 3.0.4.23-dbd16.

// Skeleton interface for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.

/**
 ** \file parser.tab.hh
 ** Define the yy::parser class.
 */

// C++ LALR(1) parser skeleton written by Akim Demaille.

#ifndef YY_YY_PARSER_TAB_HH_INCLUDED
# define YY_YY_PARSER_TAB_HH_INCLUDED
// //                    "%code requires" blocks.
#line 43 "/Users/x/workspace/snowlake/src/parser/parser.yy" // lalr1.cc:392

class ParserDriver;
#include "ast.h"

#line 49 "parser.tab.hh" // lalr1.cc:392

# include <cassert>
# include <cstdlib> // std::abort
# include <iostream>
# include <stdexcept>
# include <string>
# include <vector>
# include "stack.hh"
# include "location.hh"
#include <typeinfo>
#ifndef YYASSERT
# include <cassert>
# define YYASSERT assert
#endif


#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif


namespace yy {
#line 126 "parser.tab.hh" // lalr1.cc:392



  /// A char[S] buffer to store and retrieve objects.
  ///
  /// Sort of a variant, but does not keep track of the nature
  /// of the stored data, since that knowledge is available
  /// via the current state.
  template <size_t S>
  struct variant
  {
    /// Type of *this.
    typedef variant<S> self_type;

    /// Empty construction.
    variant ()
      : yytypeid_ (YY_NULLPTR)
    {}

    /// Construct and fill.
    template <typename T>
    variant (const T& t)
      : yytypeid_ (&typeid (T))
    {
      YYASSERT (sizeof (T) <= S);
      new (yyas_<T> ()) T (t);
    }

    /// Destruction, allowed only if empty.
    ~variant ()
    {
      YYASSERT (!yytypeid_);
    }

    /// Instantiate an empty \a T in here.
    template <typename T>
    T&
    build ()
    {
      YYASSERT (!yytypeid_);
      YYASSERT (sizeof (T) <= S);
      yytypeid_ = & typeid (T);
      return *new (yyas_<T> ()) T;
    }

    /// Instantiate a \a T in here from \a t.
    template <typename T>
    T&
    build (const T& t)
    {
      YYASSERT (!yytypeid_);
      YYASSERT (sizeof (T) <= S);
      yytypeid_ = & typeid (T);
      return *new (yyas_<T> ()) T (t);
    }

    /// Accessor to a built \a T.
    template <typename T>
    T&
    as ()
    {
      YYASSERT (*yytypeid_ == typeid (T));
      YYASSERT (sizeof (T) <= S);
      return *yyas_<T> ();
    }

    /// Const accessor to a built \a T (for %printer).
    template <typename T>
    const T&
    as () const
    {
      YYASSERT (*yytypeid_ == typeid (T));
      YYASSERT (sizeof (T) <= S);
      return *yyas_<T> ();
    }

    /// Swap the content with \a other, of same type.
    ///
    /// Both variants must be built beforehand, because swapping the actual
    /// data requires reading it (with as()), and this is not possible on
    /// unconstructed variants: it would require some dynamic testing, which
    /// should not be the variant's responsability.
    /// Swapping between built and (possibly) non-built is done with
    /// variant::move ().
    template <typename T>
    void
    swap (self_type& other)
    {
      YYASSERT (yytypeid_);
      YYASSERT (*yytypeid_ == *other.yytypeid_);
      std::swap (as<T> (), other.as<T> ());
    }

    /// Move the content of \a other to this.
    ///
    /// Destroys \a other.
    template <typename T>
    void
    move (self_type& other)
    {
      build<T> ();
      swap<T> (other);
      other.destroy<T> ();
    }

    /// Copy the content of \a other to this.
    template <typename T>
    void
    copy (const self_type& other)
    {
      build<T> (other.as<T> ());
    }

    /// Destroy the stored \a T.
    template <typename T>
    void
    destroy ()
    {
      as<T> ().~T ();
      yytypeid_ = YY_NULLPTR;
    }

  private:
    /// Prohibit blind copies.
    self_type& operator=(const self_type&);
    variant (const self_type&);

    /// Accessor to raw memory as \a T.
    template <typename T>
    T*
    yyas_ ()
    {
      void *yyp = yybuffer_.yyraw;
      return static_cast<T*> (yyp);
     }

    /// Const accessor to raw memory as \a T.
    template <typename T>
    const T*
    yyas_ () const
    {
      const void *yyp = yybuffer_.yyraw;
      return static_cast<const T*> (yyp);
     }

    union
    {
      /// Strongest alignment constraints.
      long double yyalign_me;
      /// A buffer large enough to store any of the semantic values.
      char yyraw[S];
    } yybuffer_;

    /// Whether the content is built: if defined, the name of the stored type.
    const std::type_info *yytypeid_;
  };


  /// A Bison parser.
  class Parser
  {
  public:
#ifndef YYSTYPE
    /// An auxiliary type to compute the largest semantic type.
    union union_type
    {
      // deductionTarget
      char dummy1[sizeof(ASTDeductionTarget)];

      // deduction_target_array
      char dummy2[sizeof(ASTDeductionTargetArray)];

      // deduction_target_computed
      char dummy3[sizeof(ASTDeductionTargetComputed)];

      // deduction_target_list
      char dummy4[sizeof(ASTDeductionTargetList)];

      // deduction_target_singular
      char dummy5[sizeof(ASTDeductionTargetSingular)];

      // environment_defn
      char dummy6[sizeof(ASTEnvironmentDefn)];

      // environment_defn_list
      char dummy7[sizeof(ASTEnvironmentDefnList)];

      // global_decl
      char dummy8[sizeof(ASTGlobalDecl)];

      // global_decl_set
      // global_decl_list
      char dummy9[sizeof(ASTGlobalDeclList)];

      // identifiable
      char dummy10[sizeof(ASTIdentifiable)];

      // identifier
      char dummy11[sizeof(ASTIdentifier)];

      // inference_argument
      char dummy12[sizeof(ASTInferenceArgument)];

      // argument_set
      // argument_list
      char dummy13[sizeof(ASTInferenceArgumentList)];

      // inference_defn
      char dummy14[sizeof(ASTInferenceDefn)];

      // inference_defn_list
      char dummy15[sizeof(ASTInferenceDefnList)];

      // premise_type_equality_defn
      char dummy16[sizeof(ASTInferenceEqualityDefn)];

      // inference_group
      char dummy17[sizeof(ASTInferenceGroup)];

      // inference_group_list
      char dummy18[sizeof(ASTInferenceGroupList)];

      // premise_type_inference_defn
      char dummy19[sizeof(ASTInferencePremiseDefn)];

      // input
      char dummy20[sizeof(ASTModule)];

      // premise_defn
      char dummy21[sizeof(ASTPremiseDefn)];

      // premise_set
      // premise_defn_list
      char dummy22[sizeof(ASTPremiseDefnList)];

      // proposition_defn
      char dummy23[sizeof(ASTPropositionDefn)];

      // range_clause
      char dummy24[sizeof(ASTRangeClause)];

      // while_clause
      char dummy25[sizeof(ASTWhileClause)];

      // equality_operator
      char dummy26[sizeof(EqualityOperator)];

      // KEYWORD_GROUP
      // KEYWORD_INFERENCE
      // KEYWORD_ENVIRONMENT
      // KEYWORD_ARGUMENTS
      // KEYWORD_GLOBALS
      // KEYWORD_WHILE
      // KEYWORD_INRANGE
      // KEYWORD_PREMISES
      // KEYWORD_PROPOSITION
      // IDENTIFIER
      // DOT
      // COMMA
      // COLON
      // SEMICOLON
      // LBRACKET
      // RBRACKET
      // LBRACE
      // RBRACE
      // LPAREN
      // RPAREN
      // OPERATOR_EQ
      // OPERATOR_NEQ
      // OPERATOR_LT
      // OPERATOR_LTE
      // ELLIPSIS
      char dummy27[sizeof(std::string)];

      // INTEGER_LITERAL
      char dummy28[sizeof(uint64_t)];
};

    /// Symbol semantic values.
    typedef variant<sizeof(union_type)> semantic_type;
#else
    typedef YYSTYPE semantic_type;
#endif
    /// Symbol locations.
    typedef location location_type;

    /// Syntax errors thrown from user actions.
    struct syntax_error : std::runtime_error
    {
      syntax_error (const location_type& l, const std::string& m);
      location_type location;
    };

    /// Tokens.
    struct token
    {
      enum yytokentype
      {
        END = 0,
        KEYWORD_GROUP = 258,
        KEYWORD_INFERENCE = 259,
        KEYWORD_ENVIRONMENT = 260,
        KEYWORD_ARGUMENTS = 261,
        KEYWORD_GLOBALS = 262,
        KEYWORD_WHILE = 263,
        KEYWORD_INRANGE = 264,
        KEYWORD_PREMISES = 265,
        KEYWORD_PROPOSITION = 266,
        INTEGER_LITERAL = 267,
        IDENTIFIER = 268,
        DOT = 269,
        COMMA = 270,
        COLON = 271,
        SEMICOLON = 272,
        LBRACKET = 273,
        RBRACKET = 274,
        LBRACE = 275,
        RBRACE = 276,
        LPAREN = 277,
        RPAREN = 278,
        OPERATOR_EQ = 279,
        OPERATOR_NEQ = 280,
        OPERATOR_LT = 281,
        OPERATOR_LTE = 282,
        ELLIPSIS = 283
      };
    };

    /// (External) token type, as returned by yylex.
    typedef token::yytokentype token_type;

    /// Symbol type: an internal symbol number.
    typedef int symbol_number_type;

    /// The symbol type number to denote an empty symbol.
    enum { empty_symbol = -2 };

    /// Internal symbol number for tokens (subsumed by symbol_number_type).
    typedef unsigned char token_number_type;

    /// A complete symbol.
    ///
    /// Expects its Base type to provide access to the symbol type
    /// via type_get().
    ///
    /// Provide access to semantic value and location.
    template <typename Base>
    struct basic_symbol : Base
    {
      /// Alias to Base.
      typedef Base super_type;

      /// Default constructor.
      basic_symbol ();

      /// Copy constructor.
      basic_symbol (const basic_symbol& other);

      /// Constructor for valueless symbols, and symbols from each type.

  basic_symbol (typename Base::kind_type t, const location_type& l);

  basic_symbol (typename Base::kind_type t, const ASTDeductionTarget v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const ASTDeductionTargetArray v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const ASTDeductionTargetComputed v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const ASTDeductionTargetList v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const ASTDeductionTargetSingular v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const ASTEnvironmentDefn v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const ASTEnvironmentDefnList v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const ASTGlobalDecl v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const ASTGlobalDeclList v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const ASTIdentifiable v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const ASTIdentifier v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const ASTInferenceArgument v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const ASTInferenceArgumentList v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const ASTInferenceDefn v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const ASTInferenceDefnList v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const ASTInferenceEqualityDefn v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const ASTInferenceGroup v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const ASTInferenceGroupList v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const ASTInferencePremiseDefn v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const ASTModule v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const ASTPremiseDefn v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const ASTPremiseDefnList v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const ASTPropositionDefn v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const ASTRangeClause v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const ASTWhileClause v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const EqualityOperator v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const std::string v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const uint64_t v, const location_type& l);


      /// Constructor for symbols with semantic value.
      basic_symbol (typename Base::kind_type t,
                    const semantic_type& v,
                    const location_type& l);

      /// Destroy the symbol.
      ~basic_symbol ();

      /// Destroy contents, and record that is empty.
      void clear ();

      /// Whether empty.
      bool empty () const;

      /// Destructive move, \a s is emptied into this.
      void move (basic_symbol& s);

      /// The semantic value.
      semantic_type value;

      /// The location.
      location_type location;

    private:
      /// Assignment operator.
      basic_symbol& operator= (const basic_symbol& other);
    };

    /// Type access provider for token (enum) based symbols.
    struct by_type
    {
      /// Default constructor.
      by_type ();

      /// Copy constructor.
      by_type (const by_type& other);

      /// The symbol type as needed by the constructor.
      typedef token_type kind_type;

      /// Constructor from (external) token numbers.
      by_type (kind_type t);

      /// Record that this symbol is empty.
      void clear ();

      /// Steal the symbol type from \a that.
      void move (by_type& that);

      /// The (internal) type number (corresponding to \a type).
      /// \a empty when empty.
      symbol_number_type type_get () const;

      /// The token.
      token_type token () const;

      /// The symbol type.
      /// \a empty_symbol when empty.
      /// An int, not token_number_type, to be able to store empty_symbol.
      int type;
    };

    /// "External" symbols: returned by the scanner.
    typedef basic_symbol<by_type> symbol_type;

    // Symbol constructors declarations.
    static inline
    symbol_type
    make_END (const location_type& l);

    static inline
    symbol_type
    make_KEYWORD_GROUP (const std::string& v, const location_type& l);

    static inline
    symbol_type
    make_KEYWORD_INFERENCE (const std::string& v, const location_type& l);

    static inline
    symbol_type
    make_KEYWORD_ENVIRONMENT (const std::string& v, const location_type& l);

    static inline
    symbol_type
    make_KEYWORD_ARGUMENTS (const std::string& v, const location_type& l);

    static inline
    symbol_type
    make_KEYWORD_GLOBALS (const std::string& v, const location_type& l);

    static inline
    symbol_type
    make_KEYWORD_WHILE (const std::string& v, const location_type& l);

    static inline
    symbol_type
    make_KEYWORD_INRANGE (const std::string& v, const location_type& l);

    static inline
    symbol_type
    make_KEYWORD_PREMISES (const std::string& v, const location_type& l);

    static inline
    symbol_type
    make_KEYWORD_PROPOSITION (const std::string& v, const location_type& l);

    static inline
    symbol_type
    make_INTEGER_LITERAL (const uint64_t& v, const location_type& l);

    static inline
    symbol_type
    make_IDENTIFIER (const std::string& v, const location_type& l);

    static inline
    symbol_type
    make_DOT (const std::string& v, const location_type& l);

    static inline
    symbol_type
    make_COMMA (const std::string& v, const location_type& l);

    static inline
    symbol_type
    make_COLON (const std::string& v, const location_type& l);

    static inline
    symbol_type
    make_SEMICOLON (const std::string& v, const location_type& l);

    static inline
    symbol_type
    make_LBRACKET (const std::string& v, const location_type& l);

    static inline
    symbol_type
    make_RBRACKET (const std::string& v, const location_type& l);

    static inline
    symbol_type
    make_LBRACE (const std::string& v, const location_type& l);

    static inline
    symbol_type
    make_RBRACE (const std::string& v, const location_type& l);

    static inline
    symbol_type
    make_LPAREN (const std::string& v, const location_type& l);

    static inline
    symbol_type
    make_RPAREN (const std::string& v, const location_type& l);

    static inline
    symbol_type
    make_OPERATOR_EQ (const std::string& v, const location_type& l);

    static inline
    symbol_type
    make_OPERATOR_NEQ (const std::string& v, const location_type& l);

    static inline
    symbol_type
    make_OPERATOR_LT (const std::string& v, const location_type& l);

    static inline
    symbol_type
    make_OPERATOR_LTE (const std::string& v, const location_type& l);

    static inline
    symbol_type
    make_ELLIPSIS (const std::string& v, const location_type& l);


    /// Build a parser object.
    Parser (ParserDriver& driver_yyarg);
    virtual ~Parser ();

    /// Parse.
    /// \returns  0 iff parsing succeeded.
    virtual int parse ();

#if YYDEBUG
    /// The current debugging stream.
    std::ostream& debug_stream () const YY_ATTRIBUTE_PURE;
    /// Set the current debugging stream.
    void set_debug_stream (std::ostream &);

    /// Type for debugging levels.
    typedef int debug_level_type;
    /// The current debugging level.
    debug_level_type debug_level () const YY_ATTRIBUTE_PURE;
    /// Set the current debugging level.
    void set_debug_level (debug_level_type l);
#endif

    /// Report a syntax error.
    /// \param loc    where the syntax error is found.
    /// \param msg    a description of the syntax error.
    virtual void error (const location_type& loc, const std::string& msg);

    /// Report a syntax error.
    void error (const syntax_error& err);

  private:
    /// This class is not copyable.
    Parser (const Parser&);
    Parser& operator= (const Parser&);

    /// State numbers.
    typedef int state_type;

    /// Generate an error message.
    /// \param yystate   the state where the error occurred.
    /// \param yyla      the lookahead token.
    virtual std::string yysyntax_error_ (state_type yystate,
                                         const symbol_type& yyla) const;

    /// Compute post-reduction state.
    /// \param yystate   the current state
    /// \param yysym     the nonterminal to push on the stack
    state_type yy_lr_goto_state_ (state_type yystate, int yysym);

    /// Whether the given \c yypact_ value indicates a defaulted state.
    /// \param yyvalue   the value to check
    static bool yy_pact_value_is_default_ (int yyvalue);

    /// Whether the given \c yytable_ value indicates a syntax error.
    /// \param yyvalue   the value to check
    static bool yy_table_value_is_error_ (int yyvalue);

    static const signed char yypact_ninf_;
    static const signed char yytable_ninf_;

    /// Convert a scanner token number \a t to a symbol number.
    static token_number_type yytranslate_ (token_type t);

    // Tables.
  // YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
  // STATE-NUM.
  static const signed char yypact_[];

  // YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
  // Performed when YYTABLE does not specify something else to do.  Zero
  // means the default is an error.
  static const unsigned char yydefact_[];

  // YYPGOTO[NTERM-NUM].
  static const signed char yypgoto_[];

  // YYDEFGOTO[NTERM-NUM].
  static const signed char yydefgoto_[];

  // YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
  // positive, shift that token.  If negative, reduce the rule whose
  // number is the opposite.  If YYTABLE_NINF, syntax error.
  static const signed char yytable_[];

  static const signed char yycheck_[];

  // YYSTOS[STATE-NUM] -- The (internal number of the) accessing
  // symbol of state STATE-NUM.
  static const unsigned char yystos_[];

  // YYR1[YYN] -- Symbol number of symbol that rule YYN derives.
  static const unsigned char yyr1_[];

  // YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.
  static const unsigned char yyr2_[];


    /// Convert the symbol name \a n to a form suitable for a diagnostic.
    static std::string yytnamerr_ (const char *n);


    /// For a symbol, its name in clear.
    static const char* const yytname_[];
#if YYDEBUG
  // YYRLINE[YYN] -- Source line where rule number YYN was defined.
  static const unsigned short int yyrline_[];
    /// Report on the debug stream that the rule \a r is going to be reduced.
    virtual void yy_reduce_print_ (int r);
    /// Print the state stack on the debug stream.
    virtual void yystack_print_ ();

    // Debugging.
    int yydebug_;
    std::ostream* yycdebug_;

    /// \brief Display a symbol type, value and location.
    /// \param yyo    The output stream.
    /// \param yysym  The symbol.
    template <typename Base>
    void yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const;
#endif

    /// \brief Reclaim the memory associated to a symbol.
    /// \param yymsg     Why this token is reclaimed.
    ///                  If null, print nothing.
    /// \param yysym     The symbol.
    template <typename Base>
    void yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const;

  private:
    /// Type access provider for state based symbols.
    struct by_state
    {
      /// Default constructor.
      by_state ();

      /// The symbol type as needed by the constructor.
      typedef state_type kind_type;

      /// Constructor.
      by_state (kind_type s);

      /// Copy constructor.
      by_state (const by_state& other);

      /// Record that this symbol is empty.
      void clear ();

      /// Steal the symbol type from \a that.
      void move (by_state& that);

      /// The (internal) type number (corresponding to \a state).
      /// \a empty_symbol when empty.
      symbol_number_type type_get () const;

      /// The state number used to denote an empty symbol.
      enum { empty_state = -1 };

      /// The state.
      /// \a empty when empty.
      state_type state;
    };

    /// "Internal" symbol: element of the stack.
    struct stack_symbol_type : basic_symbol<by_state>
    {
      /// Superclass.
      typedef basic_symbol<by_state> super_type;
      /// Construct an empty symbol.
      stack_symbol_type ();
      /// Steal the contents from \a sym to build this.
      stack_symbol_type (state_type s, symbol_type& sym);
      /// Assignment, needed by push_back.
      stack_symbol_type& operator= (const stack_symbol_type& that);
    };

    /// Stack type.
    typedef stack<stack_symbol_type> stack_type;

    /// The stack.
    stack_type yystack_;

    /// Push a new state on the stack.
    /// \param m    a debug message to display
    ///             if null, no trace is output.
    /// \param s    the symbol
    /// \warning the contents of \a s.value is stolen.
    void yypush_ (const char* m, stack_symbol_type& s);

    /// Push a new look ahead token on the state on the stack.
    /// \param m    a debug message to display
    ///             if null, no trace is output.
    /// \param s    the state
    /// \param sym  the symbol (for its value and location).
    /// \warning the contents of \a s.value is stolen.
    void yypush_ (const char* m, state_type s, symbol_type& sym);

    /// Pop \a n symbols the three stacks.
    void yypop_ (unsigned n = 1);

    /// Constants.
    enum
    {
      yyeof_ = 0,
      yylast_ = 108,     ///< Last index in yytable_.
      yynnts_ = 30,  ///< Number of nonterminal symbols.
      yyfinal_ = 3, ///< Termination state number.
      yyterror_ = 1,
      yyerrcode_ = 256,
      yyntokens_ = 29  ///< Number of tokens.
    };


    // User arguments.
    ParserDriver& driver;
  };

  // Symbol number corresponding to token number t.
  inline
  Parser::token_number_type
  Parser::yytranslate_ (token_type t)
  {
    static
    const token_number_type
    translate_table[] =
    {
     0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28
    };
    const unsigned int user_token_number_max_ = 283;
    const token_number_type undef_token_ = 2;

    if (static_cast<int>(t) <= yyeof_)
      return yyeof_;
    else if (static_cast<unsigned int> (t) <= user_token_number_max_)
      return translate_table[t];
    else
      return undef_token_;
  }

  inline
  Parser::syntax_error::syntax_error (const location_type& l, const std::string& m)
    : std::runtime_error (m)
    , location (l)
  {}

  // basic_symbol.
  template <typename Base>
  inline
  Parser::basic_symbol<Base>::basic_symbol ()
    : value ()
  {}

  template <typename Base>
  inline
  Parser::basic_symbol<Base>::basic_symbol (const basic_symbol& other)
    : Base (other)
    , value ()
    , location (other.location)
  {
      switch (other.type_get ())
    {
      case 54: // deductionTarget
        value.copy< ASTDeductionTarget > (other.value);
        break;

      case 56: // deduction_target_array
        value.copy< ASTDeductionTargetArray > (other.value);
        break;

      case 57: // deduction_target_computed
        value.copy< ASTDeductionTargetComputed > (other.value);
        break;

      case 53: // deduction_target_list
        value.copy< ASTDeductionTargetList > (other.value);
        break;

      case 55: // deduction_target_singular
        value.copy< ASTDeductionTargetSingular > (other.value);
        break;

      case 34: // environment_defn
        value.copy< ASTEnvironmentDefn > (other.value);
        break;

      case 33: // environment_defn_list
        value.copy< ASTEnvironmentDefnList > (other.value);
        break;

      case 39: // global_decl
        value.copy< ASTGlobalDecl > (other.value);
        break;

      case 37: // global_decl_set
      case 38: // global_decl_list
        value.copy< ASTGlobalDeclList > (other.value);
        break;

      case 51: // identifiable
        value.copy< ASTIdentifiable > (other.value);
        break;

      case 52: // identifier
        value.copy< ASTIdentifier > (other.value);
        break;

      case 42: // inference_argument
        value.copy< ASTInferenceArgument > (other.value);
        break;

      case 40: // argument_set
      case 41: // argument_list
        value.copy< ASTInferenceArgumentList > (other.value);
        break;

      case 36: // inference_defn
        value.copy< ASTInferenceDefn > (other.value);
        break;

      case 35: // inference_defn_list
        value.copy< ASTInferenceDefnList > (other.value);
        break;

      case 48: // premise_type_equality_defn
        value.copy< ASTInferenceEqualityDefn > (other.value);
        break;

      case 32: // inference_group
        value.copy< ASTInferenceGroup > (other.value);
        break;

      case 31: // inference_group_list
        value.copy< ASTInferenceGroupList > (other.value);
        break;

      case 46: // premise_type_inference_defn
        value.copy< ASTInferencePremiseDefn > (other.value);
        break;

      case 30: // input
        value.copy< ASTModule > (other.value);
        break;

      case 45: // premise_defn
        value.copy< ASTPremiseDefn > (other.value);
        break;

      case 43: // premise_set
      case 44: // premise_defn_list
        value.copy< ASTPremiseDefnList > (other.value);
        break;

      case 50: // proposition_defn
        value.copy< ASTPropositionDefn > (other.value);
        break;

      case 49: // range_clause
        value.copy< ASTRangeClause > (other.value);
        break;

      case 47: // while_clause
        value.copy< ASTWhileClause > (other.value);
        break;

      case 58: // equality_operator
        value.copy< EqualityOperator > (other.value);
        break;

      case 3: // KEYWORD_GROUP
      case 4: // KEYWORD_INFERENCE
      case 5: // KEYWORD_ENVIRONMENT
      case 6: // KEYWORD_ARGUMENTS
      case 7: // KEYWORD_GLOBALS
      case 8: // KEYWORD_WHILE
      case 9: // KEYWORD_INRANGE
      case 10: // KEYWORD_PREMISES
      case 11: // KEYWORD_PROPOSITION
      case 13: // IDENTIFIER
      case 14: // DOT
      case 15: // COMMA
      case 16: // COLON
      case 17: // SEMICOLON
      case 18: // LBRACKET
      case 19: // RBRACKET
      case 20: // LBRACE
      case 21: // RBRACE
      case 22: // LPAREN
      case 23: // RPAREN
      case 24: // OPERATOR_EQ
      case 25: // OPERATOR_NEQ
      case 26: // OPERATOR_LT
      case 27: // OPERATOR_LTE
      case 28: // ELLIPSIS
        value.copy< std::string > (other.value);
        break;

      case 12: // INTEGER_LITERAL
        value.copy< uint64_t > (other.value);
        break;

      default:
        break;
    }

  }

  template <typename Base>
  inline
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const semantic_type& v, const location_type& l)
    : Base (t)
    , value ()
    , location (l)
  {
    (void) v;
      switch (this->type_get ())
    {
      case 54: // deductionTarget
        value.copy< ASTDeductionTarget > (v);
        break;

      case 56: // deduction_target_array
        value.copy< ASTDeductionTargetArray > (v);
        break;

      case 57: // deduction_target_computed
        value.copy< ASTDeductionTargetComputed > (v);
        break;

      case 53: // deduction_target_list
        value.copy< ASTDeductionTargetList > (v);
        break;

      case 55: // deduction_target_singular
        value.copy< ASTDeductionTargetSingular > (v);
        break;

      case 34: // environment_defn
        value.copy< ASTEnvironmentDefn > (v);
        break;

      case 33: // environment_defn_list
        value.copy< ASTEnvironmentDefnList > (v);
        break;

      case 39: // global_decl
        value.copy< ASTGlobalDecl > (v);
        break;

      case 37: // global_decl_set
      case 38: // global_decl_list
        value.copy< ASTGlobalDeclList > (v);
        break;

      case 51: // identifiable
        value.copy< ASTIdentifiable > (v);
        break;

      case 52: // identifier
        value.copy< ASTIdentifier > (v);
        break;

      case 42: // inference_argument
        value.copy< ASTInferenceArgument > (v);
        break;

      case 40: // argument_set
      case 41: // argument_list
        value.copy< ASTInferenceArgumentList > (v);
        break;

      case 36: // inference_defn
        value.copy< ASTInferenceDefn > (v);
        break;

      case 35: // inference_defn_list
        value.copy< ASTInferenceDefnList > (v);
        break;

      case 48: // premise_type_equality_defn
        value.copy< ASTInferenceEqualityDefn > (v);
        break;

      case 32: // inference_group
        value.copy< ASTInferenceGroup > (v);
        break;

      case 31: // inference_group_list
        value.copy< ASTInferenceGroupList > (v);
        break;

      case 46: // premise_type_inference_defn
        value.copy< ASTInferencePremiseDefn > (v);
        break;

      case 30: // input
        value.copy< ASTModule > (v);
        break;

      case 45: // premise_defn
        value.copy< ASTPremiseDefn > (v);
        break;

      case 43: // premise_set
      case 44: // premise_defn_list
        value.copy< ASTPremiseDefnList > (v);
        break;

      case 50: // proposition_defn
        value.copy< ASTPropositionDefn > (v);
        break;

      case 49: // range_clause
        value.copy< ASTRangeClause > (v);
        break;

      case 47: // while_clause
        value.copy< ASTWhileClause > (v);
        break;

      case 58: // equality_operator
        value.copy< EqualityOperator > (v);
        break;

      case 3: // KEYWORD_GROUP
      case 4: // KEYWORD_INFERENCE
      case 5: // KEYWORD_ENVIRONMENT
      case 6: // KEYWORD_ARGUMENTS
      case 7: // KEYWORD_GLOBALS
      case 8: // KEYWORD_WHILE
      case 9: // KEYWORD_INRANGE
      case 10: // KEYWORD_PREMISES
      case 11: // KEYWORD_PROPOSITION
      case 13: // IDENTIFIER
      case 14: // DOT
      case 15: // COMMA
      case 16: // COLON
      case 17: // SEMICOLON
      case 18: // LBRACKET
      case 19: // RBRACKET
      case 20: // LBRACE
      case 21: // RBRACE
      case 22: // LPAREN
      case 23: // RPAREN
      case 24: // OPERATOR_EQ
      case 25: // OPERATOR_NEQ
      case 26: // OPERATOR_LT
      case 27: // OPERATOR_LTE
      case 28: // ELLIPSIS
        value.copy< std::string > (v);
        break;

      case 12: // INTEGER_LITERAL
        value.copy< uint64_t > (v);
        break;

      default:
        break;
    }
}


  // Implementation of basic_symbol constructor for each type.

  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const location_type& l)
    : Base (t)
    , value ()
    , location (l)
  {}

  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const ASTDeductionTarget v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const ASTDeductionTargetArray v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const ASTDeductionTargetComputed v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const ASTDeductionTargetList v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const ASTDeductionTargetSingular v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const ASTEnvironmentDefn v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const ASTEnvironmentDefnList v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const ASTGlobalDecl v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const ASTGlobalDeclList v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const ASTIdentifiable v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const ASTIdentifier v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const ASTInferenceArgument v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const ASTInferenceArgumentList v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const ASTInferenceDefn v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const ASTInferenceDefnList v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const ASTInferenceEqualityDefn v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const ASTInferenceGroup v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const ASTInferenceGroupList v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const ASTInferencePremiseDefn v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const ASTModule v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const ASTPremiseDefn v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const ASTPremiseDefnList v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const ASTPropositionDefn v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const ASTRangeClause v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const ASTWhileClause v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const EqualityOperator v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const std::string v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const uint64_t v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}


  template <typename Base>
  inline
  Parser::basic_symbol<Base>::~basic_symbol ()
  {
    clear ();
  }

  template <typename Base>
  inline
  void
  Parser::basic_symbol<Base>::clear ()
  {
    // User destructor.
    symbol_number_type yytype = this->type_get ();
    basic_symbol<Base>& yysym = *this;
    (void) yysym;
    switch (yytype)
    {
   default:
      break;
    }

    // Type destructor.
    switch (yytype)
    {
      case 54: // deductionTarget
        value.template destroy< ASTDeductionTarget > ();
        break;

      case 56: // deduction_target_array
        value.template destroy< ASTDeductionTargetArray > ();
        break;

      case 57: // deduction_target_computed
        value.template destroy< ASTDeductionTargetComputed > ();
        break;

      case 53: // deduction_target_list
        value.template destroy< ASTDeductionTargetList > ();
        break;

      case 55: // deduction_target_singular
        value.template destroy< ASTDeductionTargetSingular > ();
        break;

      case 34: // environment_defn
        value.template destroy< ASTEnvironmentDefn > ();
        break;

      case 33: // environment_defn_list
        value.template destroy< ASTEnvironmentDefnList > ();
        break;

      case 39: // global_decl
        value.template destroy< ASTGlobalDecl > ();
        break;

      case 37: // global_decl_set
      case 38: // global_decl_list
        value.template destroy< ASTGlobalDeclList > ();
        break;

      case 51: // identifiable
        value.template destroy< ASTIdentifiable > ();
        break;

      case 52: // identifier
        value.template destroy< ASTIdentifier > ();
        break;

      case 42: // inference_argument
        value.template destroy< ASTInferenceArgument > ();
        break;

      case 40: // argument_set
      case 41: // argument_list
        value.template destroy< ASTInferenceArgumentList > ();
        break;

      case 36: // inference_defn
        value.template destroy< ASTInferenceDefn > ();
        break;

      case 35: // inference_defn_list
        value.template destroy< ASTInferenceDefnList > ();
        break;

      case 48: // premise_type_equality_defn
        value.template destroy< ASTInferenceEqualityDefn > ();
        break;

      case 32: // inference_group
        value.template destroy< ASTInferenceGroup > ();
        break;

      case 31: // inference_group_list
        value.template destroy< ASTInferenceGroupList > ();
        break;

      case 46: // premise_type_inference_defn
        value.template destroy< ASTInferencePremiseDefn > ();
        break;

      case 30: // input
        value.template destroy< ASTModule > ();
        break;

      case 45: // premise_defn
        value.template destroy< ASTPremiseDefn > ();
        break;

      case 43: // premise_set
      case 44: // premise_defn_list
        value.template destroy< ASTPremiseDefnList > ();
        break;

      case 50: // proposition_defn
        value.template destroy< ASTPropositionDefn > ();
        break;

      case 49: // range_clause
        value.template destroy< ASTRangeClause > ();
        break;

      case 47: // while_clause
        value.template destroy< ASTWhileClause > ();
        break;

      case 58: // equality_operator
        value.template destroy< EqualityOperator > ();
        break;

      case 3: // KEYWORD_GROUP
      case 4: // KEYWORD_INFERENCE
      case 5: // KEYWORD_ENVIRONMENT
      case 6: // KEYWORD_ARGUMENTS
      case 7: // KEYWORD_GLOBALS
      case 8: // KEYWORD_WHILE
      case 9: // KEYWORD_INRANGE
      case 10: // KEYWORD_PREMISES
      case 11: // KEYWORD_PROPOSITION
      case 13: // IDENTIFIER
      case 14: // DOT
      case 15: // COMMA
      case 16: // COLON
      case 17: // SEMICOLON
      case 18: // LBRACKET
      case 19: // RBRACKET
      case 20: // LBRACE
      case 21: // RBRACE
      case 22: // LPAREN
      case 23: // RPAREN
      case 24: // OPERATOR_EQ
      case 25: // OPERATOR_NEQ
      case 26: // OPERATOR_LT
      case 27: // OPERATOR_LTE
      case 28: // ELLIPSIS
        value.template destroy< std::string > ();
        break;

      case 12: // INTEGER_LITERAL
        value.template destroy< uint64_t > ();
        break;

      default:
        break;
    }

    Base::clear ();
  }

  template <typename Base>
  inline
  bool
  Parser::basic_symbol<Base>::empty () const
  {
    return Base::type_get () == empty_symbol;
  }

  template <typename Base>
  inline
  void
  Parser::basic_symbol<Base>::move (basic_symbol& s)
  {
    super_type::move(s);
      switch (this->type_get ())
    {
      case 54: // deductionTarget
        value.move< ASTDeductionTarget > (s.value);
        break;

      case 56: // deduction_target_array
        value.move< ASTDeductionTargetArray > (s.value);
        break;

      case 57: // deduction_target_computed
        value.move< ASTDeductionTargetComputed > (s.value);
        break;

      case 53: // deduction_target_list
        value.move< ASTDeductionTargetList > (s.value);
        break;

      case 55: // deduction_target_singular
        value.move< ASTDeductionTargetSingular > (s.value);
        break;

      case 34: // environment_defn
        value.move< ASTEnvironmentDefn > (s.value);
        break;

      case 33: // environment_defn_list
        value.move< ASTEnvironmentDefnList > (s.value);
        break;

      case 39: // global_decl
        value.move< ASTGlobalDecl > (s.value);
        break;

      case 37: // global_decl_set
      case 38: // global_decl_list
        value.move< ASTGlobalDeclList > (s.value);
        break;

      case 51: // identifiable
        value.move< ASTIdentifiable > (s.value);
        break;

      case 52: // identifier
        value.move< ASTIdentifier > (s.value);
        break;

      case 42: // inference_argument
        value.move< ASTInferenceArgument > (s.value);
        break;

      case 40: // argument_set
      case 41: // argument_list
        value.move< ASTInferenceArgumentList > (s.value);
        break;

      case 36: // inference_defn
        value.move< ASTInferenceDefn > (s.value);
        break;

      case 35: // inference_defn_list
        value.move< ASTInferenceDefnList > (s.value);
        break;

      case 48: // premise_type_equality_defn
        value.move< ASTInferenceEqualityDefn > (s.value);
        break;

      case 32: // inference_group
        value.move< ASTInferenceGroup > (s.value);
        break;

      case 31: // inference_group_list
        value.move< ASTInferenceGroupList > (s.value);
        break;

      case 46: // premise_type_inference_defn
        value.move< ASTInferencePremiseDefn > (s.value);
        break;

      case 30: // input
        value.move< ASTModule > (s.value);
        break;

      case 45: // premise_defn
        value.move< ASTPremiseDefn > (s.value);
        break;

      case 43: // premise_set
      case 44: // premise_defn_list
        value.move< ASTPremiseDefnList > (s.value);
        break;

      case 50: // proposition_defn
        value.move< ASTPropositionDefn > (s.value);
        break;

      case 49: // range_clause
        value.move< ASTRangeClause > (s.value);
        break;

      case 47: // while_clause
        value.move< ASTWhileClause > (s.value);
        break;

      case 58: // equality_operator
        value.move< EqualityOperator > (s.value);
        break;

      case 3: // KEYWORD_GROUP
      case 4: // KEYWORD_INFERENCE
      case 5: // KEYWORD_ENVIRONMENT
      case 6: // KEYWORD_ARGUMENTS
      case 7: // KEYWORD_GLOBALS
      case 8: // KEYWORD_WHILE
      case 9: // KEYWORD_INRANGE
      case 10: // KEYWORD_PREMISES
      case 11: // KEYWORD_PROPOSITION
      case 13: // IDENTIFIER
      case 14: // DOT
      case 15: // COMMA
      case 16: // COLON
      case 17: // SEMICOLON
      case 18: // LBRACKET
      case 19: // RBRACKET
      case 20: // LBRACE
      case 21: // RBRACE
      case 22: // LPAREN
      case 23: // RPAREN
      case 24: // OPERATOR_EQ
      case 25: // OPERATOR_NEQ
      case 26: // OPERATOR_LT
      case 27: // OPERATOR_LTE
      case 28: // ELLIPSIS
        value.move< std::string > (s.value);
        break;

      case 12: // INTEGER_LITERAL
        value.move< uint64_t > (s.value);
        break;

      default:
        break;
    }

    location = s.location;
  }

  // by_type.
  inline
  Parser::by_type::by_type ()
    : type (empty_symbol)
  {}

  inline
  Parser::by_type::by_type (const by_type& other)
    : type (other.type)
  {}

  inline
  Parser::by_type::by_type (token_type t)
    : type (yytranslate_ (t))
  {}

  inline
  void
  Parser::by_type::clear ()
  {
    type = empty_symbol;
  }

  inline
  void
  Parser::by_type::move (by_type& that)
  {
    type = that.type;
    that.clear ();
  }

  inline
  int
  Parser::by_type::type_get () const
  {
    return type;
  }

  inline
  Parser::token_type
  Parser::by_type::token () const
  {
    // YYTOKNUM[NUM] -- (External) token number corresponding to the
    // (internal) symbol number NUM (which must be that of a token).  */
    static
    const unsigned short int
    yytoken_number_[] =
    {
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283
    };
    return static_cast<token_type> (yytoken_number_[type]);
  }
  // Implementation of make_symbol for each symbol type.
  Parser::symbol_type
  Parser::make_END (const location_type& l)
  {
    return symbol_type (token::END, l);
  }

  Parser::symbol_type
  Parser::make_KEYWORD_GROUP (const std::string& v, const location_type& l)
  {
    return symbol_type (token::KEYWORD_GROUP, v, l);
  }

  Parser::symbol_type
  Parser::make_KEYWORD_INFERENCE (const std::string& v, const location_type& l)
  {
    return symbol_type (token::KEYWORD_INFERENCE, v, l);
  }

  Parser::symbol_type
  Parser::make_KEYWORD_ENVIRONMENT (const std::string& v, const location_type& l)
  {
    return symbol_type (token::KEYWORD_ENVIRONMENT, v, l);
  }

  Parser::symbol_type
  Parser::make_KEYWORD_ARGUMENTS (const std::string& v, const location_type& l)
  {
    return symbol_type (token::KEYWORD_ARGUMENTS, v, l);
  }

  Parser::symbol_type
  Parser::make_KEYWORD_GLOBALS (const std::string& v, const location_type& l)
  {
    return symbol_type (token::KEYWORD_GLOBALS, v, l);
  }

  Parser::symbol_type
  Parser::make_KEYWORD_WHILE (const std::string& v, const location_type& l)
  {
    return symbol_type (token::KEYWORD_WHILE, v, l);
  }

  Parser::symbol_type
  Parser::make_KEYWORD_INRANGE (const std::string& v, const location_type& l)
  {
    return symbol_type (token::KEYWORD_INRANGE, v, l);
  }

  Parser::symbol_type
  Parser::make_KEYWORD_PREMISES (const std::string& v, const location_type& l)
  {
    return symbol_type (token::KEYWORD_PREMISES, v, l);
  }

  Parser::symbol_type
  Parser::make_KEYWORD_PROPOSITION (const std::string& v, const location_type& l)
  {
    return symbol_type (token::KEYWORD_PROPOSITION, v, l);
  }

  Parser::symbol_type
  Parser::make_INTEGER_LITERAL (const uint64_t& v, const location_type& l)
  {
    return symbol_type (token::INTEGER_LITERAL, v, l);
  }

  Parser::symbol_type
  Parser::make_IDENTIFIER (const std::string& v, const location_type& l)
  {
    return symbol_type (token::IDENTIFIER, v, l);
  }

  Parser::symbol_type
  Parser::make_DOT (const std::string& v, const location_type& l)
  {
    return symbol_type (token::DOT, v, l);
  }

  Parser::symbol_type
  Parser::make_COMMA (const std::string& v, const location_type& l)
  {
    return symbol_type (token::COMMA, v, l);
  }

  Parser::symbol_type
  Parser::make_COLON (const std::string& v, const location_type& l)
  {
    return symbol_type (token::COLON, v, l);
  }

  Parser::symbol_type
  Parser::make_SEMICOLON (const std::string& v, const location_type& l)
  {
    return symbol_type (token::SEMICOLON, v, l);
  }

  Parser::symbol_type
  Parser::make_LBRACKET (const std::string& v, const location_type& l)
  {
    return symbol_type (token::LBRACKET, v, l);
  }

  Parser::symbol_type
  Parser::make_RBRACKET (const std::string& v, const location_type& l)
  {
    return symbol_type (token::RBRACKET, v, l);
  }

  Parser::symbol_type
  Parser::make_LBRACE (const std::string& v, const location_type& l)
  {
    return symbol_type (token::LBRACE, v, l);
  }

  Parser::symbol_type
  Parser::make_RBRACE (const std::string& v, const location_type& l)
  {
    return symbol_type (token::RBRACE, v, l);
  }

  Parser::symbol_type
  Parser::make_LPAREN (const std::string& v, const location_type& l)
  {
    return symbol_type (token::LPAREN, v, l);
  }

  Parser::symbol_type
  Parser::make_RPAREN (const std::string& v, const location_type& l)
  {
    return symbol_type (token::RPAREN, v, l);
  }

  Parser::symbol_type
  Parser::make_OPERATOR_EQ (const std::string& v, const location_type& l)
  {
    return symbol_type (token::OPERATOR_EQ, v, l);
  }

  Parser::symbol_type
  Parser::make_OPERATOR_NEQ (const std::string& v, const location_type& l)
  {
    return symbol_type (token::OPERATOR_NEQ, v, l);
  }

  Parser::symbol_type
  Parser::make_OPERATOR_LT (const std::string& v, const location_type& l)
  {
    return symbol_type (token::OPERATOR_LT, v, l);
  }

  Parser::symbol_type
  Parser::make_OPERATOR_LTE (const std::string& v, const location_type& l)
  {
    return symbol_type (token::OPERATOR_LTE, v, l);
  }

  Parser::symbol_type
  Parser::make_ELLIPSIS (const std::string& v, const location_type& l)
  {
    return symbol_type (token::ELLIPSIS, v, l);
  }



} // yy
#line 2069 "parser.tab.hh" // lalr1.cc:392




#endif // !YY_YY_PARSER_TAB_HH_INCLUDED
