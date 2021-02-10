#include "stone/Session/Session.h"

#include "llvm/Support/FileSystem.h"

using namespace stone;
using namespace llvm::opt;

Session::Session(SessionOptions &sessionOpts)
    : sessionOpts(sessionOpts), mode(ModeType::None),
      targetTriple(llvm::sys::getDefaultTargetTriple()),
      fileSystem(llvm::vfs::getRealFileSystem()), strSaver(bumpAlloc) {

  // TODO: -print-stats
  timer.startTimer();
}

Session::~Session() {}

std::unique_ptr<llvm::opt::InputArgList>
Session::ParseArgList(llvm::ArrayRef<const char *> args) {
  auto argList = llvm::make_unique<llvm::opt::InputArgList>(
      sessionOpts.GetOpts().ParseArgs(args, missingArgIndex, missingArgCount,
                                      includedFlagsBitmask,
                                      excludedFlagsBitmask));

  assert(argList && "No input argument list.");

  // Check for missing argument error.
  if (missingArgCount) {
    cos << "D(SrcLoc(),"
        << "msg::error_missing_arg_value,"
        << "argList->getArgString(missingArgIndex),"
        << "missingArgCount" << '\n';
    return nullptr;
  }

  // Check for unknown arguments.
  for (const llvm::opt::Arg *arg : argList->filtered(opts::UNKNOWN)) {
    cos << "D(SourceLoc(), "
        << "msg::error_unknown_arg,"
        << "arg->getAsString(*ArgList));" << '\n';
    return nullptr;
  }
  return argList;
}
std::unique_ptr<llvm::opt::DerivedArgList>
Session::TranslateArgList(const llvm::opt::InputArgList &args) {
  auto dArgList = llvm::make_unique<llvm::opt::DerivedArgList>(args);

  for (Arg *arg : args) {
    dArgList->append(arg);
  }
  return dArgList;
}

void Session::SetTargetTriple(const llvm::Triple &triple) {
  // TODO: langOpts.SetTarget(triple);
}
void Session::SetTargetTriple(llvm::StringRef triple) {
  SetTargetTriple(llvm::Triple(triple));
}

void Session::ComputeMode(const llvm::opt::DerivedArgList &args) {
  assert(mode.GetType() == ModeType::None && "mode id already computed");
  const llvm::opt::Arg *const modeArg = args.getLastArg(opts::ModeGroup);

  // TODO: may have to claim
  if (modeArg) {
    switch (modeArg->getOption().getID()) {
    case opts::Parse:
      mode.SetType(ModeType::Parse);
      break;
    case opts::Check:
      mode.SetType(ModeType::Check);
      break;
    case opts::EmitIR:
      mode.SetType(ModeType::EmitIR);
      break;
    case opts::EmitBC:
      mode.SetType(ModeType::EmitBC);
      break;
    case opts::EmitObject:
      mode.SetType(ModeType::EmitObject);
      break;
    case opts::EmitAssembly:
      mode.SetType(ModeType::EmitAssembly);
      break;
    case opts::EmitLibrary:
      mode.SetType(ModeType::EmitLibrary);
      break;
    default:
      break;
    }
  }
  if (mode.GetType() == ModeType::None) {
    mode.SetType(GetDefaultModeType());
  }
}

void Session::Purge() {}

void Session::Finish() {
  timer.stopTimer();
  Purge();
  PrintDiagnostics();
  PrintStatistics();
}
void Session::PrintDiagnostics() { GetDiagEngine().Print(); }

void Session::PrintStatistics() {
  if (sessionOpts.printStats) {
    GetStatEngine().Print();
  }
}

/*
static llvm::StringRef Mode::GetNameByKind(ModeType kind) {
        //TODO: I think you can get these from the mode group
        // so that you do not have to dublicate the text -- fragile.
  switch (kind) {
  case ModeType::Parse:
    return "parse";
        case Modekind::Check :
                return "check";
  case ModeType::EmitIR:
    return "emit-ir";
  case ModeType::EmitBC:
    return "emit-bc";
  case ModeType::EmitObject:
    return "emit-object";
  case ModeType::EmitAssembly:
    return "emit-assembly";
  case ModeType::EmitLibrary:
    return "emit-library";
  case ModeType::EmitExecutable:
    return "emit-executable";
  }
        llvm_unreachable("Invalid ModeType.");
}
*/

void Session::PrintVersion() {}
