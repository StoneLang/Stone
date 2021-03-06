#include "stone/Syntax/Decl.h"

//#include "stone/Syntax/DeclContextInternals.h"
// TODO: #include "stone/Syntax/Friend.h"

#include "stone/Basic/LLVM.h"
#include "stone/Basic/LangOptions.h"
#include "stone/Basic/SrcLoc.h"
#include "stone/Basic/Target.h"
#include "stone/Syntax/Identifier.h"
#include "stone/Syntax/Module.h"
#include "stone/Syntax/Stmt.h"
#include "stone/Syntax/Template.h" //DeclTemplate
#include "stone/Syntax/Type.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/Support/VersionTuple.h"
#include "llvm/Support/raw_ostream.h"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <string>
#include <tuple>
#include <utility>

using namespace stone;
using namespace stone::syn;

void DeclStats::Print() {}

// Only allow allocation of Decls using the allocator in ASTContext.
void *syn::Decl::operator new(std::size_t bytes, const TreeContext &tc,
                              unsigned alignment) {
  return tc.Allocate(bytes, alignment);
}

// Only allow allocation of Modules using the allocator in ASTContext.
void *syn::Module::operator new(std::size_t bytes, const TreeContext &tc,
                                unsigned alignment) {
  return tc.Allocate(bytes, alignment);
}

/*
void *Decl::operator new(std::size_t size, const TreeContext &tc,
                         unsigned globalDeclID, std::size_t extra) {
  // Allocate an extra 8 bytes worth of storage, which ensures that the
  // resulting pointer will still be 8-byte aligned.
  static_assert(sizeof(unsigned) * 2 >= alignof(Decl),
                "Decl won't be misaligned");

  void *start = tc.Allocate(size + extra + 8);
  void *result = (char *)start + 8;

  unsigned *prefixPtr = (unsigned *)result - 2;

  // Zero out the first 4 bytes; this is used to store the owning module ID.
  prefixPtr[0] = 0;
  // Store the global declaration ID in the second 4 bytes.
  prefixPtr[1] = globalDeclID;

  return result;
}
*/

/*
void *Decl::operator new(std::size_t size, const TreeContext &tc,
                         DeclContext *parentDeclContext, std::size_t extra) {

    assert(!parent || &parent->GetParentTreeContext() == &astCtx);
    // With local visibility enabled, we track the owning module even for local
    // declarations. We create the TU decl early and may not yet know what the
    // LangOpts are, so conservatively allocate the storage.
    if (astCtx.GetLangOpts().TrackLocalOwningModule() || !parentDeclContext) {
      // Ensure required alignment of the resulting object by adding extra
      // padding at the start if required.
      size_t extraAlign =
          llvm::offsetToAlignment(sizeof(Module *), llvm::Align(alignof(Decl)));

      auto *buffer = reinterpret_cast<char *>(
          ::operator new(extraAlign + sizeof(Module *) + size + extra, astCtx));

      buffer += extraAlign;

      auto *parentModule =
          parentDeclContext ? cast<Decl>(parentDeclContext)->GetOwningModule() :
    nullptr;

      return new (buffer) Module*(parentModule) + 1;
    }

  return ::operator new(size + extra, tc);
}
*/

// stone::Module *Decl::GetOwningModule() const {
//  assert(IsFromASTFile() && "Not from AST file?");
//  return GetTreeContext().GetExternalSource()->GetModule(GetOwningModuleID());
//}
//
//
//
