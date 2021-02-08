#ifndef STONE_SYNTAX_BUILTIN_H
#define STONE_SYNTAX_BUILTIN_H

namespace stone {
namespace syn {
class TreeContext;

class Builtin final {
  enum BuiltinID {
    None,
  };
  Builtin(const Builtin &) = delete;
  void operator=(const Builtin &) = delete;

  void InitType(TreeContext &astCtx);
  void InitTypes(TreeContext &astCtx);

  BuiltinID builtinID;

public:
  Builtin() = default;
  ~Builtin();

  void Init(TreeContext &astCtx);
};
} // namespace syn
} // namespace stone
#endif
