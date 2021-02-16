#ifndef STONE_DRIVER_JOBOPTIONS_H
#define STONE_DRIVER_JOBOPTIONS_H

#include "stone/Session/SessionOptions.h"
#include "stone/Utils/File.h"

namespace stone {
namespace driver {

using OutputType = file::Type;

class JobOptions final {
public:
  file::Files inputs;
  OutputType outputType = file::Type::None;

public:
  JobOptions() {}
};
} // namespace driver
} // namespace stone

#endif
