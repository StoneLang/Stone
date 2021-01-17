#include "stone/Session/Session.h"

#include "llvm/Support/FileSystem.h"

using namespace stone;
using namespace llvm::opt;

Session::Session(SessionOptions &sessionOpts)
    : sessionOpts(sessionOpts),
      mode(ModeKind::None),
      targetTriple(llvm::sys::getDefaultTargetTriple()),
      fileSystem(llvm::vfs::getRealFileSystem()),
      strSaver(bumpAlloc) {
  startTime = std::chrono::system_clock::now();
}

Session::~Session() {}

std::unique_ptr<llvm::opt::InputArgList> Session::BuildArgList(
    llvm::ArrayRef<const char *> args) {
  std::unique_ptr<llvm::opt::InputArgList> argList =
      llvm::make_unique<llvm::opt::InputArgList>(
          sessionOpts.GetOpts().ParseArgs(args, missingArgIndex,
                                          missingArgCount, includedFlagsBitmask,
                                          excludedFlagsBitmask));

  assert(argList && "No input argument list.");

  // Check for missing argument error.
  if (missingArgCount) {
    os << "D(SrcLoc(),"
       << "msg::error_missing_arg_value,"
       << "argList->getArgString(missingArgIndex),"
       << "missingArgCount" << '\n';
    return nullptr;
  }

  // Check for unknown arguments.
  for (const llvm::opt::Arg *arg : argList->filtered(opts::UNKNOWN)) {
    os << "D(SourceLoc(), "
       << "msg::error_unknown_arg,"
       << "arg->getAsString(*ArgList));" << '\n';
    return nullptr;
  }
  return argList;
}

void Session::SetTargetTriple(const llvm::Triple &triple) {
  // TODO: langOpts.SetTarget(triple);
}
void Session::SetTargetTriple(llvm::StringRef triple) {
  SetTargetTriple(llvm::Triple(triple));
}

void Session::ComputeMode(const llvm::opt::DerivedArgList &args) {
  assert(mode.GetKind() == ModeKind::None && "mode id already computed");
  const llvm::opt::Arg *const modeArg = args.getLastArg(opts::ModeGroup);

  // TODO: may have to claim
  if (modeArg) {
    switch (modeArg->getOption().getID()) {
      case opts::Parse:
        mode.SetKind(ModeKind::Parse);
        break;
      case opts::Check:
        mode.SetKind(ModeKind::Check);
        break;
      case opts::EmitIR:
        mode.SetKind(ModeKind::EmitIR);
        break;
      case opts::EmitBC:
        mode.SetKind(ModeKind::EmitBC);
        break;
      case opts::EmitObject:
        mode.SetKind(ModeKind::EmitObject);
        break;
      case opts::EmitAssembly:
        mode.SetKind(ModeKind::EmitAssembly);
        break;
      case opts::EmitLibrary:
        mode.SetKind(ModeKind::EmitLibrary);
        break;
      default:
        break;
    }
  }
  if (mode.GetKind() == ModeKind::None) {
    mode.SetKind(GetDefaultModeKind());
  }
}
llvm::opt::DerivedArgList *Session::TranslateInputArgs(
    const llvm::opt::InputArgList &args) const {
  DerivedArgList *dArgList = new DerivedArgList(args);
  for (Arg *arg : args) {
    dArgList->append(arg);
  }
  return dArgList;
}

Mode &Session::GetMode() { return mode; }

/*
static llvm::StringRef Mode::GetNameByKind(ModeKind kind) {
        //TODO: I think you can get these from the mode group
        // so that you do not have to dublicate the text -- fragile.
  switch (kind) {
  case ModeKind::Parse:
    return "parse";
        case Modekind::Check :
                return "check";
  case ModeKind::EmitIR:
    return "emit-ir";
  case ModeKind::EmitBC:
    return "emit-bc";
  case ModeKind::EmitObject:
    return "emit-object";
  case ModeKind::EmitAssembly:
    return "emit-assembly";
  case ModeKind::EmitLibrary:
    return "emit-library";
  case ModeKind::EmitExecutable:
    return "emit-executable";
  }
        llvm_unreachable("Invalid ModeKind.");
}
*/

void Session::Purge() {}

void Session::Finish() {
  Purge();
  PrintDiagnostics();
  PrintStatistics();
}
void Session::PrintDiagnostics() {}
void Session::PrintStatistics() {}
