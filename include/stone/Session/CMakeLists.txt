set(LLVM_TARGET_DEFINITIONS SessionOptions.td)
stone_tablegen(SessionOptions.inc -gen-opt-parser-defs)
stone_add_public_tablegen_target(StoneOptions)
