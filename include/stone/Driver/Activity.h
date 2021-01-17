#ifndef STONE_DRIVER_ACTIVITY_H
#define STONE_DRIVER_ACTIVITY_H

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

namespace llvm {
namespace opt {
class Arg;
}
}  // namespace llvm

namespace stone {
namespace driver {

class Activity {
  unsigned kind : 4;
  unsigned ty : 28;

 public:
  using size_type = llvm::ArrayRef<const Activity *>::size_type;
  using iterator = llvm::ArrayRef<const Activity *>::iterator;
  using const_iterator = llvm::ArrayRef<const Activity *>::const_iterator;

  enum class Kind : unsigned {
    Input = 0,
    Compile,
    Backend,
    DynamicLink,
    StaticLink,
    Assemble,
    First = Compile,
    Last = Assemble
  };
  Activity(Activity::Kind kind, file::FileType ty)
      : kind(unsigned(kind)), ty(ty) {}
  virtual ~Activity() = default;

 public:
  Activity::Kind GetKind() const { return static_cast<Activity::Kind>(kind); }
  file::FileType GetType() const { return static_cast<file::FileType>(ty); }
  const char *GetName(Activity::Kind kind);

 private:
  friend class Compilation;
  /// Activitys must be created through Compilation::CreateActivity...
  void *operator new(size_t size) { return ::operator new(size); };
};

// An event that represents and input file
class InputActivity : public Activity {
  const llvm::opt::Arg &input;

 public:
  InputActivity(const llvm::opt::Arg &intput, file::FileType ty)
      : Activity(Activity::Kind::Input, ty), input(input) {}

  const llvm::opt::Arg &GetInput() const { return input; }

  static bool classof(const Activity *e) {
    return e->GetKind() == Activity::Kind::Input;
  }
};

class CompilationActivity : public Activity {
  bool isAsync = false;
  bool isTopLevel = false;

  // A list of user activities
  llvm::TinyPtrVector<const Activity *> inputs;

 public:
  CompilationActivity(Activity::Kind kind,
                      llvm::ArrayRef<const Activity *> inputs,
                      file::FileType ty)
      : Activity(kind, ty), inputs(inputs) {}

 public:
  llvm::ArrayRef<const Activity *> GetInputs() const { return inputs; }
  void AddInput(const Activity *input) { inputs.push_back(input); }

  size_type Size() const { return inputs.size(); }

  iterator begin() { return inputs.begin(); }
  iterator end() { return inputs.end(); }
  const_iterator begin() const { return inputs.begin(); }
  const_iterator end() const { return inputs.end(); }

  // Returns the index of the Input action's output file which is used as
  // (single) input to this action. Most actions produce only a single output
  // file, so we return 0 by default.
  virtual size_t GetInputIndex() const { return 0; }

  bool IsTopLevel() { return isTopLevel; }
  bool IsAsync() { return isAsync; }

  static bool classof(const Activity *e) {
    return (e->GetKind() >= Kind::First && e->GetKind() <= Kind::Last);
  }
};

class CompileActivity final : public CompilationActivity {
 public:
  class State {};

 public:
  CompileActivity(file::FileType outputType)
      : CompilationActivity(Activity::Kind::Compile, llvm::None, outputType) {}

  CompileActivity(Activity *input, file::FileType outputType)
      : CompilationActivity(Activity::Kind::Compile, input, outputType) {}

  static bool classof(const Activity *e) {
    return e->GetKind() == Activity::Kind::Compile;
  }

  /// Return a *single* FileType::stone InputActivity, if one exists;
  /// if 0 or >1 such inputs exist, return nullptr.
  const InputActivity *FindSingleInput(file::FileType ty) const {
    auto inputs = GetInputs();
    const InputActivity *e = nullptr;
    for (auto const *input : inputs) {
      if (auto const *s = dyn_cast<InputActivity>(input)) {
        if (s->GetType() == ty) {
          if (e == nullptr) {
            e = s;
          } else {
            // Already found one, two is too many.
            return nullptr;
          }
        }
      }
    }
    return e;
  }
};
class BackendActivity final : public CompilationActivity {
 private:
  // In case of multi-threaded compilation, the compile-event produces multiple
  // output bitcode-files. For each bitcode-file a BackendActivity is created.
  // This index specifies which of the files to select for the input.
  size_t inputIndex;

 public:
  BackendActivity(const Activity *input, file::FileType outputType,
                  size_t inputIndex)
      : CompilationActivity(Activity::Kind::Backend, input, outputType),
        inputIndex(inputIndex) {}

  static bool classof(const Activity *e) {
    return e->GetKind() == Activity::Kind::Backend;
  }
  virtual size_t GetInputIndex() const override { return inputIndex; }
};

class DynamicLinkActivity final : public CompilationActivity {
  LinkType linkType;
  bool shouldPerformLTO;

 public:
  DynamicLinkActivity(llvm::ArrayRef<const Activity *> inputs,
                      LinkType linkType, bool shouldPerformLTO)
      : CompilationActivity(Activity::Kind::DynamicLink, inputs,
                            file::FileType::Image),
        linkType(linkType),
        shouldPerformLTO(shouldPerformLTO) {
    assert(linkType != LinkType::None && linkType != LinkType::StaticLibrary);
  }

  LinkType GetLinkType() const { return linkType; }
  bool ShouldPerformLTO() const { return shouldPerformLTO; }

  static bool classof(const Activity *e) {
    return e->GetKind() == Activity::Kind::DynamicLink;
  }
};

class StaticLinkActivity : public CompilationActivity {
  LinkType linkType;

 public:
  StaticLinkActivity(ArrayRef<const Activity *> inputs, LinkType linkType)
      : CompilationActivity(Activity::Kind::StaticLink, inputs,
                            file::FileType::Image),
        linkType(linkType) {
    assert(linkType == LinkType::StaticLibrary);
  }
  static bool classof(const Activity *e) {
    return e->GetKind() == Activity::Kind::StaticLink;
  }
};

class AssembleActivity final : public CompilationActivity {
 public:
  AssembleActivity(const Activity *input, file::FileType outputType)
      : CompilationActivity(Activity::Kind::Assemble, input, outputType) {}

  static bool classof(const Activity *e) {
    return e->GetKind() == Activity::Kind::Assemble;
  }
};

class ActivityList final : public List<Activity> {
 public:
  void Print() const;
};
}  // namespace driver
}  // namespace stone
#endif
