#ifndef STONE_ANALYZE_CHECKER_H
#define STONE_ANALYZE_CHECKER_H

#include "stone/Basic/Basic.h"
#include "stone/Basic/Stats.h"
#include "stone/Syntax/Module.h"
#include "stone/Syntax/TreeContext.h"

#include <memory>

using namespace stone::syn;

namespace stone {
class CheckerPipeline;

namespace syn {
class Syntax;
class SourceModuleFile;
} // namespace syn
namespace sema {
class Checker;

class CheckerStats final : public Stats {
  const Checker &checker;

public:
  CheckerStats(const Checker &checker, Basic &basic)
      : Stats("checker statistics:", basic), checker(checker) {}
  void Print() override;
};

class Checker final {
  friend CheckerStats;

  Syntax &syntax;
  syn::SourceModuleFile &sf;
  std::unique_ptr<CheckerStats> stats;
  CheckerPipeline *pipeline;

public:
  /// TODO: Pass in Syntax so that you can create the Nodes
  Checker(syn::SourceModuleFile &sf, Syntax &syntax,
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
