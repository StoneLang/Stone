#ifndef STONE_CORE_BUILTIN_H
#define STONE_CORE_BUILTIN_H

namespace stone {
namespace syntax {
class ASTContext;

class Builtin final {
  enum BuiltinID {
    None,
  };
  Builtin(const Builtin &) = delete;
  void operator=(const Builtin &) = delete;

  void InitType(ASTContext &astCtx);
  void InitTypes(ASTContext &astCtx);

  BuiltinID builtinID;

 public:
  Builtin() = default;
  ~Builtin();

  void Init(ASTContext &astCtx);
};
}  // namespace syntax
}  // namespace stone
#endif
