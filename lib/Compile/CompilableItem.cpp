#include "stone/Compile/CompilableItem.h"
#include "stone/Compile/Compiler.h"

using namespace stone;

bool CompilableItem::CanOutput() { return compiler.GetMode().CanOutput(); }

void CompilableItem::CreateOutputFile() {}

void CompilableItem::PurgeOutputFile() {}