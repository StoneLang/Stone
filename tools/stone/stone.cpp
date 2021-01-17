#include "llvm/ADT/Triple.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/Process.h"
#include "llvm/Support/Program.h"
#include "llvm/Support/StringSaver.h"
#include "llvm/Support/TargetSelect.h"
#include "stone/Compile/Compile.h"
#include "stone/Core/Ret.h"
#include "stone/Driver/Run.h"
#include "stone/Session/ExecutablePath.h"

using namespace stone;

int main(int argc, const char **args) {
  llvm::InitLLVM initLLVM(argc, args);
  if (llvm::sys::Process::FixupStandardFileDescriptors()) {
    return ret::err;
  }
  llvm::InitializeAllTargets();

  llvm::SmallVector<const char *, 256> argsToExpand(args, args + argc);
  llvm::BumpPtrAllocator ptrAlloc;
  llvm::StringSaver strSaver(ptrAlloc);

  llvm::cl::ExpandResponseFiles(
      strSaver,
      llvm::Triple(llvm::sys::getProcessTriple()).isOSWindows()
          ? llvm::cl::TokenizeWindowsCommandLine
          : llvm::cl::TokenizeGNUCommandLine,
      argsToExpand);

  llvm::ArrayRef<const char *> expandedArgs(argsToExpand);

  if (expandedArgs.size() > 1) {
    llvm::StringRef firstArg(expandedArgs[1]);
    if (firstArg == "-compile") {
      return stone::Compile(
          llvm::makeArrayRef(expandedArgs.data() + 2,
                             expandedArgs.data() + expandedArgs.size()),
          expandedArgs[0], (void *)(intptr_t)stone::GetExecutablePath, nullptr);
    }
  }

  return stone::Run(
      llvm::makeArrayRef(expandedArgs.data() + 1,
                         expandedArgs.data() + expandedArgs.size()),
      expandedArgs[0], (void *)(intptr_t)stone::GetExecutablePath);
}
