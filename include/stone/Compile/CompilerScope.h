#ifndef STONE_COMPILE_COMPILERSCOPE_H
#define STONE_COMPILE_COMPILERSCOPE_H

namespace stone {
class CompilerScope final {
  CompilerScope(const CompilerScope &) = delete;
  void operator=(const CompilerScope &) = delete;

 public:
  CompilerScope();
  ~CompilerScope();

 public:
  void Enter();
  void Exit();
};
}  // namespace stone
#endif
