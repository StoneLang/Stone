#ifndef STONE_COMPILE_PARSER_H
#define STONE_COMPILE_PARSER_H

#include <memory>

#include "stone/Compile/Lexer.h"
#include "stone/Compile/ParserDiagnostics.h"
#include "stone/Core/ASTContext.h"
#include "stone/Core/Context.h"
#include "stone/Core/Module.h"
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
  Clock clock;
  Pipeline *pipeline;
  std::unique_ptr<Lexer> lexer;
  std::unique_ptr<ParserStats> stats;

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
}  // namespace syn
}  // namespace stone
#endif
