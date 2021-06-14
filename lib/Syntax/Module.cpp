#include "stone/Syntax/Module.h"

#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/ADT/TinyPtrVector.h"
#include "llvm/Support/MD5.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/SaveAndRestore.h"
#include "llvm/Support/raw_ostream.h"

using namespace stone;
using namespace stone::syn;

void Module::AddFile(ModuleFile &file) {
  // If this is a LoadedFile, make sure it loaded without error.
  // assert(!(isa<LoadedFile>(newFile) &&
  //         cast<LoadedFile>(newFile).hadLoadError()));
  // Require Main and REPL files to be the first file added.
  assert(
      files.empty() || !isa<SourceModuleFile>(file) ||
      cast<SourceModuleFile>(file).kind == SourceModuleFile::Kind::Library
      /*||cast<SourceModuleFile>(unit).Kind == SourceModuleFile::Kind::SIL*/);
  files.push_back(&file);
  // ClearLookupCache();
}
syn::Module *syn::Module::Create(Identifier &identifier, TreeContext &tc) {

  return nullptr;
}
syn::Module *syn::Module::CreateMainModule(Identifier &identifier,
                                           TreeContext &tc) {

  return nullptr;
}
