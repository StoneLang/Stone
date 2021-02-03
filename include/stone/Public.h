#ifndef STONE_PUBLIC_H
#define STONE_PUBLIC_H

namespace stone {
class Compiler;
namespace syn {
class SourceUnit;
}
namespace syn {
class Token;
}

class Pipeline {
  Pipeline() = default;
  virtual ~Pipeline() = default;

 public:
  virtual void CompilerConfigured(const Compiler &compiler) = 0;

 public:
  virtual void CompileStarted(const Compiler &compiler) = 0;
  virtual void CompileFinished(const Compiler &compiler) = 0;

 public:
  ///
  virtual void ParseStarted(const Compiler &compiler) = 0;
  ///
  virtual void ParseStopped(const Compiler &compiler) = 0;
  ///
  virtual void ParseFinished(const Compiler &compiler) = 0;

  // virtual void OnToken(const syn::Token &token);
  // virtual void OnSourceFile(const syn::SourceUnit &unit);

 public:
  ///
  virtual void CheckStarted(const Compiler &compiler) = 0;
  ///
  virtual void CheckStopped(const Compiler &compiler) = 0;
  ///
  virtual void CheckFinished(const Compiler &compiler) = 0;

 public:
  ///
  virtual void GenIRStarted(const Compiler &compiler) = 0;
  ///
  virtual void GenIRStopped(const Compiler &compiler) = 0;
  ///
  virtual void GenIRFinished(const Compiler &compiler) = 0;

 public:
  ///
  virtual void GenObjectStarted(const Compiler &compiler);
  ///
  virtual void GenObjectStopped(const Compiler &compiler);
  ///
  virtual void GenObjectFinished(const Compiler &compiler);
};
}  // namespace stone
#endif
