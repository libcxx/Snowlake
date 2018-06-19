// A Bison parser, made by GNU Bison 3.0.4.23-dbd16.

// Skeleton implementation for Bison LALR(1) parsers in C++

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


// First part of user declarations.

#line 37 "parser.tab.cc" // lalr1.cc:404

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

#include "parser.tab.hh"

// User implementation prologue.

#line 51 "parser.tab.cc" // lalr1.cc:412
// Unqualified %code blocks.
#line 60 "/Users/x/workspace/snowlake/src/parser/parser.yy" // lalr1.cc:413

// HACK: We need to refer to the header under the source directory
// from the build directory.
// TODO: Currently this is the only hack that I can think of.
// Will need to devise a cleaner solution eventually.
#include "../../../src/parser/ParserDriver.h"
#include "lex.yy.hh"

#line 62 "parser.tab.cc" // lalr1.cc:413


#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K].location)
/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

# ifndef YYLLOC_DEFAULT
#  define YYLLOC_DEFAULT(Current, Rhs, N)                               \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).begin  = YYRHSLOC (Rhs, 1).begin;                   \
          (Current).end    = YYRHSLOC (Rhs, N).end;                     \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).begin = (Current).end = YYRHSLOC (Rhs, 0).end;      \
        }                                                               \
    while (/*CONSTCOND*/ false)
# endif


// Suppress unused-variable warnings by "using" E.
#define YYUSE(E) ((void) (E))

// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << '\n';                       \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yystack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YYUSE(Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void>(0)
# define YY_STACK_PRINT()                static_cast<void>(0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)


namespace yy {
#line 148 "parser.tab.cc" // lalr1.cc:479

  /* Return YYSTR after stripping away unnecessary quotes and
     backslashes, so that it's suitable for yyerror.  The heuristic is
     that double-quoting is unnecessary unless the string contains an
     apostrophe, a comma, or backslash (other than backslash-backslash).
     YYSTR is taken from yytname.  */
  std::string
  Parser::yytnamerr_ (const char *yystr)
  {
    if (*yystr == '"')
      {
        std::string yyr = "";
        char const *yyp = yystr;

        for (;;)
          switch (*++yyp)
            {
            case '\'':
            case ',':
              goto do_not_strip_quotes;

            case '\\':
              if (*++yyp != '\\')
                goto do_not_strip_quotes;
              // Fall through.
            default:
              yyr += *yyp;
              break;

            case '"':
              return yyr;
            }
      do_not_strip_quotes: ;
      }

    return yystr;
  }


  /// Build a parser object.
  Parser::Parser (ParserDriver& driver_yyarg)
    :
#if YYDEBUG
      yydebug_ (false),
      yycdebug_ (&std::cerr),
#endif
      driver (driver_yyarg)
  {}

  Parser::~Parser ()
  {}


  /*---------------.
  | Symbol types.  |
  `---------------*/



  // by_state.
  inline
  Parser::by_state::by_state ()
    : state (empty_state)
  {}

  inline
  Parser::by_state::by_state (const by_state& other)
    : state (other.state)
  {}

  inline
  void
  Parser::by_state::clear ()
  {
    state = empty_state;
  }

  inline
  void
  Parser::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  inline
  Parser::by_state::by_state (state_type s)
    : state (s)
  {}

  inline
  Parser::symbol_number_type
  Parser::by_state::type_get () const
  {
    if (state == empty_state)
      return empty_symbol;
    else
      return yystos_[state];
  }

  inline
  Parser::stack_symbol_type::stack_symbol_type ()
  {}


  inline
  Parser::stack_symbol_type::stack_symbol_type (state_type s, symbol_type& that)
    : super_type (s, that.location)
  {
      switch (that.type_get ())
    {
      case 54: // deduction_target
        value.move< ASTDeductionTarget > (that.value);
        break;

      case 56: // deduction_target_array
        value.move< ASTDeductionTargetArray > (that.value);
        break;

      case 57: // deduction_target_computed
        value.move< ASTDeductionTargetComputed > (that.value);
        break;

      case 53: // deduction_target_list
        value.move< ASTDeductionTargetList > (that.value);
        break;

      case 55: // deduction_target_singular
        value.move< ASTDeductionTargetSingular > (that.value);
        break;

      case 34: // environment_defn
        value.move< ASTEnvironmentDefn > (that.value);
        break;

      case 33: // environment_defn_list
        value.move< ASTEnvironmentDefnList > (that.value);
        break;

      case 39: // global_decl
        value.move< ASTGlobalDecl > (that.value);
        break;

      case 37: // global_decl_set
      case 38: // global_decl_list
        value.move< ASTGlobalDeclList > (that.value);
        break;

      case 51: // identifiable
        value.move< ASTIdentifiable > (that.value);
        break;

      case 52: // identifier
        value.move< ASTIdentifier > (that.value);
        break;

      case 42: // inference_argument
        value.move< ASTInferenceArgument > (that.value);
        break;

      case 40: // argument_set
      case 41: // argument_list
        value.move< ASTInferenceArgumentList > (that.value);
        break;

      case 36: // inference_defn
        value.move< ASTInferenceDefn > (that.value);
        break;

      case 35: // inference_defn_list
        value.move< ASTInferenceDefnList > (that.value);
        break;

      case 48: // premise_type_equality_defn
        value.move< ASTInferenceEqualityDefn > (that.value);
        break;

      case 32: // inference_group
        value.move< ASTInferenceGroup > (that.value);
        break;

      case 31: // inference_group_list
        value.move< ASTInferenceGroupList > (that.value);
        break;

      case 46: // premise_type_inference_defn
        value.move< ASTInferencePremiseDefn > (that.value);
        break;

      case 30: // input
        value.move< ASTModule > (that.value);
        break;

      case 45: // premise_defn
        value.move< ASTPremiseDefn > (that.value);
        break;

      case 43: // premise_set
      case 44: // premise_defn_list
        value.move< ASTPremiseDefnList > (that.value);
        break;

      case 50: // proposition_defn
        value.move< ASTPropositionDefn > (that.value);
        break;

      case 49: // range_clause
        value.move< ASTRangeClause > (that.value);
        break;

      case 47: // while_clause
        value.move< ASTWhileClause > (that.value);
        break;

      case 58: // equality_operator
        value.move< EqualityOperator > (that.value);
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
        value.move< std::string > (that.value);
        break;

      case 12: // INTEGER_LITERAL
        value.move< uint64_t > (that.value);
        break;

      default:
        break;
    }

    // that is emptied.
    that.type = empty_symbol;
  }

