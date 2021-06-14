#ifndef STONE_SYNTAX_BUILTIN_H
#define STONE_SYNTAX_BUILTIN_H

namespace stone {
namespace syn {
class TreeContext;

namespace builtin {
enum ID {};
struct Profile {
  const char *name;
  const char *ty;
  const char *attributes;
  const char *features;
};
} // namespace builtin

class Builtin final {

  Builtin(const Builtin &) = delete;
  void operator=(const Builtin &) = delete;

  void InitType(TreeContext &tc);
  void InitTypes(TreeContext &tc);

public:
  Builtin() = default;
  ~Builtin();

  void Init(TreeContext &tc);
};
} // namespace syn
} // namespace stone
#endif
