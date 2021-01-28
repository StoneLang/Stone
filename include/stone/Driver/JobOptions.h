#ifndef STONE_DRIVER_JOBOPTIONS_H
#define STONE_DRIVER_JOBOPTIONS_H

#include "stone/Session/FileType.h"
#include "stone/Session/SessionOptions.h"

namespace stone {
namespace driver {

using OutputFileType = file::FileType;

class JobOptions final {
 public:
  InputFiles inputs;
  OutputFileType outputFileType = file::FileType::None;

 public:
  JobOptions() {}
};
}  // namespace driver
}  // namespace stone

#endif
