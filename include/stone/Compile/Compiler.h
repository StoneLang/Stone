#ifndef STONE_COMPILE_COMPILER_H
#define STONE_COMPILE_COMPILER_H

#include "stone/Compile/CompilerOptions.h"
#include "stone/Compile/CompilerUnit.h"
#include "stone/Core/ASTContext.h"
#include "stone/Core/Module.h"
#include "stone/Core/SearchPathOptions.h"
#include "stone/Session/Session.h"

using namespace stone::syntax;

namespace stone {
class Pipeline;

// struct CompileInputProfile final {};
// struct CompileOutputProfile final {};

class Compiler final : public Session {
  SrcMgr sm;
  FileMgr fm;
  Pipeline *pipeline = nullptr;
  mutable Module *mainModule = nullptr;
  std::unique_ptr<ASTContext> ac;

 private:
  class Implementation;
  static int Run(Compiler &compiler);
  /*
          /// Identifies the set of input buffers in the SrcMgr that are
    /// considered main source files.
    llvm::SetVector<unsigned> primaryBufferIDs;

          /// Return whether there is an entry in PrimaryInputs for buffer \p
    BufID. bool IsPrimaryInput(SrcID fileID) const { return
    primaryBufferIDs.count(fileID) != 0;
    }

    /// Record in PrimaryBufferIDs the fact that \p BufID is a primary.
    /// If \p BufID is already in the set, do nothing.
    void RecordPrimaryInputBuffer(SrcID fileID);
  */
 public:
  CompilerOptions compilerOpts;

 public:
  Compiler(const Compiler &) = delete;
  Compiler(Compiler &&) = delete;
  Compiler &operator=(const Compiler &) = delete;
  Compiler &operator=(Compiler &&) = delete;

  Compiler(Pipeline *pipeline = nullptr);

 public:
  /// Parse the given list of strings into an InputArgList.
  bool Build(llvm::ArrayRef<const char *> args) override;

  int Run() override;
  /// Parse the given list of strings into an InputArgList.
  void PrintLifecycle() override;
  void PrintHelp(bool showHidden) override;

  SearchPathOptions &GetSearchPathOptions() { return compilerOpts.spOpts; }
  const SearchPathOptions &GetSearchPathOptions() const {
    return compilerOpts.spOpts;
  }

  CompilerOptions &GetCompilerOptions() { return compilerOpts; }
  const CompilerOptions &GetCompilerOptions() const { return compilerOpts; }

  SrcMgr &GetSrcMgr() { return sm; }

  ASTContext &GetASTContext() { return *ac.get(); }
  // stone::syntax::Module &GetModule() { return *md.get(); }
  //
  /// Retrieve the main module containing the files being compiled.
  Module *GetMainModule() const;

  /// Replace the current main module with a new one. This is used for top-level
  /// cached code completion.
  void SetMainModule(Module *moduleDecl);

  void SetInputType(file::FileType ty) { compilerOpts.inputFileType = ty; }
  file::FileType GetInputKind() const { return compilerOpts.inputFileType; }

  void SetModuleName(llvm::StringRef name) { compilerOpts.moduleName = name; }
  const llvm::StringRef GetModuleName() const {
    return compilerOpts.moduleName;
  }

 protected:
  void ComputeMode(const llvm::opt::DerivedArgList &args) override;

  ModeKind GetDefaultModeKind() override;
  /// TranslateInputArgs - Create a new derived argument list from the input
  /// arguments, after applying the standard argument translations.
  // llvm::opt::DerivedArgList *
  // TranslateInputArgs(const llvm::opt::InputArgList &args) override const;
  static std::unique_ptr<Compiler> Create();

 private:
  void BuildInputs();

  // TODO:
  // void BuildCompileUnits();

 public:
  void *Allocate(size_t size, unsigned align) const {
    return bumpAlloc.Allocate(size, align);
  }
  template <typename T>
  T *Allocate(size_t num = 1) const {
    return static_cast<T *>(Allocate(num * sizeof(T), alignof(T)));
  }
  void Deallocate(void *ptr) const {}

 public:
  template <typename InputFileTy, typename AllocatorTy>
  static void *Allocate(AllocatorTy &alloc, size_t baseSize) {
    static_assert(alignof(InputFileTy) >= sizeof(void *),
                  "A pointer must fit in the alignment of the InputFile!");

    return (void *)alloc.Allocate(baseSize, alignof(InputFileTy));
  }
};
}  // namespace stone
#endif
