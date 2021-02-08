#ifndef STONE_CHECK_CHECKER_H
#define STONE_CHECK_CHECKER_H

#include <memory>

#include "stone/Syntax/Module.h"
#include "stone/Syntax/TreeContext.h"
#include "stone/Utils/Context.h"
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
} // namespace sema
} // namespace stone
#endif
