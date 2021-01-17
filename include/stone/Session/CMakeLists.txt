set(LLVM_TARGET_DEFINITIONS Options.td)
stone_tablegen(Options.inc -gen-opt-parser-defs)
stone_add_public_tablegen_target(StoneOptions)
