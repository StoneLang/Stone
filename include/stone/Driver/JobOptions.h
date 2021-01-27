#ifndef STONE_DRIVER_JOBOPTIONS_H
#define STONE_DRIVER_JOBOPTIONS_H

#include "stone/Session/FileType.h"
#include "stone/Session/SessionOptions.h"

namespace stone {
namespace driver {
class Job;
using OutputFileType = file::FileType;
using Jobs = llvm::SmallVector<const Job*, 4>;

class JobOptions final {
 public:
  InputFiles inputs;
  Jobs deps;
  OutputFileType outputFileType = file::FileType::None;

 public:
  JobOptions() {}
};
}  // namespace driver
}  // namespace stone

#endif
