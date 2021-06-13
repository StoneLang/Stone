#include "stone/Compile/CompilableItem.h"

using namespace stone;

std::unique_ptr<CompilableItem> CompilableItem::Create(file::File input,
                                                       Compiler &compiler) {

  // std::unique<CompilableItem> compilable(InputFile::Create(input), compiler);
  // if (compilable->CanOutput()) {
  //  comilable->CreateOutput();
  //}
  return nullptr;
}

void CompilableItem::CreateOutputFile() {}
