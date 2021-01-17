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

class Tool {
  /// The tool name (for debugging).
  llvm::StringRef fullName;
  /// The human readable name for the tool, for use in diagnostics.
  llvm::StringRef shortName;
  /// The tool chain this tool is a part of.
  const ToolChain &toolChain;

 public:
  /// Whether the tool is still on the system
  bool isOnSystem;
  /// Whether the tools is obsolete
  bool isObsolete;

 protected:
  /// The tool is able to emit ir
  bool canEmitIR;
  /// The tool has a builtin assemble
  bool hasBuiltinAssembler;
  /// The tool as a builtin linker
  bool hasBuiltinLinker;

 public:
  Tool(llvm::StringRef fullName, llvm::StringRef shortName,
       const ToolChain &toolChain);

 public:
  virtual ~Tool();

 public:
  bool IsOnSystem() { return isOnSystem; }
  bool IsObsolete() { return isObsolete; }
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
  const Driver &driver;
  const llvm::Triple triple;

  /// A special name used to identify the 'stone' executable itself.
  constexpr static const char *const stoneExecutableName = "stone";

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
  std::unique_ptr<DynamicLinkTool> staticLinkTool;
  std::unique_ptr<StaticLinkTool> dynamicLinkTool;
  std::unique_ptr<AssembleTool> assembleTool;
  std::unique_ptr<GCCTool> gccTool;
  std::unique_ptr<StoneTool> StoneTool;

 public:
  virtual ~ToolChain() = default;

  const Driver &GetDriver() const { return driver; }
  const llvm::Triple &GetTriple() const { return triple; }

  /// Construct a Job for the action \p JA, taking the given information
  /// into account.
  ///
  /// This method dispatches to the various \c constructInvocation methods,
  /// which may be overridden by platform-specific subclasses.
  std::unique_ptr<Job> CreateJob(/*const CompilationActivity &event, Compilation &compilation,
                                    llvm::SmallVectorImpl<const Job *> &&jobs,
                                    ArrayRef<const Activity *> activities,
                                    std::unique_ptr<CommandOutput> output,
                                    const OutputInfo &OI*/) const;

  Paths &GetLibraryPaths() { return libraryPaths; }
  const Paths &GetLibraryPaths() const { return libraryPaths; }

  Paths &GetFilePaths() { return filePaths; }
  const Paths &GetFilePaths() const { return filePaths; }

  Paths &GetProgramPaths() { return programPaths; }
  const Paths &GetProgramPaths() const { return programPaths; }

  /// Pick a tool to use to handle the compilation event \p event.
  ///
  /// This can be overridden when a particular ToolChain needs to use
  /// a compiler other than Clang.
  virtual Tool *PickTool(const CompilationActivity &event) const;

 public:
  virtual Tool *BuildClangTool() const = 0;
  virtual Tool *BuildAssembleTool() const = 0;
  virtual Tool *BuildDynamicLinkTool() const = 0;
  virtual Tool *BuildStaticLinkTool() const = 0;
  virtual Tool *BuildGCCTool() const = 0;
  virtual Tool *BuildStoneTool() const = 0;
  virtual Tool *GetTool(ModeKind kind) const = 0;
};

class DarwinToolChain final : public ToolChain {
  const llvm::Optional<llvm::Triple> &targetVariant;

 public:
  DarwinToolChain(const Driver &driver, const llvm::Triple &triple,
                  const llvm::Optional<llvm::Triple> &targetVariant);
  ~DarwinToolChain() = default;

 public:
  Tool *BuildClangTool() const override;
  Tool *BuildAssembleTool() const override;
  Tool *BuildDynamicLinkTool() const override;
  Tool *BuildStaticLinkTool() const override;
  Tool *BuildGCCTool() const override;
  Tool *BuildStoneTool() const override;

  Tool *GetTool(ModeKind kind) const override;
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
