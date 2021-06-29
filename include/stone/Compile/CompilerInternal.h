#ifndef STONE_COMPILE_COMPILERINTERNAL_H
#define STONE_COMPILE_COMPILERINTERNAL_H

#include <memory>

namespace stone {

class Compiler;
class CompilableItem;

namespace syn {
class SourceModuleFile;
}

namespace internal {

int ResolveImports(Compiler &compiler);
int ExecuteCompilable(Compiler &compiler, CompilableItem &compilable);

std::unique_ptr<CompilableItem> BuildCompilable(Compiler &compiler,
                                                file::File &input);
syn::SourceModuleFile *
BuildSourceModuleFileForMainModule(Compiler &compiler,
                                   CompilableItem &compilable);

int Parse(Compiler &compiler, CompilableItem &compilable);
int Parse(Compiler &compiler, CompilableItem &compilable, bool check);
int Check(Compiler &compiler, CompilableItem &compilable);
int EmitIR(Compiler &compiler, CompilableItem &compilable);
int EmitObject(Compiler &compiler, CompilableItem &compilable);
int EmitAssembly(Compiler &compiler, CompilableItem &compilable);
int EmitLibrary(Compiler &compiler, CompilableItem &compilable);
int EmitModuleOnly(Compiler &compiler, CompilableItem &compilable);
int EmitBitCode(Compiler &compiler, CompilableItem &compilable);

} // namespace internal
} // namespace stone

#endif