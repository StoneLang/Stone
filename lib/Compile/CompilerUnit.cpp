#include "stone/Compile/CompilerUnit.h"

#include "stone/Core/Module.h"

using namespace stone;
using namespace stone::syntax;

CompilerUnit::CompilerUnit(Compiler& compiler, SourceUnit& su,
                           CompilerScope& scope)
    : compiler(compiler), su(su), scope(scope) {}
