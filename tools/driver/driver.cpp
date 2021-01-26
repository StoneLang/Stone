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
#include "stone/Session/Mode.h"
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
using Jobs = llvm::SmallVector<const Job*, 4>;

/// If positive, contains llvm::ProcessID for a real Job on the host OS. If
/// negative, contains a quasi-PID, which identifies a Job that's a member of
/// a BatchJob _without_ denoting an operating system process.

using PID = int64_t;

class Job {
	Jobs deps;
	JobType jobType;
  InputFiles* inputs;
  OutputFileType output;

 public:
  // Some job depend on other jobs -- For example, LinkJob
  Job(JobType jobType, InputFiles* inputs, OutputFileType output)
      : jobType(jobType), inputs(inputs), output(output) {}

  // Some jobs only consume inputs -- For example, LinkJob
  Job(JobType jobType, Jobs deps, OutputFileType output)
      : jobType(jobType), deps(deps), output(output) {}

  JobType GetType() const { return jobType; }
  Jobs& GetDeps() { return deps; }
  InputFiles& GetInputs() { return *inputs; }
  OutputFileType GetOutputFileType() { return output; }
};

class CompileJob final : public Job {
 public:
  // Some job depend on other jobs -- For example, LinkJob
  CompileJob(InputFiles* inputs, OutputFileType output)
      : Job(JobType::Compile, inputs, output) {}

	// Some job depend on other jobs -- For example, LinkJob
  //CompileJob(Jobs deps, OutputFileType output)
  //    : Job(JobType::Compile, inputs, output) {}

};

class LinkJob : public Job {
  LinkType linkType;

 public:
  // Some jobs only consume inputs -- For example, LinkJob
  LinkJob(JobType jobType, InputFiles* inputs, LinkType linkType,
          OutputFileType output)
      : Job(jobType, inputs, output), linkType(linkType) {}

  // Some jobs only consume inputs -- For example, LinkJob
  LinkJob(JobType jobType, Jobs deps, LinkType linkType, OutputFileType output)
      : Job(JobType::StaticLink, deps, output), linkType(linkType) {}

 public:
  LinkType GetLinkType() { return linkType; }
};
class StaticLinkJob final : public LinkJob {
 public:
  // Some jobs only consume inputs -- For example, LinkJob
  StaticLinkJob(InputFiles* inputs, LinkType linkType, OutputFileType output)
      : LinkJob(JobType::StaticLink, inputs, linkType, output) {}

  // Some jobs only consume inputs -- For example, LinkJob
  StaticLinkJob(Jobs deps, LinkType linkType, OutputFileType output)
      : LinkJob(JobType::StaticLink, deps, linkType, output) {}
};


class DynamicLinkJob final : public LinkJob {
 public:
  // Some jobs only consume inputs -- For example, LinkJob
  DynamicLinkJob(InputFiles* inputs, LinkType linkType, OutputFileType output)
      : LinkJob(JobType::DynamicLink, inputs, linkType, output) {}

  // Some jobs only consume inputs -- For example, LinkJob
  DynamicLinkJob(Jobs deps, LinkType linkType, OutputFileType output)
      : LinkJob(JobType::DynamicLink, deps, linkType, output) {}
};

int main() { return 0; }
