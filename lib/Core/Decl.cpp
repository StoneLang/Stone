#include "stone/Core/Decl.h"

#include "stone/Core/ASTContext.h"
#include "stone/Core/Decl.h"
//#include "stone/Core/DeclContextInternals.h"
// TODO: #include "stone/Core/Friend.h"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <string>
#include <tuple>
#include <utility>

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/Support/VersionTuple.h"
#include "llvm/Support/raw_ostream.h"
#include "stone/Core/Identifier.h"
#include "stone/Core/LLVM.h"
#include "stone/Core/LangOptions.h"
#include "stone/Core/Module.h"
#include "stone/Core/SrcLoc.h"
#include "stone/Core/Stmt.h"
#include "stone/Core/Target.h"
#include "stone/Core/Template.h"  //DeclTemplate
#include "stone/Core/Type.h"

using namespace stone;
using namespace stone::syntax;

void *Decl::operator new(std::size_t size, const ASTContext &astCtx,
                         unsigned globalDeclID, std::size_t extra) {
  // Allocate an extra 8 bytes worth of storage, which ensures that the
  // resulting pointer will still be 8-byte aligned.
  static_assert(sizeof(unsigned) * 2 >= alignof(Decl),
                "Decl won't be misaligned");

  void *start = astCtx.Allocate(size + extra + 8);
  void *result = (char *)start + 8;

  unsigned *prefixPtr = (unsigned *)result - 2;

  // Zero out the first 4 bytes; this is used to store the owning module ID.
  prefixPtr[0] = 0;
  // Store the global declaration ID in the second 4 bytes.
  prefixPtr[1] = globalDeclID;

  return result;
}

void *Decl::operator new(std::size_t size, const ASTContext &astCtx,
                         DeclContext *parentDeclContext, std::size_t extra) {
  /*TODO:
    assert(!parent || &parent->GetParentASTContext() == &astCtx);
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
  */

  return ::operator new(size + extra, astCtx);
}

// stone::Module *Decl::GetOwningModule() const {
//  assert(IsFromASTFile() && "Not from AST file?");
//  return GetASTContext().GetExternalSource()->GetModule(GetOwningModuleID());
//}
//
//
//

void DeclStats::Print() const {}
