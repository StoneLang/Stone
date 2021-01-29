#ifndef STONE_DRIVER_TOOLCHAIN_H
#define STONE_DRIVER_TOOLCHAIN_H

#include <cassert>
#include <climits>
#include <memory>
#include <string>
#include <utility>

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/Triple.h"
#include "llvm/MC/MCTargetOptions.h"
#include "llvm/Option/Option.h"
#include "llvm/Support/VersionTuple.h"
#include "llvm/Target/TargetOptions.h"
#include "stone/Core/LLVM.h"
#include "stone/Core/Mem.h"
#include "stone/Driver/Job.h"
#include "stone/Session/FileType.h"
#include "stone/Session/Mode.h"
#include "stone/Session/Options.h"

namespace llvm {
namespace opt {
class Arg;
class ArgList;
class DerivedArgList;

}  // namespace opt
namespace vfs {
class FileSystem;
}  // namespace vfs
}  // namespace llvm

namespace stone {
namespace driver {
class ToolChain;
class Driver;
class OutputProfile;

/// The tool types that are supported
enum class ToolType {
  None,
  Assemble,
  Clang,
  DynamicLink,
  GCC,
  StaticLink,
  Stone
};
class ToolOptions final {
 public:
  bool canEmitIR = false;
  bool canAssemble = false;
  bool canLink = false;
};
class Tool {
  ToolType ty;
  /// The tool name (for debugging).
  llvm::StringRef fullName;
  /// The human readable name for the tool, for use in diagnostics.
  llvm::StringRef shortName;
  /// The tool chain this tool is a part of.
  const ToolChain &toolChain;

 public:
  /// Whether the tool is still on the system
  bool isOnSystem;
  /// Whether the tool is obsolete
  bool isObsolete;
  /// The version of the tool
  llvm::StringRef version;

 protected:
  ToolOptions toolOpts;

 public:
  Tool(llvm::StringRef fullName, llvm::StringRef shortName, ToolType ty,
       const ToolChain &toolChain);

 public:
  virtual ~Tool();

 public:
  bool IsOnSystem() { return isOnSystem; }
  bool IsObsolete() { return isObsolete; }
  llvm::StringRef GetFullName() { return fullName; }
  llvm::StringRef GetShortName() { return shortName; }
  ToolType GetType() { return ty; }
  ToolOptions &GetOptions() { return toolOpts; }
  const ToolOptions &GetOptions() const { return toolOpts; }
  const ToolChain &GetToolChain() const { return toolChain; }
};

class ClangTool : public Tool {
 public:
  ClangTool(llvm::StringRef fullName, llvm::StringRef shortName,
            const ToolChain &toolChain);
  ~ClangTool();
};

class StoneTool final : public Tool {
 public:
  StoneTool(llvm::StringRef fullName, llvm::StringRef shortName,
            const ToolChain &toolChain);
  ~StoneTool();
};

class GCCTool final : public Tool {
 public:
  GCCTool(llvm::StringRef fullName, llvm::StringRef shortName,
          const ToolChain &toolChain);
  ~GCCTool();
};

class DynamicLinkTool final : public Tool {
 public:
  DynamicLinkTool(llvm::StringRef fullName, llvm::StringRef shortName,
                  const ToolChain &toolChain);
  ~DynamicLinkTool();
};

class StaticLinkTool final : public Tool {
 public:
  StaticLinkTool(llvm::StringRef fullName, llvm::StringRef shortName,
                 const ToolChain &toolChain);
  ~StaticLinkTool();
};

class AssembleTool final : public Tool {
 public:
  AssembleTool(llvm::StringRef fullName, llvm::StringRef shortName,
               const ToolChain &toolChain);
  ~AssembleTool();
};

class ToolChain {
  /// A special name used to identify the 'stone' executable itself.
  constexpr static const char *const stoneExecutableName = "stone";

 protected:
  const Driver &driver;
  const llvm::Triple triple;

 public:
  using Paths = llvm::SmallVector<std::string, 16>;

 protected:
  ToolChain(const Driver &driver, const llvm::Triple &triple);

 private:
  /// The list of toolchain specific path prefixes to search for libraries.
  Paths libraryPaths;

  /// The list of toolchain specific path prefixes to search for files.
  Paths filePaths;

  /// The list of toolchain specific path prefixes to search for programs.
  Paths programPaths;

 protected:
  /// Tools that stone supports and looks for
  std::unique_ptr<ClangTool> clang;
  std::unique_ptr<DynamicLinkTool> staticLink;
  std::unique_ptr<StaticLinkTool> dynamicLink;
  std::unique_ptr<AssembleTool> assemble;
  std::unique_ptr<GCCTool> gcc;
  std::unique_ptr<StoneTool> stone;

  /// Since you are picking a tool you may need a dense map here
  // llvm::SmallVector<std::unique_ptr<Tool>, 4> tools;

 public:
  virtual ~ToolChain() = default;

  const Driver &GetDriver() { return driver; }
  const llvm::Triple &GetTriple() const { return triple; }

