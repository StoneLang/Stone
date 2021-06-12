#ifndef STONE_ANALYZE_PARSER_H
#define STONE_ANALYZE_PARSER_H

#include "llvm/Support/Timer.h"

#include "stone/Analyze/Lexer.h"
#include "stone/Analyze/SyntaxResult.h"
#include "stone/Basic/Context.h"
#include "stone/Basic/Stats.h"
#include "stone/Syntax/Module.h"
#include "stone/Syntax/Syntax.h"
#include "stone/Syntax/TreeContext.h"

#include <memory>

namespace stone {
class ParserPipeline;
namespace syn {
class Parser;
class ParserStats final : public Stats {
  Parser &parser;

public:
  ParserStats(Parser &parser) : Stats("parser statistics:"), parser(parser) {}
  void Print() override;
};

class Parser final {
  friend ParserStats;
  Context &ctx;
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
  class ParsingScope {};

  class MultiParsingScope {};

public:
  Parser(Context &ctx, ParserPipeline *pipeline = nullptr);
  ~Parser();

public:
  ParserStats &GetStats() { return *stats.get(); }
  Lexer &GetLexer() { return *lexer.get(); }

public:
public:
  // Decl
  int ParseTopDecl();

  //
  void ParseDecl();

  void ParseSpaceDecl();

public:
  // Stmt
  void ParseStmt();

public:
  // Expr
  void ParseExpr();

public:
  /// Stop parsing immediately.
  void Stop() { tok.SetType(tk::Type::eof); }
  /// Is at end of file.
  bool IsDone() { return tok.GetType() == tk::Type::eof; }
};
} // namespace syn
} // namespace stone
#endif
