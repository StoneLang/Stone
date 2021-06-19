#ifndef STONE_ANALYZE_PARSER_H
#define STONE_ANALYZE_PARSER_H

#include "llvm/Support/Timer.h"

#include "stone/Analyze/Lexer.h"
#include "stone/Analyze/SyntaxResult.h"
#include "stone/Basic/Stats.h"
#include "stone/Syntax/Module.h"
#include "stone/Syntax/TreeContext.h"

#include <memory>

namespace stone {
class Context;
class SrcMgr;
class ParserPipeline;

namespace syn {

class Parser;
class Scope;

class ParserStats final : public Stats {
  Parser &parser;

public:
  ParserStats(Parser &parser, Context &ctx)
      : Stats("parser statistics:", ctx), parser(parser) {}
  void Print() override;
};

class Parser final {
  friend ParserStats;

  Context &ctx;
  SrcMgr &sm;
  SourceModuleFile &sf;
  ParserPipeline *pipeline;
  std::unique_ptr<Lexer> lexer;

  std::unique_ptr<ParserStats> stats;

  /// This is the current token being considered by the parser.
  Token tok;

  /// leading trivias for \c Tok.
  /// Always empty if !SF.shouldBuildSyntaxTree().
  Trivia leadingTrivia;

  /// trailing trivias for \c Tok.
  /// Always empty if !SF.shouldBuildSyntaxTree().
  Trivia trailingTrivia;

  /// The location of the previous token.
  SrcLoc prevTokLoc;

public:
  /// Control flags for SkipUntil functions.
  enum SkipUntilFlags {
    ///< Stop skipping at semicolon
    StopAtSemi = 1 << 0,
    /// Stop skipping at specified token, but don't skip the token itself
    StopBeforeMatch = 1 << 1,
    ///< Stop at code completion
    StopAtCodeCompletion = 1 << 2
  };

public:
  Parser(SourceModuleFile &sf, SrcMgr &sm, Context &ctx,
         ParserPipeline *pipeline = nullptr);

  Parser(SourceModuleFile &sf, SrcMgr &sm, Context &ctx,
         std::unique_ptr<Lexer> lexer, ParserPipeline *pipeline = nullptr);

  ~Parser();

public:
  class ParsingScope final {
    Parser *self;
    ParsingScope(const ParsingScope &) = delete;
    void operator=(const ParsingScope &) = delete;

  public:
    // ParseScope - Construct a new object to manage a scope in the
    // parser Self where the new Scope is created with the flags
    // ScopeFlags, but only when we aren't about to enter a compound statement.
    ParsingScope(Parser *self, unsigned scopeFlags, bool enteredScope = true,
                 bool beforeCompoundStmt = false)
        : self(self) {

      if (enteredScope && !beforeCompoundStmt) {
        self->EnterScope(scopeFlags);
      } else {
        if (beforeCompoundStmt) {
          // TODO: self->incrementMSManglingNumber();
        }
        this->self = nullptr;
      }
    }
    // Exit - Exit the scope associated with this object now, rather
    // than waiting until the object is destroyed.
    void Exit() {
      if (self) {
        self->ExitScope();
        self = nullptr;
      }
    }
    ~ParsingScope() { Exit(); }
  };

public:
  class MultiParsingScope final {
  public:
  };

public:
  ParserStats &GetStats() { return *stats.get(); }
  Lexer &GetLexer() { return *lexer.get(); }
  const Token &GetCurToken() const { return tok; }

public:
  bool ParseTopDecl(DeclGroupPtrTy &result, bool isFirstDecl = false);

  bool ParseTopDecl() {
    syn::DeclGroupPtrTy result;
    return ParseTopDecl(result);
  }
  ///
  void ParseDecl();

  ///
  void ParseSpaceDecl();

  ///
public:
  /// Stmt
  void ParseStmt();

public:
  /// Expr
  void ParseExpr();

public:
  /// Stop parsing immediately.
  void Stop() { tok.SetType(tk::Type::eof); }

  /// Is at end of file.
  bool IsDone() { return tok.GetType() == tk::Type::eof; }

  void EatToken() {}

  SrcLoc EatBracket() { return SrcLoc(); }
  SrcLoc EatBrace() { return SrcLoc(); }
  SrcLoc EatParen() { return SrcLoc(); }

public:
  /// EnterScope - start a new scope.
  void EnterScope(unsigned scopeFlags);

  /// ExitScope - pop a scope off the scope stack.
  void ExitScope();

  syn::Scope *GetCurScope() const;
};

} // namespace syn
} // namespace stone
#endif
