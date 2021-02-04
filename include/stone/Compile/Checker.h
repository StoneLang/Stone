#ifndef STONE_COMPILE_CHECKER_H
#define STONE_COMPILE_CHECKER_H

#include <memory>

#include "stone/Compile/CheckerDiagnostics.h"
#include "stone/Core/ASTContext.h"
#include "stone/Core/Context.h"
#include "stone/Core/Module.h"
#include "stone/Utils/Stats.h"

using namespace stone::syn;

namespace stone {
class Pipeline;

namespace syn {
class SourceUnit;
}

namespace sema {
class Checker;

class CheckerStats final : public Stats {
  const Checker &checker;

 public:
  CheckerStats(const Checker &checker)
      : Stats("checker statistics:"), checker(checker) {}
  void Print() override;
};

class Checker final {
  friend CheckerStats;
  syn::SourceUnit &su;
  std::unique_ptr<CheckerStats> stats;
  Pipeline *pipeline;
  Context &ctx;

 public:
  // TODO: CompileUnit
  Checker(syn::SourceUnit &su, Context &ctx, Pipeline *pipeline = nullptr);

 public:
  void CheckDecl();

 public:
  void CheckStmt();

 public:
  void CheckExpr();
};
}  // namespace sema
}  // namespace stone
#endif
