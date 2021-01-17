

class InputActivityArgs {};

class InputActivityListener {
 public:
  virtual void OnInputEntered(InputActivityArgs &args) = 0;
};

int main() { return 0; }
