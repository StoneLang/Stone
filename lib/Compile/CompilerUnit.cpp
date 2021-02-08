#include "stone/Compile/CompilerUnit.h"

#include "stone/Syntax/Module.h"

using namespace stone;
using namespace stone::syn;

CompilerUnit::CompilerUnit(Compiler &compiler, SourceFile &su,
                           CompilerScope &scope)
    : compiler(compiler), su(su), scope(scope) {}
