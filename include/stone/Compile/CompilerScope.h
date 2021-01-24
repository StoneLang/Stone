#ifndef STONE_COMPILE_COMPILERSCOPE_H
#define STONE_COMPILE_COMPILERSCOPE_H

namespace stone {
class CompilerScope final {
  CompilerScope(const CompilerScope &) = delete;
  CompilerScope(CompilerScope &&) = delete;
  CompilerScope &operator=(const CompilerScope &) = delete;
  CompilerScope &operator=(CompilerScope &&) = delete;

 public:
  CompilerScope();
  ~CompilerScope();

 public:
  void Enter();
  void Exit();
};
}  // namespace stone
#endif
