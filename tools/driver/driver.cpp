#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/DenseMapInfo.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/ADT/TinyPtrVector.h"
#include "llvm/Support/Allocator.h"
#include "llvm/Support/Chrono.h"
#include "llvm/Support/PointerLikeTypeTraits.h"
#include "llvm/Support/type_traits.h"
#include "stone/Core/List.h"
#include "stone/Driver/LinkType.h"
#include "stone/Session/FileType.h"
#include "stone/Session/SessionOptions.h"

using namespace stone;
using namespace stone::driver;

enum class JobType {
  None,
  Compile,
  Backend,
  DynamicLink,
  StaticLink,
  Assemble,
  First = Compile,
  Last = Assemble
};

using OutputFileType = file::FileType;
class CrashCondition {};

class Job;
using Jobs = llvm::ArrayRef<const Job *>;
using Inputs = llvm::ArrayRef<const InputFile *>;

class Job {
 private:
  // TODO: Mode
  //
  JobType jobType;
  Jobs deps;
  Inputs inputs;
  OutputFileType output;

  bool isAsync;

 public:
  // Some job depend on other jobs -- For example, LinkJob
  Job(JobType jobType, Inputs inputs, OutputFileType output)
      : jobType(jobType), inputs(inputs), output(output) {}

  // Some jobs only consume inputs -- For example, LinkJob
  Job(JobType jobType, Jobs deps, OutputFileType output)
      : jobType(jobType), deps(deps), output(output) {}

  /*
JobType GetType() const { return jobType; }
llvm::ArrayRef<const Job *> GetDeps() { return deps; }
llvm::ArrayRef<const InputFile *> GetInputs() { return inputs; }
OutputFileType GetOutputFileType() { return output; }
bool IsAsync() { return isAsync; }
  */
};

/*
class CompileJob final : public Job {
 public:
  CompileJob(Job::Input *input, OutputFileType output)
      : Job(JobType::Compile, input, output) {}
};

class StaticLinkJob final : public Job {
  LinkType linkType;

 public:
  StaticLinkJob(llvm::ArrayRef<const Job *> deps, LinkType linkType)
      : Job(JobType::StaticLink, deps, file::FileType::Image),
        linkType(linkType) {}

  StaticLinkJob(llvm::ArrayRef<const Job::Input *> inputs, LinkType linkType)
      : Job(JobType::StaticLink, inputs, file::FileType::Image),
        linkType(linkType) {}
};
*/

int main() { return 0; }
