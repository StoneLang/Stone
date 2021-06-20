#include "stone/Syntax/Syntax.h"

using namespace stone;
using namespace stone::syn;

Syntax::Syntax(TreeContext &tc) : tc(tc) {}

Syntax::~Syntax() {}

class Syntax::FunDeclFactory {
public:
  FunDeclFactory();
  ~FunDeclFactory();

public:
  FunDecl *Create();
};

FunDecl *Syntax::FunDeclFactory::Create() { return nullptr; }