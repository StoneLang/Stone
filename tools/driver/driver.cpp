

class InputActivityArgs {};

class InputActivityListener {
 public:
  virtual void OnInput(InputArgs &args) = 0;
};

enum class CompilationJobKind { Compile, Link };

class CompilationUnit {
  const file::FileType ty;
  const llvm::opt::Arg &input;

 public:
};
class Job {
 public:
};

class CompileJob : public CompilationJob {
  CompilationRequest &request;
};

class LinkJob : public CompilationJob {};

int main() { return 0; }
