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
#include "stone/Driver/LinkType.h"
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
enum class ToolType { None, Assemble, Clang, GCC, LD, LLD, Stone };

class ToolOptions final {
 public:
  bool canEmitIR = false;
  bool canAssemble = false;
  bool canLink = false;
};
class LinkTool;

class Tool {
  ToolType toolType;
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
  SafeList<Job> jobs;
  ToolOptions toolOpts;

 public:
  Tool(llvm::StringRef fullName, llvm::StringRef shortName, ToolType toolType,
       const ToolChain &toolChain);
  virtual ~Tool();

 public:
  /// Create a Job
  /// into account.
  ///
  /// This method dispatches to the various \c constructInvocation methods,
  /// which may be overridden by platform-specific subclasses.
  virtual Job *CreateJob(Compilation &compilation,
                         std::unique_ptr<CmdOutput> cmdOutput,
                         const OutputProfile &outputProfile) = 0;

  /// Create a Job
  /// into account.
  ///
  /// This method dispatches to the various \c constructInvocation methods,
  /// which may be overridden by platform-specific subclasses.
  virtual Job *CreateJob(Compilation &compilation,
                         llvm::SmallVectorImpl<const Job *> &&deps,
                         std::unique_ptr<CmdOutput> cmdOutput,
                         const OutputProfile &outputProfile) = 0;

 public:
  bool IsOnSystem() { return isOnSystem; }
  bool IsObsolete() { return isObsolete; }
  llvm::StringRef GetFullName() { return fullName; }
  llvm::StringRef GetShortName() { return shortName; }
  ToolType GetType() { return toolType; }
  ToolOptions &GetOptions() { return toolOpts; }
  const ToolOptions &GetOptions() const { return toolOpts; }
  const ToolChain &GetToolChain() const { return toolChain; }
  const llvm::StringRef &GetVersion() const { return version; }
  const SafeList<Job> &GetJobs() const { return jobs; }
};

class StoneTool final : public Tool {
 public:
  StoneTool(llvm::StringRef fullName, llvm::StringRef shortName,
            const ToolChain &toolChain);
  ~StoneTool();

 public:
  /// Create a Job for the action \p JA, taking the given information
  /// into account.
  ///
  /// This method dispatches to the various \c constructInvocation methods,
  /// which may be overridden by platform-specific subclasses.
  virtual Job *CreateJob(Compilation &compilation,
                         std::unique_ptr<CmdOutput> cmdOutput,
                         const OutputProfile &outputProfile) override;

  /// Create a Job for the action \p JA, taking the given information
  /// into account.
  ///
  /// This method dispatches to the various \c constructInvocation methods,
  /// which may be overridden by platform-specific subclasses.
  virtual Job *CreateJob(Compilation &compilation,
                         llvm::SmallVectorImpl<const Job *> &&deps,
                         std::unique_ptr<CmdOutput> cmdOutput,
                         const OutputProfile &outputProfile) override;
};

class LinkTool : public Tool {
 protected:
  LinkType linkType;

 public:
  LinkTool(llvm::StringRef fullName, llvm::StringRef shortName, ToolType toolTy,
           const ToolChain &toolChain, LinkType linkTy);
  ~LinkTool();
  LinkType GetLinkType() { return linkType; }

 public:
  /// Create a Job for the action \p JA, taking the given information
  /// into account.
  ///
  /// This method dispatches to the various \c constructInvocation methods,
  /// which may be overridden by platform-specific subclasses.
  virtual Job *CreateJob(Compilation &compilation,
                         std::unique_ptr<CmdOutput> cmdOutput,
                         const OutputProfile &outputProfile) override;

  /// Create a Job for the action \p JA, taking the given information
  /// into account.
  ///
  /// This method dispatches to the various \c constructInvocation methods,
  /// which may be overridden by platform-specific subclasses.
  virtual Job *CreateJob(Compilation &compilation,
                         llvm::SmallVectorImpl<const Job *> &&deps,
                         std::unique_ptr<CmdOutput> cmdOutput,
                         const OutputProfile &outputProfile) override;
};

class LDLinkTool final : public LinkTool {
 public:
  LDLinkTool(llvm::StringRef fullName, llvm::StringRef shortName,
             const ToolChain &toolChain, LinkType linkTy);
  ~LDLinkTool();
};

class LLDLinkTool final : public LinkTool {
 public:
  LLDLinkTool(llvm::StringRef fullName, llvm::StringRef shortName,
              const ToolChain &toolChain, LinkType linkTy);
  ~LLDLinkTool();
};

class ClangTool : public LinkTool {
 public:
  ClangTool(llvm::StringRef fullName, llvm::StringRef shortName,
            const ToolChain &toolChain, LinkType linkTy);
  ~ClangTool();

