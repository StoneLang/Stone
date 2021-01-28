#ifndef STONE_COMPILE_CHECKER_H
#define STONE_COMPILE_CHECKER_H

#include <memory>

#include "stone/Compile/CheckerDiagnostic.h"
#include "stone/Core/ASTContext.h"
#include "stone/Core/Context.h"
#include "stone/Core/Module.h"
#include "stone/Core/Stats.h"

using namespace stone::syntax;

namespace stone {
class Pipeline;

namespace syntax {
class SourceUnit;
}
namespace semantics {
class Checker;

class CheckerStats final : public Stats {
  const Checker &checker;

 public:
  CheckerStats(const char *name, const Checker &checker)
      : Stats(name), checker(checker) {}
  void Print() const override;
};

class Checker final {
  friend CheckerStats;
  syntax::SourceUnit &su;
  CheckerStats stats;
  Pipeline *pipeline;

 public:
  // TODO: CompileUnit
  Checker(syntax::SourceUnit &su, Pipeline *pipeline = nullptr);

 public:
  void CheckDecl();

 public:
  void CheckStmt();

 public:
  void CheckExpr();
};
}  // namespace semantics
}  // namespace stone
#endif
