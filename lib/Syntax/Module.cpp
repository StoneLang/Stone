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

ModuleFile::ModuleFile(ModuleFile::Kind kind, Module &owner)
    : DeclContext(DeclContext::Type::File, Decl::Type::None, &owner),
      kind(kind) {}

Module::Module(Identifier &name, TreeContext &tc)
    : DeclContext(DeclContext::Type::Decl, Decl::Type::Module),
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

bool Module::Walk(Walker &waker) {}

SourceModuleFile::SourceModuleFile(SourceModuleFile::Kind kind,
                                   syn::Module &owner, const SrcID srcID,
                                   bool isPrimary)
    : ModuleFile(ModuleFile::Kind::Source, owner), kind(kind), srcID(srcID),
      isPrimary(isPrimary) {}

syn::SourceModuleFile *
syn::SourceModuleFile::Create(SourceModuleFile::Kind kind, syn::Module &owner,
                              TreeContext &tc, bool isPrimary) {
  return nullptr;
}

SourceModuleFile::~SourceModuleFile() {}
