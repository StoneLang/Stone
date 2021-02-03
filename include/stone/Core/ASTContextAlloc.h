#ifndef STONE_ASTCTX_ALLOC_H
#define STONE_ASTCTX_ALLOC_H

#include <cstddef>

namespace stone {
namespace syn {
class ASTContext;
}
}  // namespace stone

// Defined in CompilationContext.h
void *operator new(size_t bytes, const stone::syn::ASTContext &ctx,
                   size_t alignment = 8);
//
void *operator new[](size_t bytes, const stone::syn::ASTContext &ctx,
                     size_t alignment = 8);
// It is good practice to pair new/delete operators.  Also, MSVC gives many
// warnings if a matching delete overload is not declared, even though the
// throw() spec guarantees it will not be implicitly called.
void operator delete(void *Ptr, const stone::syn::ASTContext &ctx, size_t);
//
void operator delete[](void *Ptr, const stone::syn::ASTContext &ctx, size_t);

#endif
