#include "stone/Basic/TokenType.h"
#include "llvm/ADT/StringRef.h"

using namespace stone;

bool stone::IsTokenTextDetermined(tk::Type ty) { return false; }

llvm::StringRef stone::GetTokenText(tk::Type ty) { return ""; }

void stone::DumpTokenKind(llvm::raw_ostream &os, tk::Type ty) {}
