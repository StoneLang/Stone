#ifndef STONE_SESSION_MODEKIND_H
#define STONE_SESSION_MODEKIND_H

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
  EmitExecutable,
  Link
};
}
#endif
