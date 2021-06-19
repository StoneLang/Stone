#ifndef STONE_ANALYZE_CHECKER_H
#define STONE_ANALYZE_CHECKER_H

#include "stone/Basic/Context.h"
#include "stone/Basic/Stats.h"
#include "stone/Syntax/Module.h"
#include "stone/Syntax/TreeContext.h"

#include <memory>

using namespace stone::syn;

namespace stone {
class CheckerPipeline;

namespace syn {
class SourceModuleFile;
}
namespace sema {
class Checker;

class CheckerStats final : public Stats {
  const Checker &checker;

public:
  CheckerStats(const Checker &checker, Context &ctx)
      : Stats("checker statistics:", ctx), checker(checker) {}
  void Print() override;
};

class Checker final {
  friend CheckerStats;
  syn::SourceModuleFile &su;
  std::unique_ptr<CheckerStats> stats;

  CheckerPipeline *pipeline;
  Context &ctx;

public:
  /// TODO: Pass in Syntax so that you can create the Nodes
  Checker(syn::SourceModuleFile &su, Context &ctx,
          CheckerPipeline *pipeline = nullptr);

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
