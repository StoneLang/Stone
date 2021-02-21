#ifndef STONE_COMPILE_PARSER_H
#define STONE_COMPILE_PARSER_H

#include <memory>

#include "llvm/Support/Timer.h"

#include "stone/Compile/Lexer.h"
#include "stone/Syntax/Module.h"
#include "stone/Syntax/Syntax.h"
#include "stone/Syntax/TreeContext.h"
#include "stone/Utils/Context.h"
#include "stone/Utils/Stats.h"

namespace stone {
class Pipeline;
namespace syn {
class Parser;
class ParserStats final : public Stats {
  const Parser &parser;

public:
  ParserStats(const Parser &parser)
      : Stats("parser statistics:"), parser(parser) {}
  void Print() override;
};

class Parser final {
  friend ParserStats;
  Context &ctx;
  llvm::Timer timer;
  Pipeline *pipeline;
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
  Parser(Context &ctx, Pipeline *pipeline = nullptr);
  ~Parser();

public:
  ParserStats &GetStats() { return *stats.get(); }

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
