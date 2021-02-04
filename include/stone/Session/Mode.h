#ifndef STONE_SESSION_MODE_H
#define STONE_SESSION_MODE_H

#include "stone/Session/ModeKind.h"

namespace stone {

class Mode final {
  friend class Session;
  ModeKind kind;
  llvm::StringRef name;

  void SetKind(ModeKind k) { kind = k; }
  void SetName(llvm::StringRef v) { name = v; }

private:
  bool IsCompileOnlyImpl() const {
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

public:
  ModeKind GetKind() const { return kind; }
  llvm::StringRef GetName() const { return name; }

  bool CanOutput() {
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
  bool IsCompileOnly() const {
    if (CanLink())
      return false;
    return IsCompileOnlyImpl();
  }

  bool CanCompile() const {
    switch (GetKind()) {
    case ModeKind::EmitExecutable:
      return true;
    default:
      return IsCompileOnlyImpl();
    }
  }
  bool IsLinkOnly() const { return GetKind() == ModeKind::Link; }
  bool CanLink() const {
    switch (GetKind()) {
    case ModeKind::EmitExecutable:
    case ModeKind::Link:
      return true;
    default:
      return false;
    }
  }

  static llvm::StringRef GetNameByKind(ModeKind kind);

private:
  Mode(ModeKind kind) : kind(kind) {}
};
} // namespace stone
#endif