  inline
  Parser::stack_symbol_type&
  Parser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
      switch (that.type_get ())
    {
      case 54: // deduction_target
        value.copy< ASTDeductionTarget > (that.value);
        break;

      case 56: // deduction_target_array
        value.copy< ASTDeductionTargetArray > (that.value);
        break;

      case 57: // deduction_target_computed
        value.copy< ASTDeductionTargetComputed > (that.value);
        break;

      case 53: // deduction_target_list
        value.copy< ASTDeductionTargetList > (that.value);
        break;

      case 55: // deduction_target_singular
        value.copy< ASTDeductionTargetSingular > (that.value);
        break;

      case 34: // environment_defn
        value.copy< ASTEnvironmentDefn > (that.value);
        break;

      case 33: // environment_defn_list
        value.copy< ASTEnvironmentDefnList > (that.value);
        break;

      case 39: // global_decl
        value.copy< ASTGlobalDecl > (that.value);
        break;

      case 37: // global_decl_set
      case 38: // global_decl_list
        value.copy< ASTGlobalDeclList > (that.value);
        break;

      case 51: // identifiable
        value.copy< ASTIdentifiable > (that.value);
        break;

      case 52: // identifier
        value.copy< ASTIdentifier > (that.value);
        break;

      case 42: // inference_argument
        value.copy< ASTInferenceArgument > (that.value);
        break;

      case 40: // argument_set
      case 41: // argument_list
        value.copy< ASTInferenceArgumentList > (that.value);
        break;

      case 36: // inference_defn
        value.copy< ASTInferenceDefn > (that.value);
        break;

      case 35: // inference_defn_list
        value.copy< ASTInferenceDefnList > (that.value);
        break;

      case 48: // premise_type_equality_defn
        value.copy< ASTInferenceEqualityDefn > (that.value);
        break;

      case 32: // inference_group
        value.copy< ASTInferenceGroup > (that.value);
        break;

      case 31: // inference_group_list
        value.copy< ASTInferenceGroupList > (that.value);
        break;

      case 46: // premise_type_inference_defn
        value.copy< ASTInferencePremiseDefn > (that.value);
        break;

      case 30: // input
        value.copy< ASTModule > (that.value);
        break;

      case 45: // premise_defn
        value.copy< ASTPremiseDefn > (that.value);
        break;

      case 43: // premise_set
      case 44: // premise_defn_list
        value.copy< ASTPremiseDefnList > (that.value);
        break;

      case 50: // proposition_defn
        value.copy< ASTPropositionDefn > (that.value);
        break;

      case 49: // range_clause
        value.copy< ASTRangeClause > (that.value);
        break;

      case 47: // while_clause
        value.copy< ASTWhileClause > (that.value);
        break;

      case 58: // equality_operator
        value.copy< EqualityOperator > (that.value);
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
        value.copy< std::string > (that.value);
        break;

      case 12: // INTEGER_LITERAL
        value.copy< uint64_t > (that.value);
        break;

      default:
        break;
    }

    location = that.location;
    return *this;
  }


  template <typename Base>
  inline
  void
  Parser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);
  }

#if YYDEBUG
  template <typename Base>
  void
  Parser::yy_print_ (std::ostream& yyo,
                                     const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YYUSE (yyoutput);
    symbol_number_type yytype = yysym.type_get ();
    // Avoid a (spurious) G++ 4.8 warning about "array subscript is
    // below array bounds".
    if (yysym.empty ())
      std::abort ();
    yyo << (yytype < yyntokens_ ? "token" : "nterm")
        << ' ' << yytname_[yytype] << " ("
        << yysym.location << ": ";
    YYUSE (yytype);
    yyo << ')';
  }
