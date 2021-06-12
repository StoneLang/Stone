#ifndef STONE_ANALYZE_CHECKER_H
#define STONE_ANALYZE_CHECKER_H

#include "stone/Basic/Context.h"
#include "stone/Basic/Stats.h"
#include "stone/Syntax/Module.h"
#include "stone/Syntax/TreeContext.h"

#include <memory>

using namespace stone::syn;

namespace stone {
class Pipeline;

namespace syn {
class SourceFile;
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
  syn::SourceFile &su;
  std::unique_ptr<CheckerStats> stats;

  Pipeline *pipeline;
  Context &ctx;

public:
  // TODO: CompileUnit
  Checker(syn::SourceFile &su, Context &ctx, Pipeline *pipeline = nullptr);

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
