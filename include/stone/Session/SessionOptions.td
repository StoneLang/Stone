// Include the common option parsing interfaces.
include "llvm/Option/OptParser.td"

/////////
// Flags

// The option should be accepted by cram 
def CompilerOption : OptionFlag;

// The option should not be accepted by the build.
def NoCompilerOption : OptionFlag;

// The option should not be accepted by the build.
def DriverOption : OptionFlag;

// The option should not be accepted by the build.
def NoDriverOption : OptionFlag;

// The option should not be accepted by the build.
def DebugOption : OptionFlag;

def ModeGroup : OptionGroup<"<mode options>">, HelpText<"MODES">;
class ModeOpt : Group<ModeGroup>;


def InternalGroup : OptionGroup<"<stone internal options>">;
def InternalDebugGroup :
OptionGroup<"<stone debug/development internal options>">,
Group<InternalGroup>, HelpText<"DEBUG/DEVELOPMENT OPTIONS">;

class InternalDebugOpt : Group<InternalDebugGroup>,
Flags<[HelpHidden]>;


//COMPILE ACTIONS

def Parse: Flag<["-"], "parse">,
HelpText<"Parse input file(s)">, ModeOpt,
Flags<[CompilerOption]>;

def Check : Flag<["-"], "check">,
HelpText<"Parse and type-check input file(s)">, ModeOpt,
Flags<[CompilerOption]>;

def EmitLibrary : Flag<["-"], "emit-library">,
HelpText<"Emit a linked library">, ModeOpt,
Flags<[CompilerOption]>;

def EmitObject : Flag<["-"], "emit-object">,
HelpText<"Emit object file(s) (-C)">, ModeOpt,
Flags<[CompilerOption]>;


def EmitExecutable : Flag<["-"], "emit-executable">,
HelpText<"Emit object file(s) (-C)">, ModeOpt,
Flags<[DriverOption]>;


def C : Flag<["-"], "C">, Alias<EmitObject>,
Flags<[CompilerOption]>, ModeOpt;

def EmitAssembly : Flag<["-"], "emit-assembly">,
HelpText<"Emit assembly file(s) (-S)">, ModeOpt,
Flags<[CompilerOption]>;

def S: Flag<["-"], "S">, Alias<EmitAssembly>,
Flags<[CompilerOption]>, ModeOpt;

def EmitBC : Flag<["-"], "emit-bc">,
HelpText<"Emit LLVM BC file(s)">, ModeOpt,
Flags<[CompilerOption]>;

def EmitIR : Flag<["-"], "emit-ir">,
HelpText<"Emit LLVM IR file(s)">, ModeOpt,
Flags<[CompilerOption]>;

def Static : Flag<["-"], "static">,
  Flags<[CompilerOption]>,
  HelpText<"Make this module statically linkable and make the output of -emit-library a static library.">;


def DumpAST : Flag<["-"], "dump-ast">,
HelpText<"Parse and type-check input file(s) and dump AST(s)">, ModeOpt,
Flags<[CompilerOption]>;

def Help : Flag<["-", "--"], "help">, 
Flags<[CompilerOption, DriverOption]>,
HelpText<"Display available options">, ModeOpt;

def h : Flag<["-"], "h">, Alias<Help>;

def HelpSecrete : Flag<["-", "--"], "help-hidden">,
Flags<[CompilerOption]>,
HelpText<"Display available options, including hidden options">;


def Version : Flag<["-", "--"], "version">,
Flags<[DriverOption]>,
HelpText<"Print version information and exit">, ModeOpt;

def v : Flag<["-"], "v">, 
Flags<[DriverOption]>,
HelpText<"Show commands to run and use verbose output">;

def Link : Flag<["-"], "link">,
HelpText<"Specifies a library which should be linked against">, ModeOpt,
Flags<[DriverOption]>;

//GENERAL OPTIONS 
def Target : Separate<["-"], "target">,
Flags<[CompilerOption, DriverOption]>,
HelpText<"Generate code for the given target">;

def TargetVariant : Separate<["-"], "target-variant">,
  Flags<[CompilerOption, DriverOption]>,
  HelpText<"Generate 'zippered' code for macCatalyst that can run on the specified"
           " variant target triple in addition to the main -target triple">;

def TargetCPU : Separate<["-"], "target-cpu">, Flags<[CompilerOption, DriverOption]>,
HelpText<"Generate code for a particular CPU variant">;

// DEV OPTIONS 

def SyncJob : Flag<["-"], "sync-job">,
Flags<[DebugOption]>,
HelpText<"Run jobs synchronously -- default is asynchronous">;

def PrintMode : Flag<["-"], "print-mode">,
Flags<[DebugOption]>,
HelpText<"Print the current 'Compiler' or 'Driver' mode">;

def PrintTime : Flag<["-"], "print-time">,
Flags<[DebugOption]>,
HelpText<"Print the time of the 'Compiler' or 'Driver' lifecycle">;

def PrintStats : Flag<["-"], "print-stats">,
Flags<[DebugOption]>,
HelpText<"Print the 'Compiler' statistics">;


def PrintDriverActivities : Flag<["-"], "driver-print-activities">,
Flags<[DebugOption]>,
HelpText<"Print the list of activities to run">;


def PrintDriverJobs : Flag<["-"], "driver-print-jobs">,
Flags<[DebugOption]>,
HelpText<"Print the list of jobs to be executed">;

def PrintDriverLifecycle : Flag<["-"], "driver-print-lifecycle">,
Flags<[DebugOption]>,
HelpText<"Print the lifecycle of activities and jobs">;

def MainFile : Separate<["-"], "main-file">,
  HelpText<"Produce output for this file, not the whole module">;
