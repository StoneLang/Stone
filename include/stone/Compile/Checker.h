#ifndef STONE_COMPILE_CHECKER_H
#define STONE_COMPILE_CHECKER_H

#include <memory>

#include "stone/Compile/Analysis.h"
#include "stone/Compile/AnalysisOptions.h"
#include "stone/Compile/CheckerDiagnostic.h"
#include "stone/Core/ASTContext.h"
#include "stone/Core/Context.h"
#include "stone/Core/Module.h"
#include "stone/Core/Stats.h"

namespace stone {
class CompilePipeline;

namespace analysis {
class Checker;

class CheckerStats final : public Stats {
  const Checker &checker;

 public:
  CheckerStats(const Checker &checker) : checker(checker) {}
  void Print() const override;
};

class Checker final {
  friend CheckerStats;
  Analysis &analysis;
  CheckerStats stats;
  CompilePipeline *pipeline;

 public:
  // TODO: Pass CompileUnit
  Checker(Analysis &analysis, CompilePipeline *pipeline = nullptr);

 public:
  void CheckDecl();

 public:
  void CheckStmt();

 public:
  void CheckExpr();
};
}  // namespace analysis
}  // namespace stone
#endif
