#ifndef STONE_PUBLIC_H
#define STONE_PUBLIC_H

namespace stone {

namespace syntax {
class SourceUnit;
}
namespace analysis {
class Token;
class Compiler;
}  // namespace analysis

class CompilePipeline {
  CompilePipeline() = default;
  virtual ~CompilePipeline() = default;

 public:
  virtual void OnCompilerConfigured(const analysis::Compiler &compiler);

 public:
  virtual void OnCompileStarted(const analysis::Compiler &compiler);
  virtual void OnCompileFinished(const analysis::Compiler &compiler);

 public:
  ///
  virtual void OnParseStarted(const analysis::Compiler &compiler);
  ///
  virtual void OnParseStopped(const analysis::Compiler &compiler);
  ///
  virtual void OnParseFinished(const analysis::Compiler &compiler);

  virtual void OnToken(const analysis::Token &token);
  virtual void OnSourceFile(const syntax::SourceUnit &unit);

 public:
  ///
  virtual void OnCheckStarted(const analysis::Compiler &compiler);
  ///
  virtual void OnCheckStopped(const analysis::Compiler &compiler);
  ///
  virtual void OnCheckFinished(const analysis::Compiler &compiler);

 public:
  ///
  virtual void OnGenIRStarted(const analysis::Compiler &compiler);
  ///
  virtual void OnGenIRStopped(const analysis::Compiler &compiler);
  ///
  virtual void OnGenIRFinished(const analysis::Compiler &compiler);

 public:
  ///
  virtual void OnGenObjectStarted(const analysis::Compiler &compiler);
  ///
  virtual void OnGenObjectStopped(const analysis::Compiler &compiler);
  ///
  virtual void OnGenObjectFinished(const analysis::Compiler &compiler);
};
}  // namespace stone
#endif
