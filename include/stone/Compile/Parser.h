#ifndef STONE_COMPILE_PARSER_H
#define STONE_COMPILE_PARSER_H

#include <memory>

#include "stone/Compile/Analysis.h"
#include "stone/Compile/AnalysisOptions.h"
#include "stone/Compile/ParserDiagnostic.h"
#include "stone/Core/ASTContext.h"
#include "stone/Core/Context.h"
#include "stone/Core/Module.h"
#include "stone/Core/Stats.h"

namespace stone {

class CompilePipeline;
namespace analysis {
class Parser;
class ParserStats final : public Stats {
  const Parser &parser;

 public:
  ParserStats(const Parser &parser) : parser(parser) {}
  void Print() const override;
};

// class ParserDiagnostics final : public Diagnostics { };

class Parser final {
  friend ParserStats;
  Analysis &analysis;
  ParserStats stats;
  CompilePipeline *pipeline;

 public:
  Parser(Analysis &analysis, CompilePipeline *pipeline = nullptr);

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
}  // namespace analysis
}  // namespace stone
#endif
