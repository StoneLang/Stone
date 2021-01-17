// Include the common option parsing interfaces.
include "llvm/Option/OptParser.td"

/////////
// Flags

// The option should be accepted by cram 
def CompileOption : OptionFlag;

// The option should not be accepted by the build.
def NoCompileOption : OptionFlag;

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
Flags<[CompileOption]>;

def Check : Flag<["-"], "check">,
HelpText<"Parse and type-check input file(s)">, ModeOpt,
Flags<[CompileOption]>;

def EmitLibrary : Flag<["-"], "emit-library">,
HelpText<"Emit a linked library">, ModeOpt,
Flags<[CompileOption]>;

def EmitObject : Flag<["-"], "emit-object">,
HelpText<"Emit object file(s) (-C)">, ModeOpt,
Flags<[CompileOption]>;


def EmitExecutable : Flag<["-"], "emit-executable">,
HelpText<"Emit object file(s) (-C)">, ModeOpt,
Flags<[DriverOption]>;


def C : Flag<["-"], "C">, Alias<EmitObject>,
Flags<[CompileOption]>, ModeOpt;

def EmitAssembly : Flag<["-"], "emit-assembly">,
HelpText<"Emit assembly file(s) (-S)">, ModeOpt,
Flags<[CompileOption]>;

def S: Flag<["-"], "S">, Alias<EmitAssembly>,
Flags<[CompileOption]>, ModeOpt;

def EmitBC : Flag<["-"], "emit-bc">,
HelpText<"Emit LLVM BC file(s)">, ModeOpt,
Flags<[CompileOption]>;

def EmitIR : Flag<["-"], "emit-ir">,
HelpText<"Emit LLVM IR file(s)">, ModeOpt,
Flags<[CompileOption]>;

def DumpAST : Flag<["-"], "dump-ast">,
HelpText<"Parse and type-check input file(s) and dump AST(s)">, ModeOpt,
Flags<[CompileOption]>;

def Help : Flag<["-", "--"], "help">, 
Flags<[CompileOption, DriverOption]>,
HelpText<"Display available options">, ModeOpt;

def h : Flag<["-"], "h">, Alias<Help>;

def HelpSecrete : Flag<["-", "--"], "help-hidden">,
Flags<[CompileOption]>,
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
Flags<[CompileOption, DriverOption]>,
HelpText<"Generate code for the given target">;

def TargetVariant : Separate<["-"], "target-variant">,
  Flags<[CompileOption, DriverOption]>,
  HelpText<"Generate 'zippered' code for macCatalyst that can run on the specified"
           " variant target triple in addition to the main -target triple">;

def TargetCPU : Separate<["-"], "target-cpu">, Flags<[CompileOption, DriverOption]>,
HelpText<"Generate code for a particular CPU variant">;

// DEV OPTIONS 

def SyncProc : Flag<["-"], "sync-proc">,
Flags<[DebugOption]>,
HelpText<"Run procs synchronously -- default is asynchronous">;

def ShowMode : Flag<["-"], "show-action">,
Flags<[DebugOption]>,
HelpText<"Show every step in the lifecycle of the actions">;

def ShowTasks : Flag<["-"], "show-tasks">,
Flags<[DebugOption]>,
HelpText<"Show every step in the lifecycle of the actions">;

def ShowTime : Flag<["-"], "show-time">,
Flags<[DebugOption]>,
HelpText<"Show time in the lifecycle of actions and jobs">;

def ShowProcs : Flag<["-"], "show-procs">,
Flags<[DebugOption]>,
HelpText<"Show procs">;

def ShowLifecycle : Flag<["-"], "show-lifecycle">,
Flags<[DebugOption]>,
HelpText<"Show lifecycle of steps and procs">;


def MainFile : Separate<["-"], "main-file">,
  HelpText<"Produce output for this file, not the whole module">;
