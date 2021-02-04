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
  virtual void OnCompilerConfigured(const Compiler &compiler) = 0;

public:
  virtual void OnCompileStarted(const Compiler &compiler) = 0;
  virtual void OnCompileFinished(const Compiler &compiler) = 0;

public:
  ///
  virtual void OnParseStarted(const Compiler &compiler) = 0;
  ///
  virtual void OnParseStopped(const Compiler &compiler) = 0;
  ///
  virtual void OnParseFinished(const Compiler &compiler) = 0;

  // virtual void OnToken(const syn::Token &token);
  // virtual void OnSourceFile(const syn::SourceUnit &unit);

public:
  ///
  virtual void OnCheckStarted(const Compiler &compiler) = 0;
  ///
  virtual void OnCheckStopped(const Compiler &compiler) = 0;
  ///
  virtual void OnCheckFinished(const Compiler &compiler) = 0;

public:
  ///
  virtual void OnGenIRStarted(const Compiler &compiler) = 0;
  ///
  virtual void OnGenIRStopped(const Compiler &compiler) = 0;
  ///
  virtual void OnGenIRFinished(const Compiler &compiler) = 0;

public:
  ///
  virtual void OnGenObjectStarted(const Compiler &compiler);
  ///
  virtual void OnGenObjectStopped(const Compiler &compiler);
  ///
  virtual void OnGenObjectFinished(const Compiler &compiler);
};
} // namespace stone
#endif