#endif

  inline
  void
  Parser::yypush_ (const char* m, state_type s, symbol_type& sym)
  {
    stack_symbol_type t (s, sym);
    yypush_ (m, t);
  }

  inline
  void
  Parser::yypush_ (const char* m, stack_symbol_type& s)
  {
    if (m)
      YY_SYMBOL_PRINT (m, s);
    yystack_.push (s);
  }

  inline
  void
  Parser::yypop_ (unsigned n)
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  Parser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  Parser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  Parser::debug_level_type
  Parser::debug_level () const
  {
    return yydebug_;
  }

  void
  Parser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

  inline Parser::state_type
  Parser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - yyntokens_] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - yyntokens_];
  }

  inline bool
  Parser::yy_pact_value_is_default_ (int yyvalue)
  {
    return yyvalue == yypact_ninf_;
  }

  inline bool
  Parser::yy_table_value_is_error_ (int yyvalue)
  {
    return yyvalue == yytable_ninf_;
  }

  int
  Parser::parse ()
  {
    // State.
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The locations where the error started and ended.
    stack_symbol_type yyerror_range[3];

    /// The return value of parse ().
    int yyresult;

    // FIXME: This shoud be completely indented.  It is not yet to
    // avoid gratuitous conflicts when merging into the master branch.
    try
      {
    YYCDEBUG << "Starting parse\n";


    // User initialization code.
    #line 50 "/Users/x/workspace/snowlake/src/parser/parser.yy" // lalr1.cc:745
{
  // Initialize the initial location.
  if (!driver.input_file().empty())
  {
    yyla.location.begin.filename = yyla.location.end.filename = &driver.input_file();
  }
}

#line 703 "parser.tab.cc" // lalr1.cc:745

    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, yyla);

    // A new symbol was pushed on the stack.
  yynewstate:
    YYCDEBUG << "Entering state " << yystack_[0].state << '\n';

    // Accept?
    if (yystack_[0].state == yyfinal_)
      goto yyacceptlab;

    goto yybackup;

    // Backup.
  yybackup:

    // Try to take a decision without lookahead.
    yyn = yypact_[yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token: ";
        try
          {
            symbol_type yylookahead (yylex (driver));
            yyla.move (yylookahead);
          }
        catch (const syntax_error& yyexc)
          {
            error (yyexc);
            goto yyerrlab1;
          }
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.type_get ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.type_get ())
      goto yydefault;

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", yyn, yyla);
    goto yynewstate;

  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;

  /*-----------------------------.
  | yyreduce -- Do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_ (yystack_[yylen].state, yyr1_[yyn]);
      /* Variants are always initialized to an empty instance of the
         correct type. The default '$$ = $1' action is NOT applied
         when using variants.  */
        switch (yyr1_[yyn])
    {
      case 54: // deduction_target
        yylhs.value.build< ASTDeductionTarget > ();
        break;

      case 56: // deduction_target_array
        yylhs.value.build< ASTDeductionTargetArray > ();
        break;

      case 57: // deduction_target_computed
        yylhs.value.build< ASTDeductionTargetComputed > ();
        break;

      case 53: // deduction_target_list
        yylhs.value.build< ASTDeductionTargetList > ();
        break;

      case 55: // deduction_target_singular
        yylhs.value.build< ASTDeductionTargetSingular > ();
        break;

      case 34: // environment_defn
        yylhs.value.build< ASTEnvironmentDefn > ();
        break;

      case 33: // environment_defn_list
        yylhs.value.build< ASTEnvironmentDefnList > ();
        break;

      case 39: // global_decl
        yylhs.value.build< ASTGlobalDecl > ();
        break;

      case 37: // global_decl_set
      case 38: // global_decl_list
        yylhs.value.build< ASTGlobalDeclList > ();
        break;

      case 51: // identifiable
        yylhs.value.build< ASTIdentifiable > ();
        break;

      case 52: // identifier
        yylhs.value.build< ASTIdentifier > ();
        break;

      case 42: // inference_argument
        yylhs.value.build< ASTInferenceArgument > ();
        break;

      case 40: // argument_set
      case 41: // argument_list
        yylhs.value.build< ASTInferenceArgumentList > ();
        break;

      case 36: // inference_defn
        yylhs.value.build< ASTInferenceDefn > ();
        break;

      case 35: // inference_defn_list
        yylhs.value.build< ASTInferenceDefnList > ();
        break;

      case 48: // premise_type_equality_defn
        yylhs.value.build< ASTInferenceEqualityDefn > ();
        break;

      case 32: // inference_group
        yylhs.value.build< ASTInferenceGroup > ();
        break;

      case 31: // inference_group_list
        yylhs.value.build< ASTInferenceGroupList > ();
        break;

      case 46: // premise_type_inference_defn
        yylhs.value.build< ASTInferencePremiseDefn > ();
        break;

      case 30: // input
        yylhs.value.build< ASTModule > ();
        break;

      case 45: // premise_defn
        yylhs.value.build< ASTPremiseDefn > ();
        break;

      case 43: // premise_set
      case 44: // premise_defn_list
        yylhs.value.build< ASTPremiseDefnList > ();
        break;

      case 50: // proposition_defn
        yylhs.value.build< ASTPropositionDefn > ();
        break;

      case 49: // range_clause
        yylhs.value.build< ASTRangeClause > ();
        break;

      case 47: // while_clause
        yylhs.value.build< ASTWhileClause > ();
        break;

      case 58: // equality_operator
        yylhs.value.build< EqualityOperator > ();
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
        yylhs.value.build< std::string > ();
        break;

      case 12: // INTEGER_LITERAL
        yylhs.value.build< uint64_t > ();
        break;

      default:
        break;
    }


      // Default location.
      {
        slice<stack_symbol_type, stack_type> slice (yystack_, yylen);
        YYLLOC_DEFAULT (yylhs.location, slice, yylen);
        yyerror_range[1].location = yylhs.location;
      }

      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
      try
        {
          switch (yyn)
            {
  case 2:
#line 139 "/Users/x/workspace/snowlake/src/parser/parser.yy" // lalr1.cc:860
    {
            ASTModule module(std::move(yystack_[0].value.as< ASTInferenceGroupList > ()));
            driver.set_module(std::move(module));
        }
#line 956 "parser.tab.cc" // lalr1.cc:860
    break;

  case 3:
#line 147 "/Users/x/workspace/snowlake/src/parser/parser.yy" // lalr1.cc:860
    {
            yylhs.value.as< ASTInferenceGroupList > () = ASTInferenceGroupList();
        }
#line 964 "parser.tab.cc" // lalr1.cc:860
    break;

  case 4:
#line 152 "/Users/x/workspace/snowlake/src/parser/parser.yy" // lalr1.cc:860
    {
            yystack_[1].value.as< ASTInferenceGroupList > ().push_back(yystack_[0].value.as< ASTInferenceGroup > ());
            yylhs.value.as< ASTInferenceGroupList > () = std::move(yystack_[1].value.as< ASTInferenceGroupList > ());
        }
#line 973 "parser.tab.cc" // lalr1.cc:860
    break;

  case 5:
#line 164 "/Users/x/workspace/snowlake/src/parser/parser.yy" // lalr1.cc:860
    {
            yylhs.value.as< ASTInferenceGroup > () = ASTInferenceGroup(std::move(yystack_[4].value.as< std::string > ()), std::move(yystack_[2].value.as< ASTEnvironmentDefnList > ()), std::move(yystack_[1].value.as< ASTInferenceDefnList > ()));
        }
#line 981 "parser.tab.cc" // lalr1.cc:860
    break;

  case 6:
#line 171 "/Users/x/workspace/snowlake/src/parser/parser.yy" // lalr1.cc:860
    {
            yylhs.value.as< ASTEnvironmentDefnList > () = ASTEnvironmentDefnList();
        }
#line 989 "parser.tab.cc" // lalr1.cc:860
    break;

  case 7:
#line 176 "/Users/x/workspace/snowlake/src/parser/parser.yy" // lalr1.cc:860
    {
            yystack_[1].value.as< ASTEnvironmentDefnList > ().push_back(yystack_[0].value.as< ASTEnvironmentDefn > ());
            yylhs.value.as< ASTEnvironmentDefnList > () = std::move(yystack_[1].value.as< ASTEnvironmentDefnList > ());
        }
#line 998 "parser.tab.cc" // lalr1.cc:860
    break;

  case 8:
#line 185 "/Users/x/workspace/snowlake/src/parser/parser.yy" // lalr1.cc:860
    {
            yylhs.value.as< ASTEnvironmentDefn > () = ASTEnvironmentDefn(std::move(yystack_[3].value.as< std::string > ()), std::move(yystack_[1].value.as< std::string > ()));
        }
#line 1006 "parser.tab.cc" // lalr1.cc:860
    break;

  case 9:
#line 192 "/Users/x/workspace/snowlake/src/parser/parser.yy" // lalr1.cc:860
    {
            yylhs.value.as< ASTInferenceDefnList > () = ASTInferenceDefnList();
        }
#line 1014 "parser.tab.cc" // lalr1.cc:860
    break;

  case 10:
#line 197 "/Users/x/workspace/snowlake/src/parser/parser.yy" // lalr1.cc:860
    {
            yystack_[1].value.as< ASTInferenceDefnList > ().push_back(yystack_[0].value.as< ASTInferenceDefn > ());
            yylhs.value.as< ASTInferenceDefnList > () = std::move(yystack_[1].value.as< ASTInferenceDefnList > ());
        }
#line 1023 "parser.tab.cc" // lalr1.cc:860
    break;

  case 11:
#line 211 "/Users/x/workspace/snowlake/src/parser/parser.yy" // lalr1.cc:860
    {
            yylhs.value.as< ASTInferenceDefn > () = ASTInferenceDefn(std::move(yystack_[6].value.as< std::string > ()),
                std::move(yystack_[4].value.as< ASTGlobalDeclList > ()),std::move(yystack_[3].value.as< ASTInferenceArgumentList > ()),std::move(yystack_[2].value.as< ASTPremiseDefnList > ()), std::move(yystack_[1].value.as< ASTPropositionDefn > ()));
        }
#line 1032 "parser.tab.cc" // lalr1.cc:860
    break;

  case 12:
#line 219 "/Users/x/workspace/snowlake/src/parser/parser.yy" // lalr1.cc:860
    {
            yylhs.value.as< ASTGlobalDeclList > () = ASTGlobalDeclList();
        }
#line 1040 "parser.tab.cc" // lalr1.cc:860
    break;

  case 13:
#line 224 "/Users/x/workspace/snowlake/src/parser/parser.yy" // lalr1.cc:860
    {
            yylhs.value.as< ASTGlobalDeclList > () = std::move(yystack_[1].value.as< ASTGlobalDeclList > ());
        }
#line 1048 "parser.tab.cc" // lalr1.cc:860
    break;

  case 14:
#line 232 "/Users/x/workspace/snowlake/src/parser/parser.yy" // lalr1.cc:860
    {
            ASTGlobalDeclList decls;
            decls.push_back(yystack_[0].value.as< ASTGlobalDecl > ());
            yylhs.value.as< ASTGlobalDeclList > () = std::move(decls);
        }
#line 1058 "parser.tab.cc" // lalr1.cc:860
    break;

  case 15:
#line 239 "/Users/x/workspace/snowlake/src/parser/parser.yy" // lalr1.cc:860
    {
            yystack_[2].value.as< ASTGlobalDeclList > ().push_back(yystack_[0].value.as< ASTGlobalDecl > ());
            yylhs.value.as< ASTGlobalDeclList > () = std::move(yystack_[2].value.as< ASTGlobalDeclList > ());
        }
#line 1067 "parser.tab.cc" // lalr1.cc:860
    break;

  case 16:
#line 248 "/Users/x/workspace/snowlake/src/parser/parser.yy" // lalr1.cc:860
    {
            yylhs.value.as< ASTGlobalDecl > () = ASTGlobalDecl(std::move(yystack_[0].value.as< std::string > ()));
        }
#line 1075 "parser.tab.cc" // lalr1.cc:860
    break;

  case 17:
#line 256 "/Users/x/workspace/snowlake/src/parser/parser.yy" // lalr1.cc:860
    {
            yylhs.value.as< ASTInferenceArgumentList > () = ASTInferenceArgumentList();
        }
#line 1083 "parser.tab.cc" // lalr1.cc:860
    break;

  case 18:
#line 261 "/Users/x/workspace/snowlake/src/parser/parser.yy" // lalr1.cc:860
    {
            yylhs.value.as< ASTInferenceArgumentList > () = std::move(yystack_[1].value.as< ASTInferenceArgumentList > ());
        }
#line 1091 "parser.tab.cc" // lalr1.cc:860
    break;

  case 19:
#line 269 "/Users/x/workspace/snowlake/src/parser/parser.yy" // lalr1.cc:860
    {
            ASTInferenceArgumentList arguments;
            arguments.push_back(yystack_[0].value.as< ASTInferenceArgument > ());
            yylhs.value.as< ASTInferenceArgumentList > () = std::move(arguments);
        }
#line 1101 "parser.tab.cc" // lalr1.cc:860
    break;

  case 20:
#line 276 "/Users/x/workspace/snowlake/src/parser/parser.yy" // lalr1.cc:860
    {
            yystack_[2].value.as< ASTInferenceArgumentList > ().push_back(yystack_[0].value.as< ASTInferenceArgument > ());
            yylhs.value.as< ASTInferenceArgumentList > () = std::move(yystack_[2].value.as< ASTInferenceArgumentList > ());
        }
#line 1110 "parser.tab.cc" // lalr1.cc:860
    break;

  case 21:
#line 285 "/Users/x/workspace/snowlake/src/parser/parser.yy" // lalr1.cc:860
    {
            yylhs.value.as< ASTInferenceArgument > () = ASTInferenceArgument(std::move(yystack_[2].value.as< std::string > ()), std::move(yystack_[0].value.as< std::string > ()));
        }
#line 1118 "parser.tab.cc" // lalr1.cc:860
    break;

  case 22:
#line 293 "/Users/x/workspace/snowlake/src/parser/parser.yy" // lalr1.cc:860
    {
            yylhs.value.as< ASTPremiseDefnList > () = yystack_[1].value.as< ASTPremiseDefnList > ();
        }
#line 1126 "parser.tab.cc" // lalr1.cc:860
    break;

  case 23:
#line 300 "/Users/x/workspace/snowlake/src/parser/parser.yy" // lalr1.cc:860
    {
            yylhs.value.as< ASTPremiseDefnList > () = ASTPremiseDefnList();
        }
#line 1134 "parser.tab.cc" // lalr1.cc:860
    break;

  case 24:
#line 305 "/Users/x/workspace/snowlake/src/parser/parser.yy" // lalr1.cc:860
    {
            yystack_[1].value.as< ASTPremiseDefnList > ().push_back(yystack_[0].value.as< ASTPremiseDefn > ());
            yylhs.value.as< ASTPremiseDefnList > () = std::move(yystack_[1].value.as< ASTPremiseDefnList > ());
        }
#line 1143 "parser.tab.cc" // lalr1.cc:860
    break;

  case 25:
#line 314 "/Users/x/workspace/snowlake/src/parser/parser.yy" // lalr1.cc:860
    {
            yylhs.value.as< ASTPremiseDefn > () = ASTPremiseDefn(std::move(yystack_[0].value.as< ASTInferencePremiseDefn > ()));
        }
#line 1151 "parser.tab.cc" // lalr1.cc:860
    break;

  case 26:
#line 319 "/Users/x/workspace/snowlake/src/parser/parser.yy" // lalr1.cc:860
    {
            yylhs.value.as< ASTPremiseDefn > () = ASTPremiseDefn(std::move(yystack_[0].value.as< ASTInferenceEqualityDefn > ()));
        }
#line 1159 "parser.tab.cc" // lalr1.cc:860
    break;

  case 27:
#line 327 "/Users/x/workspace/snowlake/src/parser/parser.yy" // lalr1.cc:860
    {
            yylhs.value.as< ASTInferencePremiseDefn > () = ASTInferencePremiseDefn(std::move(yystack_[3].value.as< ASTIdentifiable > ()), std::move(yystack_[1].value.as< ASTDeductionTarget > ()));
        }
#line 1167 "parser.tab.cc" // lalr1.cc:860
    break;

  case 28:
#line 332 "/Users/x/workspace/snowlake/src/parser/parser.yy" // lalr1.cc:860
    {
            yylhs.value.as< ASTInferencePremiseDefn > () = ASTInferencePremiseDefn(std::move(yystack_[4].value.as< ASTIdentifiable > ()), std::move(yystack_[2].value.as< ASTDeductionTarget > ()), std::move(yystack_[1].value.as< ASTWhileClause > ()));
        }
#line 1175 "parser.tab.cc" // lalr1.cc:860
    break;

  case 29:
#line 340 "/Users/x/workspace/snowlake/src/parser/parser.yy" // lalr1.cc:860
    {
            yylhs.value.as< ASTWhileClause > () = ASTWhileClause(std::move(yystack_[1].value.as< ASTPremiseDefnList > ()));
        }
#line 1183 "parser.tab.cc" // lalr1.cc:860
    break;

  case 30:
#line 348 "/Users/x/workspace/snowlake/src/parser/parser.yy" // lalr1.cc:860
    {
            yylhs.value.as< ASTInferenceEqualityDefn > () = ASTInferenceEqualityDefn(std::move(yystack_[3].value.as< ASTDeductionTarget > ()), std::move(yystack_[1].value.as< ASTDeductionTarget > ()), yystack_[2].value.as< EqualityOperator > ());
        }
#line 1191 "parser.tab.cc" // lalr1.cc:860
    break;

  case 31:
#line 353 "/Users/x/workspace/snowlake/src/parser/parser.yy" // lalr1.cc:860
    {
            yylhs.value.as< ASTInferenceEqualityDefn > () = ASTInferenceEqualityDefn(std::move(yystack_[4].value.as< ASTDeductionTarget > ()), std::move(yystack_[2].value.as< ASTDeductionTarget > ()), yystack_[3].value.as< EqualityOperator > (), std::move(yystack_[1].value.as< ASTRangeClause > ()));
        }
#line 1199 "parser.tab.cc" // lalr1.cc:860
    break;

  case 32:
#line 361 "/Users/x/workspace/snowlake/src/parser/parser.yy" // lalr1.cc:860
    {
            yylhs.value.as< ASTRangeClause > () = ASTRangeClause(yystack_[4].value.as< uint64_t > (), yystack_[2].value.as< uint64_t > (), std::move(yystack_[0].value.as< ASTDeductionTarget > ()));
        }
#line 1207 "parser.tab.cc" // lalr1.cc:860
    break;

  case 33:
#line 369 "/Users/x/workspace/snowlake/src/parser/parser.yy" // lalr1.cc:860
    {
            yylhs.value.as< ASTPropositionDefn > () = ASTPropositionDefn(std::move(yystack_[1].value.as< ASTDeductionTarget > ()));
        }
#line 1215 "parser.tab.cc" // lalr1.cc:860
    break;

  case 34:
#line 377 "/Users/x/workspace/snowlake/src/parser/parser.yy" // lalr1.cc:860
    {
            ASTIdentifiable res;
            res.add(yystack_[0].value.as< ASTIdentifier > ());
            yylhs.value.as< ASTIdentifiable > () = std::move(res);
        }
#line 1225 "parser.tab.cc" // lalr1.cc:860
    break;

  case 35:
#line 384 "/Users/x/workspace/snowlake/src/parser/parser.yy" // lalr1.cc:860
    {
            yystack_[2].value.as< ASTIdentifiable > ().add(yystack_[0].value.as< ASTIdentifier > ());
            yylhs.value.as< ASTIdentifiable > () = yystack_[2].value.as< ASTIdentifiable > ();
        }
#line 1234 "parser.tab.cc" // lalr1.cc:860
    break;

  case 36:
#line 393 "/Users/x/workspace/snowlake/src/parser/parser.yy" // lalr1.cc:860
    {
            yylhs.value.as< ASTIdentifier > () = ASTIdentifier(std::move(yystack_[0].value.as< std::string > ()));
        }
#line 1242 "parser.tab.cc" // lalr1.cc:860
    break;

  case 37:
#line 401 "/Users/x/workspace/snowlake/src/parser/parser.yy" // lalr1.cc:860
    {
            ASTDeductionTargetList list;
            list.push_back(yystack_[0].value.as< ASTDeductionTarget > ());
            yylhs.value.as< ASTDeductionTargetList > () = std::move(list);
        }
#line 1252 "parser.tab.cc" // lalr1.cc:860
    break;

  case 38:
#line 408 "/Users/x/workspace/snowlake/src/parser/parser.yy" // lalr1.cc:860
    {
            yystack_[2].value.as< ASTDeductionTargetList > ().push_back(yystack_[0].value.as< ASTDeductionTarget > ());
            yylhs.value.as< ASTDeductionTargetList > () = std::move(yystack_[2].value.as< ASTDeductionTargetList > ());
        }
#line 1261 "parser.tab.cc" // lalr1.cc:860
    break;

  case 39:
#line 417 "/Users/x/workspace/snowlake/src/parser/parser.yy" // lalr1.cc:860
    {
            yylhs.value.as< ASTDeductionTarget > () = ASTDeductionTarget(std::move(yystack_[0].value.as< ASTDeductionTargetSingular > ()));
        }
#line 1269 "parser.tab.cc" // lalr1.cc:860
    break;

  case 40:
#line 422 "/Users/x/workspace/snowlake/src/parser/parser.yy" // lalr1.cc:860
    {
            yylhs.value.as< ASTDeductionTarget > () = ASTDeductionTarget(std::move(yystack_[0].value.as< ASTDeductionTargetArray > ()));
        }
#line 1277 "parser.tab.cc" // lalr1.cc:860
    break;

  case 41:
#line 427 "/Users/x/workspace/snowlake/src/parser/parser.yy" // lalr1.cc:860
    {
            yylhs.value.as< ASTDeductionTarget > () = ASTDeductionTarget(std::move(yystack_[0].value.as< ASTDeductionTargetComputed > ()));
        }
#line 1285 "parser.tab.cc" // lalr1.cc:860
    break;

  case 42:
#line 435 "/Users/x/workspace/snowlake/src/parser/parser.yy" // lalr1.cc:860
    {
            yylhs.value.as< ASTDeductionTargetSingular > () = ASTDeductionTargetSingular(std::move(yystack_[0].value.as< std::string > ()));
        }
#line 1293 "parser.tab.cc" // lalr1.cc:860
    break;

  case 43:
#line 443 "/Users/x/workspace/snowlake/src/parser/parser.yy" // lalr1.cc:860
    {
            yylhs.value.as< ASTDeductionTargetArray > () = ASTDeductionTargetArray(std::move(yystack_[2].value.as< std::string > ()));
        }
#line 1301 "parser.tab.cc" // lalr1.cc:860
    break;

  case 44:
#line 448 "/Users/x/workspace/snowlake/src/parser/parser.yy" // lalr1.cc:860
    {
            yylhs.value.as< ASTDeductionTargetArray > () = ASTDeductionTargetArray(std::move(yystack_[3].value.as< std::string > ()), std::move(yystack_[1].value.as< uint64_t > ()));
        }
#line 1309 "parser.tab.cc" // lalr1.cc:860
    break;

  case 45:
#line 456 "/Users/x/workspace/snowlake/src/parser/parser.yy" // lalr1.cc:860
    {
            ASTDeductionTargetList arguments;
            yylhs.value.as< ASTDeductionTargetComputed > () = ASTDeductionTargetComputed(std::move(yystack_[2].value.as< std::string > ()), std::move(arguments));
        }
#line 1318 "parser.tab.cc" // lalr1.cc:860
    break;

  case 46:
#line 462 "/Users/x/workspace/snowlake/src/parser/parser.yy" // lalr1.cc:860
    {
            yylhs.value.as< ASTDeductionTargetComputed > () = ASTDeductionTargetComputed(std::move(yystack_[3].value.as< std::string > ()), std::move(yystack_[1].value.as< ASTDeductionTargetList > ()));
        }
#line 1326 "parser.tab.cc" // lalr1.cc:860
    break;

  case 47:
#line 470 "/Users/x/workspace/snowlake/src/parser/parser.yy" // lalr1.cc:860
    {
            yylhs.value.as< EqualityOperator > () = EqualityOperator::OPERATOR_EQ;
        }
#line 1334 "parser.tab.cc" // lalr1.cc:860
    break;

  case 48:
#line 475 "/Users/x/workspace/snowlake/src/parser/parser.yy" // lalr1.cc:860
    {
            yylhs.value.as< EqualityOperator > () = EqualityOperator::OPERATOR_NEQ;
        }
#line 1342 "parser.tab.cc" // lalr1.cc:860
    break;

  case 49:
#line 480 "/Users/x/workspace/snowlake/src/parser/parser.yy" // lalr1.cc:860
    {
            yylhs.value.as< EqualityOperator > () = EqualityOperator::OPERATOR_LT;
        }
#line 1350 "parser.tab.cc" // lalr1.cc:860
    break;

  case 50:
#line 485 "/Users/x/workspace/snowlake/src/parser/parser.yy" // lalr1.cc:860
    {
            yylhs.value.as< EqualityOperator > () = EqualityOperator::OPERATOR_LTE;
        }
#line 1358 "parser.tab.cc" // lalr1.cc:860
    break;


#line 1362 "parser.tab.cc" // lalr1.cc:860
            default:
              break;
            }
        }
      catch (const syntax_error& yyexc)
        {
          error (yyexc);
          YYERROR;
        }
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;
      YY_STACK_PRINT ();

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, yylhs);
    }
    goto yynewstate;

  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        error (yyla.location, yysyntax_error_ (yystack_[0].state, yyla));
      }


    yyerror_range[1].location = yyla.location;
    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.type_get () == yyeof_)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:

    /* Pacify compilers like GCC when the user code never invokes
       YYERROR and the label yyerrorlab therefore never appears in user
       code.  */
    if (false)
      goto yyerrorlab;
    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    goto yyerrlab1;

  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    {
      stack_symbol_type error_token;
      for (;;)
        {
          yyn = yypact_[yystack_[0].state];
          if (!yy_pact_value_is_default_ (yyn))
            {
              yyn += yyterror_;
              if (0 <= yyn && yyn <= yylast_ && yycheck_[yyn] == yyterror_)
                {
                  yyn = yytable_[yyn];
                  if (0 < yyn)
                    break;
                }
            }

          // Pop the current state because it cannot handle the error token.
          if (yystack_.size () == 1)
            YYABORT;

          yyerror_range[1].location = yystack_[0].location;
          yy_destroy_ ("Error: popping", yystack_[0]);
          yypop_ ();
          YY_STACK_PRINT ();
        }

      yyerror_range[2].location = yyla.location;
      YYLLOC_DEFAULT (error_token.location, yyerror_range, 2);

      // Shift the error token.
      error_token.state = yyn;
      yypush_ ("Shifting", error_token);
    }
    goto yynewstate;

    // Accept.
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;

    // Abort.
  yyabortlab:
    yyresult = 1;
    goto yyreturn;

  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack\n";
        // Do not try to display the values of the reclaimed symbols,
        // as their printer might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
  }

  void
  Parser::error (const syntax_error& yyexc)
  {
    error (yyexc.location, yyexc.what());
  }

  // Generate an error message.
  std::string
  Parser::yysyntax_error_ (state_type yystate, const symbol_type& yyla) const
  {
    // Number of reported tokens (one for the "unexpected", one per
    // "expected").
    size_t yycount = 0;
    // Its maximum.
    enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
    // Arguments of yyformat.
    char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];

    /* There are many possibilities here to consider:
       - If this state is a consistent state with a default action, then
         the only way this function was invoked is if the default action
         is an error action.  In that case, don't check for expected
         tokens because there are none.
       - The only way there can be no lookahead present (in yyla) is
         if this state is a consistent state with a default action.
         Thus, detecting the absence of a lookahead is sufficient to
         determine that there is no unexpected or expected token to
         report.  In that case, just report a simple "syntax error".
       - Don't assume there isn't a lookahead just because this state is
         a consistent state with a default action.  There might have
         been a previous inconsistent state, consistent state with a
         non-default action, or user semantic action that manipulated
         yyla.  (However, yyla is currently not documented for users.)
       - Of course, the expected token list depends on states to have
         correct lookahead information, and it depends on the parser not
         to perform extra reductions after fetching a lookahead from the
         scanner and before detecting a syntax error.  Thus, state
         merging (from LALR or IELR) and default reductions corrupt the
         expected token list.  However, the list is correct for
         canonical LR with one exception: it will still contain any
         token that will not be accepted due to an error action in a
         later state.
    */
    if (!yyla.empty ())
      {
        int yytoken = yyla.type_get ();
        yyarg[yycount++] = yytname_[yytoken];
        int yyn = yypact_[yystate];
        if (!yy_pact_value_is_default_ (yyn))
          {
            /* Start YYX at -YYN if negative to avoid negative indexes in
               YYCHECK.  In other words, skip the first -YYN actions for
               this state because they are default actions.  */
            int yyxbegin = yyn < 0 ? -yyn : 0;
            // Stay within bounds of both yycheck and yytname.
            int yychecklim = yylast_ - yyn + 1;
            int yyxend = yychecklim < yyntokens_ ? yychecklim : yyntokens_;
            for (int yyx = yyxbegin; yyx < yyxend; ++yyx)
              if (yycheck_[yyx + yyn] == yyx && yyx != yyterror_
                  && !yy_table_value_is_error_ (yytable_[yyx + yyn]))
                {
                  if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                    {
                      yycount = 1;
                      break;
                    }
                  else
                    yyarg[yycount++] = yytname_[yyx];
                }
          }
      }

    char const* yyformat = YY_NULLPTR;
    switch (yycount)
      {
#define YYCASE_(N, S)                         \
        case N:                               \
          yyformat = S;                       \
        break
        YYCASE_ (0, YY_("syntax error"));
        YYCASE_ (1, YY_("syntax error, unexpected %s"));
        YYCASE_ (2, YY_("syntax error, unexpected %s, expecting %s"));
        YYCASE_ (3, YY_("syntax error, unexpected %s, expecting %s or %s"));
        YYCASE_ (4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
        YYCASE_ (5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
      }

    std::string yyres;
    // Argument number.
    size_t yyi = 0;
    for (char const* yyp = yyformat; *yyp; ++yyp)
      if (yyp[0] == '%' && yyp[1] == 's' && yyi < yycount)
        {
          yyres += yytnamerr_ (yyarg[yyi++]);
          ++yyp;
        }
      else
        yyres += *yyp;
    return yyres;
  }


  const signed char Parser::yypact_ninf_ = -44;

  const signed char Parser::yytable_ninf_ = -37;

  const signed char
  Parser::yypact_[] =
  {
     -44,    38,    36,   -44,    27,   -44,    21,   -44,    30,    28,
     -44,    -3,    32,    33,   -44,   -44,    31,    29,   -44,    40,
      34,    45,    35,    39,    42,    41,    43,    44,    46,   -44,
     -10,   -44,     6,    47,    48,    37,    41,   -44,    50,   -44,
       2,   -44,   -44,    49,   -44,   -44,    54,    55,   -44,     9,
      11,    52,   -44,   -44,   -44,   -44,   -44,    -8,   -44,   -44,
     -44,   -44,    16,   -44,    10,     8,   -11,   -44,    57,    49,
     -44,   -44,   -44,   -44,    49,    53,   -44,   -44,   -12,   -44,
     -44,   -44,    -4,    -2,   -44,    49,   -44,    51,   -44,    56,
      62,   -44,    58,   -44,   -44,   -44,    59,   -44,     3,    64,
     -44,    60,    49,   -44
  };

  const unsigned char
  Parser::yydefact_[] =
  {
       3,     0,     2,     1,     0,     4,     0,     6,     9,     0,
       7,     0,     0,     0,     5,    10,     0,     0,     8,    12,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    16,
       0,    14,     0,     0,     0,     0,     0,    13,     0,    17,
       0,    19,    23,     0,    11,    15,     0,     0,    18,     0,
      42,     0,    39,    40,    41,    21,    20,    42,    22,    24,
      25,    26,     0,    34,     0,     0,     0,    33,     0,     0,
      47,    48,    49,    50,     0,     0,    43,    45,     0,    37,
      36,    35,     0,     0,    44,     0,    46,     0,    27,     0,
       0,    30,     0,    38,    23,    28,     0,    31,     0,     0,
      29,     0,     0,    32
  };

  const signed char
  Parser::yypgoto_[] =
  {
     -44,   -44,   -44,   -44,   -44,   -44,   -44,   -44,   -44,   -44,
      20,   -44,   -44,    61,   -44,   -31,   -44,   -44,   -44,   -44,
     -44,   -44,   -44,    12,   -44,   -43,   -44,   -44,   -44,   -44
  };

  const signed char
  Parser::yydefgoto_[] =
  {
      -1,     1,     2,     5,     8,    10,    11,    15,    21,    30,
      31,    24,    40,    41,    28,    49,    59,    60,    89,    61,
      92,    35,    62,    63,    78,    64,    52,    53,    54,    74
  };

  const signed char
  Parser::yytable_[] =
  {
      51,    13,    50,    85,    87,    36,   -36,    90,   -36,    37,
      65,    86,    77,    88,    66,    91,    57,    47,    14,    38,
      75,    48,    57,    79,   100,    39,    82,    76,    58,    65,
      68,    83,    69,    66,    70,    71,    72,    73,     3,     4,
       6,     7,    93,     9,    12,    16,    17,    20,    18,    19,
      22,    23,    27,    25,    29,    26,    45,    34,    44,   103,
      33,    32,    50,    98,    43,    42,    46,    55,    38,    67,
      80,    94,    84,    95,    96,    97,   101,     0,     0,     0,
      81,     0,     0,     0,     0,     0,     0,    99,   102,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    56
  };

  const signed char
  Parser::yycheck_[] =
  {
      43,     4,    13,    15,     8,    15,    14,     9,    16,    19,
      18,    23,    23,    17,    22,    17,    13,    15,    21,    13,
      12,    19,    13,    66,    21,    19,    69,    19,    19,    18,
      14,    74,    16,    22,    24,    25,    26,    27,     0,     3,
      13,    20,    85,    13,    16,    13,    13,     7,    17,    20,
      16,     6,    10,    18,    13,    16,    36,    11,    21,   102,
      16,    18,    13,    94,    16,    18,    16,    13,    13,    17,
      13,    20,    19,    17,    12,    17,    12,    -1,    -1,    -1,
      68,    -1,    -1,    -1,    -1,    -1,    -1,    28,    28,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    47
  };

  const unsigned char
  Parser::yystos_[] =
  {
       0,    30,    31,     0,     3,    32,    13,    20,    33,    13,
      34,    35,    16,     4,    21,    36,    13,    13,    17,    20,
       7,    37,    16,     6,    40,    18,    16,    10,    43,    13,
      38,    39,    18,    16,    11,    50,    15,    19,    13,    19,
      41,    42,    18,    16,    21,    39,    16,    15,    19,    44,
      13,    54,    55,    56,    57,    13,    42,    13,    19,    45,
      46,    48,    51,    52,    54,    18,    22,    17,    14,    16,
      24,    25,    26,    27,    58,    12,    19,    23,    53,    54,
      13,    52,    54,    54,    19,    15,    23,     8,    17,    47,
       9,    17,    49,    54,    20,    17,    12,    17,    44,    28,
      21,    12,    28,    54
  };

  const unsigned char
  Parser::yyr1_[] =
  {
       0,    29,    30,    31,    31,    32,    33,    33,    34,    35,
      35,    36,    37,    37,    38,    38,    39,    40,    40,    41,
      41,    42,    43,    44,    44,    45,    45,    46,    46,    47,
      48,    48,    49,    50,    51,    51,    52,    53,    53,    54,
      54,    54,    55,    56,    56,    57,    57,    58,    58,    58,
      58
  };

  const unsigned char
  Parser::yyr2_[] =
  {
       0,     2,     1,     0,     2,     6,     0,     2,     4,     0,
       2,     8,     0,     5,     1,     3,     1,     4,     5,     1,
       3,     3,     5,     0,     2,     1,     1,     4,     5,     4,
       4,     5,     6,     4,     1,     3,     1,     1,     3,     1,
       1,     1,     1,     3,     4,     3,     4,     1,     1,     1,
       1
  };



  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a yyntokens_, nonterminals.
  const char*
  const Parser::yytname_[] =
  {
  "\"end of file\"", "error", "$undefined", "KEYWORD_GROUP",
  "KEYWORD_INFERENCE", "KEYWORD_ENVIRONMENT", "KEYWORD_ARGUMENTS",
  "KEYWORD_GLOBALS", "KEYWORD_WHILE", "KEYWORD_INRANGE",
  "KEYWORD_PREMISES", "KEYWORD_PROPOSITION", "INTEGER_LITERAL",
  "IDENTIFIER", "DOT", "COMMA", "COLON", "SEMICOLON", "LBRACKET",
  "RBRACKET", "LBRACE", "RBRACE", "LPAREN", "RPAREN", "OPERATOR_EQ",
  "OPERATOR_NEQ", "OPERATOR_LT", "OPERATOR_LTE", "ELLIPSIS", "$accept",
  "input", "inference_group_list", "inference_group",
  "environment_defn_list", "environment_defn", "inference_defn_list",
  "inference_defn", "global_decl_set", "global_decl_list", "global_decl",
  "argument_set", "argument_list", "inference_argument", "premise_set",
  "premise_defn_list", "premise_defn", "premise_type_inference_defn",
  "while_clause", "premise_type_equality_defn", "range_clause",
  "proposition_defn", "identifiable", "identifier",
  "deduction_target_list", "deduction_target", "deduction_target_singular",
  "deduction_target_array", "deduction_target_computed",
  "equality_operator", YY_NULLPTR
  };

#if YYDEBUG
  const unsigned short int
  Parser::yyrline_[] =
  {
       0,   138,   138,   147,   151,   160,   171,   175,   184,   192,
     196,   205,   219,   223,   231,   238,   247,   255,   260,   268,
     275,   284,   292,   300,   304,   313,   318,   326,   331,   339,
     347,   352,   360,   368,   376,   383,   392,   400,   407,   416,
     421,   426,   434,   442,   447,   455,   461,   469,   474,   479,
     484
  };

  // Print the state stack on the debug stream.
  void
  Parser::yystack_print_ ()
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << i->state;
    *yycdebug_ << '\n';
  }

  // Report on the debug stream that the rule \a yyrule is going to be reduced.
  void
  Parser::yy_reduce_print_ (int yyrule)
  {
    unsigned yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):\n";
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG



} // yy
#line 1807 "parser.tab.cc" // lalr1.cc:1166
#line 490 "/Users/x/workspace/snowlake/src/parser/parser.yy" // lalr1.cc:1167


void
yy::Parser::error(const location_type& l, const std::string& m)
{
  driver.error(l, m);
}