 public:
  /// Create a Job for the action \p JA, taking the given information
  /// into account.
  ///
  /// This method dispatches to the various \c constructInvocation methods,
  /// which may be overridden by platform-specific subclasses.
  virtual Job *CreateJob(Compilation &compilation,
                         std::unique_ptr<CmdOutput> cmdOutput,
                         const OutputProfile &outputProfile) override;

  /// Create a Job for the action \p JA, taking the given information
  /// into account.
  ///
  /// This method dispatches to the various \c constructInvocation methods,
  /// which may be overridden by platform-specific subclasses.
  virtual Job *CreateJob(Compilation &compilation,
                         llvm::SmallVectorImpl<const Job *> &&deps,
                         std::unique_ptr<CmdOutput> cmdOutput,
                         const OutputProfile &outputProfile) override;
};

class GCCTool final : public LinkTool {
 public:
  GCCTool(llvm::StringRef fullName, llvm::StringRef shortName,
          const ToolChain &toolChain, LinkType linkTy);
  ~GCCTool();

 public:
  /// Create a Job for the action \p JA, taking the given information
  /// into account.
  ///
  /// This method dispatches to the various \c constructInvocation methods,
  /// which may be overridden by platform-specific subclasses.
  virtual Job *CreateJob(Compilation &compilation,
                         std::unique_ptr<CmdOutput> cmdOutput,
                         const OutputProfile &outputProfile) override;

  /// Create a Job for the action \p JA, taking the given information
  /// into account.
  ///
  /// This method dispatches to the various \c constructInvocation methods,
  /// which may be overridden by platform-specific subclasses.
  virtual Job *CreateJob(Compilation &compilation,
                         llvm::SmallVectorImpl<const Job *> &&deps,
                         std::unique_ptr<CmdOutput> cmdOutput,
                         const OutputProfile &outputProfile) override;
};

class AssembleTool final : public Tool {
 public:
  AssembleTool(llvm::StringRef fullName, llvm::StringRef shortName,
               const ToolChain &toolChain);
  ~AssembleTool();

 public:
  /// Create a Job for the action \p JA, taking the given information
  /// into account.
  ///
  /// This method dispatches to the various \c constructInvocation methods,
  /// which may be overridden by platform-specific subclasses.
  virtual Job *CreateJob(Compilation &compilation,
                         std::unique_ptr<CmdOutput> cmdOutput,
                         const OutputProfile &outputProfile) override;

  /// Create a Job for the action \p JA, taking the given information
  /// into account.
  ///
  /// This method dispatches to the various \c constructInvocation methods,
  /// which may be overridden by platform-specific subclasses.
  virtual Job *CreateJob(Compilation &compilation,
                         llvm::SmallVectorImpl<const Job *> &&deps,
                         std::unique_ptr<CmdOutput> cmdOutput,
                         const OutputProfile &outputProfile) override;
};

/// A map for caching Jobs for a given Action/ToolChain pair
// using ToolCacheMap = llvm::DenseMap<ToolType, std::unique_ptr<Tool>>;

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
  std::unique_ptr<ClangTool> clangTool;
  std::unique_ptr<LinkTool> linkTool;
  std::unique_ptr<AssembleTool> assembleTool;
  std::unique_ptr<GCCTool> gccTool;
  std::unique_ptr<StoneTool> stoneTool;

  // ToolCacheMap tools;

 public:
  virtual ~ToolChain() = default;

  const Driver &GetDriver() { return driver; }
  const llvm::Triple &GetTriple() const { return triple; }

  Paths &GetLibraryPaths() { return libraryPaths; }
  const Paths &GetLibraryPaths() const { return libraryPaths; }

  Paths &GetFilePaths() { return filePaths; }
  const Paths &GetFilePaths() const { return filePaths; }

  Paths &GetProgramPaths() { return programPaths; }
  const Paths &GetProgramPaths() const { return programPaths; }

  llvm::Triple::ArchType GetArchType() { return triple.getArch(); }
  const llvm::Triple::ArchType GetArchType() const { return triple.getArch(); }

 public:
  virtual bool Build();
  /// Pick a tool to use to handle the compilation event \p event.
  virtual Tool *PickTool(JobType jobType) const;

 protected:
  virtual bool BuildClangTool() = 0;
  virtual bool BuildAssembleTool() = 0;
  virtual bool BuildLDLinkTool() = 0;
  virtual bool BuildLLDLinkTool() = 0;
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
  /// Pick a tool to use to handle the compilation event \p event.
  ///
  /// This can be overridden when a particular ToolChain needs to use
  /// a compiler other than Clang.
  Tool *PickTool(JobType jobType) const override;

 protected:
  // bool BuildTools() override;

  bool BuildClangTool() override;
  bool BuildAssembleTool() override;
  bool BuildLDLinkTool() override;
  bool BuildLLDLinkTool() override;
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
