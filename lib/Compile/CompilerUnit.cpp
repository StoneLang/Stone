#include "stone/Compile/CompilerUnit.h"

#include "stone/Syntax/Module.h"

using namespace stone;
using namespace stone::syn;

CompilerUnit::CompilerUnit(Compiler &compiler, SourceUnit &su,
                           CompilerScope &scope)
    : compiler(compiler), su(su), scope(scope) {}
