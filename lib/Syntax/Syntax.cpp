#include "stone/Syntax/Syntax.h"

using namespace stone;
using namespace stone::syn;

Syntax::Syntax(TreeContext &tc) : tc(tc) {}

Syntax::~Syntax() {}

class Syntax::FunDeclFactory final {
public:
  FunDeclFactory();
  ~FunDeclFactory();

public:
  FunDecl *Make();
};

FunDecl *Syntax::FunDeclFactory::Make() { return nullptr; }