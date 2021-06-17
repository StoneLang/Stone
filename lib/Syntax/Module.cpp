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

void *ModuleFile::operator new(size_t bytes, TreeContext &tc,
                               unsigned alignment) {
  return tc.Allocate(bytes, alignment);
}

Module::Module(Identifier &name, TreeContext &tc)
    : DeclContext(Decl::Type::Module),
      TypeDecl(Decl::Type::Module, nullptr /*TODO: pass DeclContext*/, SrcLoc(),
               &name) {}

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
syn::Module *syn::Module::Create(Identifier &name, TreeContext &tc) {
  return new (tc) syn::Module(name, tc);
}
syn::Module *syn::Module::CreateMainModule(Identifier &name, TreeContext &tc) {
  auto *mod = syn::Module::Create(name, tc);
  // TODO: mod->Bits.ModuleDecl.IsMainModule = true;
  return mod;
}

SourceModuleFile::SourceModuleFile(SourceModuleFile::Kind kind, Module &owner,
                                   const SrcID srcID, bool isPrimary)
    : ModuleFile(ModuleFile::Kind::Source, owner), kind(kind), srcID(srcID),
      isPrimary(isPrimary) {}

syn::SourceModuleFile *
syn::SourceModuleFile::Create(SourceModuleFile::Kind kind, syn::Module &owner,
                              TreeContext &tc, bool isPrimary) {
  return nullptr;
}

SourceModuleFile::~SourceModuleFile() {}
