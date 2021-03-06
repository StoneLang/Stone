#ifndef STONE_DRIVER_JOBTYPE_H
#define STONE_DRIVER_JOBTYPE_H

namespace stone {
namespace driver {
enum class JobType {
  None,
  Compile,
  Backend,
  MergeModule,
  DynamicLink,
  StaticLink,
  Assemble
};
}
} // namespace stone
#endif
