#ifndef STONE_ANALYSIS_PARSER_H
#define STONE_ANALYSIS_PARSER_H

#include <memory>

#include "stone/Analysis/Lexer.h"
#include "stone/Syntax/Module.h"
#include "stone/Syntax/TreeContext.h"
#include "stone/Utils/Context.h"
#include "stone/Utils/Stats.h"

namespace stone {
class Pipeline;
namespace analysis {
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
  Clock clock;
  Pipeline *pipeline;
  std::unique_ptr<Lexer> lexer;
  std::unique_ptr<ParserStats> stats;

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
};
} // namespace analysis
} // namespace stone
#endif
