#ifndef STONE_DRIVER_JOBOPTIONS_H
#define STONE_DRIVER_JOBOPTIONS_H

#include "stone/Session/FileType.h"

namespace stone {
namespace driver {

using OutputFileType = file::FileType;
using Jobs = llvm::SmallVector<const Job*, 4>;

class JobOptions final {
 public:
	InputFiles inputs;
	Jobs jobs; 
  OutputFileType outputFileType = file::FileType::None;
 public:
  JobOptions() {}
};
}  // namespace driver
}  // namespace stone

#endif
