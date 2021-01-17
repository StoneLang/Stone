#ifndef STONE_SESSION_MODE_H
#define STONE_SESSION_MODE_H

namespace stone {

enum class ModeKind {
  None,
  Parse,
  Check,
  EmitIR,
  EmitBC,
  EmitObject,
  EmitLibrary,
  EmitModuleOnly,
  EmitAssembly,
  EmitExecutable
};

class Mode final {
  friend class Session;
  ModeKind kind;
  llvm::StringRef name;

  void SetKind(ModeKind k) { kind = k; }
  void SetName(llvm::StringRef v) { name = v; }

 public:
  ModeKind GetKind() { return kind; }
  llvm::StringRef GetName() const { return name; }

  bool IsOutput() {
    switch (GetKind()) {
      case ModeKind::EmitIR:
      case ModeKind::EmitBC:
      case ModeKind::EmitObject:
      case ModeKind::EmitAssembly:
      case ModeKind::EmitModuleOnly:
      case ModeKind::EmitLibrary:
      case ModeKind::EmitExecutable:
        return true;
      default:
        return false;
    }
  }
  bool IsCompileOnly() {
    switch (GetKind()) {
      case ModeKind::Parse:
      case ModeKind::Check:
      case ModeKind::EmitIR:
      case ModeKind::EmitBC:
      case ModeKind::EmitObject:
      case ModeKind::EmitModuleOnly:
      case ModeKind::EmitLibrary:
      case ModeKind::EmitAssembly:
        return true;
      default:
        return false;
    }
  }
  static llvm::StringRef GetNameByKind(ModeKind kind);

 private:
  Mode(ModeKind kind) : kind(kind) {}
};
}  // namespace stone
#endif