  /// Construct a Job for the action \p JA, taking the given information
  /// into account.
  ///
  /// This method dispatches to the various \c constructInvocation methods,
  /// which may be overridden by platform-specific subclasses.
  std::unique_ptr<Job> CreateJob(/*const CompilationActivity &activity, Compilation &compilation,
                                    llvm::SmallVectorImpl<const Job *> &&jobs,
                                    ArrayRef<const Activity *> activities,
                                    std::unique_ptr<CommandOutput> output,
                                    const OutputInfo &OI*/) const;

  // TODO: Move to tool
  /// Create a Job for the action \p JA, taking the given information
  /// into account.
  ///
  /// This method dispatches to the various \c constructInvocation methods,
  /// which may be overridden by platform-specific subclasses.
  std::unique_ptr<Job> CreateJob(Compilation &compilation,
                                 llvm::SmallVectorImpl<const Job *> &&jobs,
                                 std::unique_ptr<CmdOutput> cmdOutput,
                                 const OutputProfile &outputProfile) const;

  Paths &GetLibraryPaths() { return libraryPaths; }
  const Paths &GetLibraryPaths() const { return libraryPaths; }

  Paths &GetFilePaths() { return filePaths; }
  const Paths &GetFilePaths() const { return filePaths; }

  Paths &GetProgramPaths() { return programPaths; }
  const Paths &GetProgramPaths() const { return programPaths; }

 public:
  virtual bool Build() = 0;
  virtual Tool *GetTool(Mode mode) const = 0;
  /// Pick a tool to use to handle the compilation event \p event.
  ///
  /// This can be overridden when a particular ToolChain needs to use
  /// a compiler other than Clang.
  virtual Tool *PickTool(const Job &job) const;

 protected:
  virtual bool BuildClangTool() = 0;
  virtual bool BuildAssembleTool() = 0;
  virtual bool BuildDynamicLinkTool() = 0;
  virtual bool BuildStaticLinkTool() = 0;
  virtual bool BuildGCCTool() = 0;
  virtual bool BuildStoneTool() = 0;
};

class DarwinToolChain final : public ToolChain {
  const llvm::Optional<llvm::Triple> &targetVariant;

 public:
  DarwinToolChain(const Driver &driver, const llvm::Triple &triple,
                  const llvm::Optional<llvm::Triple> &targetVariant);
  ~DarwinToolChain() = default;

 public:
  bool Build() override;
  Tool *GetTool(Mode mode) const override;
  /// Pick a tool to use to handle the compilation event \p event.
  ///
  /// This can be overridden when a particular ToolChain needs to use
  /// a compiler other than Clang.
  Tool *PickTool(const Job &job) const override;

 protected:
  bool BuildClangTool() override;
  bool BuildAssembleTool() override;
  bool BuildDynamicLinkTool() override;
  bool BuildStaticLinkTool() override;
  bool BuildGCCTool() override;
  bool BuildStoneTool() override;
};

/*
class UnixToolChain : public ToolChain {
public:
  UnixToolChain(const Driver &driver, const llvm::Triple &triple);
  ~UnixToolChain() = default;
};
*/

/*
class LinuxToolChain final : public UnixToolChain {
public:
  LinuxToolChain(const Driver &driver, const llvm::Triple &triple);
  ~LinuxToolChain() = default;

public:
  Tool *BuildAssembleTool() override const;
  Tool *BuildLinkTool() override const;
  Tool *BuildStaticLibTool() override const;
  Tool *BuildDynamicLibTool() override const;
  Tool *GetTool(ModeType modeType) override const;
};

class FreeBSDToolChain final : public UnixToolChain {
public:
  FreeBSDToolChain(const Driver &driver, const llvm::Triple &triple);
  ~FreeBSDToolChain() = default;

public:
  Tool *BuildAssembleTool() override const;
  Tool *BuildLinkTool() override const;
  Tool *BuildStaticLibTool() override const;
  Tool *BuildDynamicLibTool() override const;
  Tool *GetTool(ModeType modeType) override const;
};
class OpenBSDToolChain final : public UnixToolChain {
public:
  OpenBSDToolChain(const Driver &driver, const llvm::Triple &triple);
  ~OpenBSDToolChain() = default;

public:
  Tool *BuildAssembleTool() override const;
  Tool *BuildLinkTool() override const;
  Tool *BuildStaticLibTool() override const;
  Tool *BuildDynamicLibTool() override const;
  Tool *GetTool(ModeType modeType) override const;
};

class WinToolChain : public ToolChain {
public:
  WinToolChain(const Driver &driver, const llvm::Triple &triple);
  ~WinToolChain() = default;

public:
  Tool *BuildAssembleTool() override const;
  Tool *BuildLinkTool() override const;
  Tool *BuildStaticLibTool() override const;
  Tool *BuildDynamicLibTool() override const;
  Tool *GetTool(ModeType modeType) override const;
};
*/
}  // namespace driver
}  // namespace stone
#endif
