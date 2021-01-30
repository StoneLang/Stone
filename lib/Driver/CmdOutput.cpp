#include "stone/Driver/CmdOutput.h"

using namespace stone;
using namespace stone::driver;

CmdOutput::CmdOutput(file::FileType primaryOutputType, OutputFileMap& map)
    : derivedOutputMap(map) {}

CmdOutput::CmdOutput(llvm::StringRef fakeBaseName, OutputFileMap& map)
    : derivedOutputMap(map) {}
